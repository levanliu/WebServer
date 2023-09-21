#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <unordered_map>
typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds Ms;
typedef Clock::time_point TimeStamp;

struct TimerNode {
  int id;
  TimeStamp expires;
  TimeoutCallBack cb;
  bool operator<(const TimerNode &t) { return expires < t.expires; }
};

class HeapTimer {
public:
  HeapTimer();
  HeapTimer(HeapTimer &&) = delete;
  HeapTimer(const HeapTimer &) = delete;
  HeapTimer &operator=(HeapTimer &&) = delete;
  HeapTimer &operator=(const HeapTimer &) = delete;
  ~HeapTimer();

private:
  void del_(std::size_t i);
  void shiftUp_(std::size_t i);
  void shiftDown_(std::size_t index, std::size_t n);
  void swapNode_(std::size_t i, std::size_t j);
  std::vector<TimerNode> heap_;
  std::unordered_map<int, std::size_t> ref_;
};
