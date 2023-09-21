#pragma once

#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.hpp"
#include "../log/log.hpp"
#include "../timer/heapTimer.hpp"
#include "../pool/threadPool.hpp"
#include "../pool/sqlConnPool.hpp"
#include "../pool/sqlConnRAII.hpp"
#include "../http/httpConn.hpp"


class WebServer {
public:
  WebServer(
    int port,int trigMode,int timeoutMs,bool openLinger,
    int sqlPort,const char* sqlUser,const char* sqlPwd,
    const char* dbName,int connPoolNum,int threadNum,
    bool openLog,int logLevel,int logQueSize
  );
  WebServer(WebServer &&) = delete;
  WebServer(const WebServer &) = delete;
  WebServer &operator=(WebServer &&) = delete;
  WebServer &operator=(const WebServer &) = delete;
  ~WebServer();

  void start();

private:
  bool initSocket_();
  void initEventMode_(int trigMode);
  void addClient_(int fd,sockaddr_in addr);

  void dealListen_();
  void dealWrite_(HttpConn* client);
  void dealRead_(HttpConn* client);

  void sendError_(int fd,const char* info);
  void extentTime_(HttpConn* client);
  void closeConn_(HttpConn* client);

  void onRead_(HttpConn* client);
  void onWrite_(HttpConn* client);
  void onProcess_(HttpConn* client);

  static const int MAX_FD = 65535;

  static int setFdNonBlock(int fd);

  int port_;
  bool openLinger_;
  int timeoutMs_;
  bool isClose_;
  int listenFd_;
  char* srcDir_;

  uint32_t listenEvent_;
  uint32_t connEvent_;

  std::unique_ptr<HeapTimer> timer_;
  std::unique_ptr<ThreadPool> threadPool_;
  std::unique_ptr<Epoller> epoller_;
  std::unordered_map<int,HttpConn> users_;
};

