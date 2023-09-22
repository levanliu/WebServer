#pragma once

#include <deque>
#include <condition_variable>
#include <sys/time.h>
#include <mutex>
#include <cassert>

template<class T>
class BlockDeque {
public:
  BlockDeque(BlockDeque &&) = delete;
  BlockDeque(const BlockDeque &) = delete;
  BlockDeque &operator=(BlockDeque &&) = delete;
  BlockDeque &operator=(const BlockDeque &) = delete;
  ~BlockDeque();

  explicit BlockDeque(std::size_t maxCapacity = 1000);

  void clear();
  bool empty();
  bool full();
  void close();
  std::size_t size();
  std::size_t capacity();

  T front();
  T back();

  void push_back(const T& item);
  void push_front(const T& item);
  bool pop(T& item);
  bool pop(T& item,int timeout);
  void flush();

private:
  std::deque<T> deq_;
  std::size_t capacity_;
  std::mutex mtx_;
  bool isClose_;
  std::condition_variable condConsumer_;
  std::condition_variable condProducer_;
};


template<class T>
BlockDeque<T>::BlockDeque(std::size_t maxCapacity):capacity_(maxCapacity){
  assert(maxCapacity > 0);
  isClose_ = false;
}

template<class T>
BlockDeque<T>::~BlockDeque()
{
  close();
}
