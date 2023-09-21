#pragma once

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/uio.h>
#include <vector>
#include <atomic>
#include <assert.h>

class Buffer{
public:
  Buffer(int initBuffSize = 1024);
  Buffer(Buffer &&) = delete;
  Buffer(const Buffer &) = delete;
  Buffer &operator=(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;
  ~Buffer() = default;

  std::size_t writableBytes() const;
  std::size_t readableBytes() const;
  std::size_t preparedBytes() const;

  const char* peek() const;
  void ensureWriteable(std::size_t len);
  void hasWritten(std::size_t len);
  
  void retrieve(std::size_t len);
  void retrieveUntil(const char* end);

  void retrieveAll();
  std::string retrieveAllToStr();

  const char* beginWriteConst() const;
  char* beginWrite();

  void append(const std::string& str);
  void append(const char* str,std::size_t len);
  void append(const void* data,std::size_t len);
  void append(const Buffer& buff);

  std::size_t readFd(int fd,int* Errno);
  std::size_t writeFd(int fd,int* Errno);
  

private:
  char* beginPtr_();
  const char* beginPtr_() const;
  void makeSpace_(std::size_t len);

  std::vector<char> buffer_;
  std::atomic<std::size_t> readPos_;
  std::atomic<std::size_t> writePos_;
};
