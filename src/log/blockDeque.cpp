#include "blockDeque.hpp"
#include <cstddef>


template<class T>
void BlockDeque<T>::close()
{
  {
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.clear();
  isClose_ = true;
  }
  condProducer_.notify_all();
  condConsumer_.notify_all();
}


template<class T>
void BlockDeque<T>::flush(){
  condConsumer_.notify_one();
}

template<class T>
void BlockDeque<T>::clear(){
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.clear();
}


template<class T>
T BlockDeque<T>::front(){
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.front();
}


template<class T>
T BlockDeque<T>::back(){
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.back();
}


template<class T>
std::size_t BlockDeque<T>::size(){
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.size();
}


template<class T>
std::size_t BlockDeque<T>::capacity(){
  std::unique_lock<std::mutex> lock(mtx_);
  deq_.capacity();
}

template<class T>
void BlockDeque<T>::push_back(const T&item){
  std::unique_lock<std::mutex> lock(mtx_);
  while(deq_.size() >= capacity_){
    condProducer_.wait(lock);
  }
  deq_.push_back(item);
  condConsumer_.notify_one();
}


template<class T>
void BlockDeque<T>::push_front(const T&item){
  std::unique_lock<std::mutex> lock(mtx_);
  while(deq_.size() >= capacity_){
    condProducer_.wait(lock);
  }
  deq_.push_front(item);
  condConsumer_.notify_one();
}

template<class T>
bool BlockDeque<T>::empty(){
  std::unique_lock<std::mutex> lock(mtx_);
  return deq_.empty();
}

template<class T>
bool BlockDeque<T>::full(){
  std::unique_lock<std::mutex> lock(mtx_);
  return deq_.size() >= capacity_;
}

template<class T>
bool BlockDeque<T>::pop(T &item){
  std::unique_lock<std::mutex> lock(mtx_);
  while(deq_.empty()){
    condConsumer_.wait(lock);
    if(isClose_){
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}

template<class T>
bool BlockDeque<T>::pop(T &item,int timeout)
{
  std::unique_lock<std::mutex> lock(mtx_);
  while(deq_.empty()){
    if(condConsumer_.wait_for(lock,std::chrono::seconds(timeout))
            == std::cv_status::timeout){
      return false;
    }
    if(isClose_){
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}
