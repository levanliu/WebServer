#pragma once

#include <assert.h>
#include <chrono>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <cassert>
#include <chrono>

// define if timeout will call callback function
typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds Ms;
typedef Clock::time_point TimeStamp;

struct TimerNode {
  int id;
  TimeStamp expires;
  TimeoutCallBack cb;

  //redefine <, compare two expires time.
  bool operator<(const TimerNode &t) { return expires < t.expires; }
};

class HeapTimer {
public:
  // heap_.size() increse to 64.
  HeapTimer() { heap_.reserve(64); }
  HeapTimer(HeapTimer &&) = delete;
  HeapTimer(const HeapTimer &) = delete;
  HeapTimer &operator=(HeapTimer &&) = delete;
  HeapTimer &operator=(const HeapTimer &) = delete;
  ~HeapTimer() { clear(); }

  void adjust(int id, int newExpires);
  void add(int id, int timeOut, const TimeoutCallBack &cb);
  void doWork(int id);
  void clear();
  void tick();
  void pop();
  int getNextTick();

private:
  void del_(std::size_t i);
  void shiftUp_(std::size_t i);
  bool shiftDown_(std::size_t index, std::size_t n);
  void swapNode_(std::size_t i, std::size_t j);
  std::vector<TimerNode> heap_;
  std::unordered_map<int, std::size_t> ref_;
};
