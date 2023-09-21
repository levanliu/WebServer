#pragma once

#include <netinet/in.h>
#include <sys/types.h>

#include "../buffer/buffer.hpp"
#include "../log/log.hpp"
#include "../pool/sqlConnRAII.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"

class HttpConn {
public:
  HttpConn();
  HttpConn(HttpConn &&) = delete;
  HttpConn(const HttpConn &) = delete;
  HttpConn &operator=(HttpConn &&) = delete;
  HttpConn &operator=(const HttpConn &) = delete;
  ~HttpConn();

  void init(int sockFd, const sockaddr_in &addr);
  std::size_t read(int *saveErrno);
  std::size_t write(int *saveErrno);
  void close();
  int getFd() const;
  int getPort() const;

  const char *getIP() const;
  sockaddr_in getAddr() const;

  bool process();

  int toWriteBytes() { return iov_[0].iov_len + iov_[1].iov_len; }
  bool isKeepAlive() const { return request_.isKeepAlive(); }

  static bool isET;
  static const char* srcDir;
  static std::atomic<int> userCount;

private:
  int fd_;
  struct sockaddr_in addr_;

  bool isClose_;
  struct iovec iov_[2];

  Buffer readBuff_;
  Buffer writeBuff_;

  HttpRequest request_;
  HttpResponse response_;
};
