#include "epoller.hpp"
#include <cassert>
#include <cstddef>
#include <sys/epoll.h>

Epoller::Epoller(std::size_t maxEvent)
    : epollFd_(epoll_create(512)), events_(maxEvent) {
  assert(epollFd_ >= 0 && events_.size() > 0);
}

Epoller::~Epoller() { close(epollFd_); }

/**
 * addFd,modFd,delFd
 *
 * for redblacktree node.
 */

bool Epoller::addFd(int fd, uint32_t events) {
  if (fd < 0)
    return false;
  epoll_event ev = {0};
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::modFd(int fd, uint32_t events) {
  if (fd < 0)
    return false;
  epoll_event ev = {0};
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoller::delFd(int fd) {
  if (fd < 0)
    return false;
  epoll_event ev = {0};
  return 0 == epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev);
}

/**
 * watch -> doubleLinkedList
 * exist data or timeout will return.
 */

int Epoller::wait(int timeoutMs) {
  return epoll_wait(epollFd_, &events_[0], static_cast<int>(events_.size()),
                    timeoutMs);
}

/**
 * getEventFd
 */
int Epoller::getEventFd(std::size_t i) const {
  assert(i < events_.size() && i >= 0);
  return events_[i].data.fd;
}

/**
 * getEvents
 */
uint32_t Epoller::getEvents(std::size_t i) const {
  assert(i < events_.size() && i >= 0);
  return events_[i].events;
}
