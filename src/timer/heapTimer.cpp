#include "heapTimer.hpp"

void HeapTimer::swapNode_(std::size_t i, std::size_t j) {
  assert(i >= 0 && i < heap_.size());
  assert(j >= 0 && j < heap_.size());
  std::swap(heap_[i], heap_[j]);

  // map<int,size_t>   heap_[i].id <---->  i
  ref_[heap_[i].id] = i;
  ref_[heap_[j].id] = j;
}

// shiftUP_ to be a small root heap.
void HeapTimer::shiftUp_(std::size_t i) {
  assert(i >= 0 && i < heap_.size());
  std::size_t j = (i - 1) / 2;
  while (j >= 0) {
    if (heap_[j] < heap_[i])
      break;

    swapNode_(i, j);
    // go on, find smallest value of a tree leaf based on i.
    i = j;
    j = (i - 1) / 2;
  }
}

// shiftDown_ to be a smaller root heap.
bool HeapTimer::shiftDown_(std::size_t index, std::size_t n) {
  assert(index >= 0 && index < heap_.size());
  assert(n >= 0 && n <= heap_.size());

  std::size_t i = index;
  std::size_t j = i * 2 + 1;
  while (j < n) {
    // find smaller son node.
    if (j + 1 < n && heap_[j + 1] < heap_[j])
      j++;
    if (heap_[i] < heap_[j])
      break;
    swapNode_(i, j);
    // go on, find smallest value of a tree based on index.
    i = j;
    j = i * 2 + 1;
  }
  return i > index;
}

void HeapTimer::add(int id, int timeout, const TimeoutCallBack &cb) {
  assert(id >= 0);
  std::size_t i;
  // map<int,size_t>   id <-----------> i (temp heap_ size())
  if (ref_.count(id) == 0) {
    // new node, insert node to heap_.end(), then shiftUp_ heap;
    i = heap_.size();
    ref_[id] = i;
    heap_.push_back({id, Clock::now() + Ms(timeout), cb});
    shiftUp_(i);
  } else {
    // existed node, shiftDown_ then shiftUp_ heap. ensure heap will be a
    // smaller root heap.
    i = ref_[id];
    heap_[i].expires = Clock::now() + Ms(timeout);
    heap_[i].cb = cb;
    if (!shiftDown_(i, heap_.size())) {
      shiftUp_(i);
    }
  }
}

// run callback function, then del i
void HeapTimer::doWork(int id) {
  if (heap_.empty() || ref_.count(id) == 0)
    return;

  std::size_t i = ref_[id];
  TimerNode node = heap_[i];
  node.cb();
  del_(i);
}

void HeapTimer::del_(std::size_t index) {
  assert(!heap_.empty() && index >= 0 && index < heap_.size());

  std::size_t i = index;
  std::size_t n = heap_.size() - 1;
  assert(i <= n);
  if (i < n) {
    swapNode_(i, n);
    if (!shiftDown_(i, n))
      shiftUp_(i);
  }
  ref_.erase(heap_.back().id);
  heap_.pop_back();
}

// weak up, call callback function, pop from heap_.
void HeapTimer::tick() {
  if (heap_.empty()) {
    return;
  }
  while (!heap_.empty()) {
    TimerNode node = heap_.front();
    if (std::chrono::duration_cast<Ms>(node.expires - Clock::now()).count() > 0)
      break;
    node.cb();
    pop();
  }
}

void HeapTimer::pop() {
  assert(!heap_.empty());
  del_(0);
}

void HeapTimer::clear() {
  ref_.clear();
  heap_.clear();
}

int HeapTimer::getNextTick() {
  tick();
  std::size_t res = -1;
  if (!heap_.empty()) {
    res = std::chrono::duration_cast<Ms>(heap_.front().expires - Clock::now())
              .count();
    if (res < 0)
      res = 0;
  }
  return res;
}
