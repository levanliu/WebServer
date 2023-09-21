#pragma once

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

class Epoller {
public:
  Epoller() = delete;
  // 2^20 = 1024*1024 = one million level
  explicit Epoller(std::size_t maxEvent = (1<<20) );
  Epoller(Epoller &&) = delete;
  Epoller(const Epoller &) = delete;
  Epoller &operator=(Epoller &&) = delete;
  Epoller &operator=(const Epoller &) = delete;
  ~Epoller();

  bool addFd(int fd, uint32_t events);
  bool modFd(int fd, uint32_t events);
  bool delFd(int fd);
  int wait(int timeoutMs = -1);
  int getEventFd(std::size_t i) const;
  uint32_t getEvents(std::size_t i) const;

private:
  int epollFd_;
  std::vector<struct epoll_event> events_;
};
