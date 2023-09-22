#include "webServer.hpp"
#include <asm-generic/socket.h>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

WebServer::WebServer(int port, int trigMode, int timeoutMs, bool openLinger,
                     int sqlPort, const char *sqlUser, const char *sqlPwd,
                     const char *dbName, int connPoolNum, int threadNum,
                     bool openLog, int logLevel, int logQueSize)
    : port_(port), openLinger_(openLinger), timeoutMs_(timeoutMs),
      isClose_(false), timer_(new HeapTimer()),
      threadPool_(new ThreadPool(threadNum)), epoller_(new Epoller()) {
  srcDir_ = getcwd(nullptr, 256);
  assert(srcDir_);
  strncat(srcDir_, "/resources/", 16);
  HttpConn::userCount = 0;
  HttpConn::srcDir = srcDir_;
  SqlConnPool::getInstance()->init("localhost", sqlPort, sqlUser, sqlPwd,
                                   dbName, connPoolNum);

  initEventMode_(trigMode);
  if (!initSocket_())
    isClose_ = true;

  if (openLog) {
    Log::getInstance()->init(logLevel, "./log", "./log", logQueSize);
    if (isClose_) {
      LOG_ERROR("============WebServer init error================");
    } else {
      LOG_INFO("============WebServer init===============");
      LOG_INFO("Port:%d,OpenLinger:%s", port_, openLinger ? "true" : "false");
      LOG_INFO("Listen Mode: %s,OpenConn Mode: %s",
               (listenEvent_ & EPOLLET ? "ET" : "LT"),
               (connEvent_ & EPOLLET ? "ET" : "LT"));
      LOG_INFO("LogSys level: %d", logLevel);
      LOG_INFO("srcDir: %s", HttpConn::srcDir);
      LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum,
               threadNum);
    }
  }
}

WebServer::~WebServer() {
  close(listenFd_);
  isClose_ = true;
  free(srcDir_);
  SqlConnPool::getInstance()->closePool();
}

void WebServer::initEventMode_(int trigMode) {
  listenEvent_ = EPOLLRDHUP;
  connEvent_ = EPOLLONESHOT | EPOLLRDHUP;
  switch (trigMode) {
  case 0:
    break;
  case 1:
    connEvent_ |= EPOLLET;
    break;
  case 2:
    listenEvent_ |= EPOLLET;
    break;
  case 3:
    connEvent_ |= EPOLLET;
    listenEvent_ |= EPOLLET;
    break;
  default:
    connEvent_ |= EPOLLET;
    listenEvent_ |= EPOLLET;
    break;
  }
  HttpConn::isET = (connEvent_ & EPOLLET);
}

void WebServer::start() {
  int timeMs = -1;
  if (!isClose_) {
    LOG_INFO("=================Server Start===================");
  }
  while (!isClose_) {
    if (timeoutMs_ > 0) {
      timeMs = timer_->getNextTick();
    }
    int eventCnt = epoller_->wait(timeMs);
    for (int i = 0; i < eventCnt; i++) {
      int fd = epoller_->getEventFd(i);
      uint32_t events = epoller_->getEvents(i);
      if (fd == listenFd_) {
        dealListen_();
      } else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
        assert(users_.count(fd) > 0);
        closeConn_(&users_[fd]);
      } else if (events & EPOLLIN) {
        assert(users_.count(fd) > 0);
        dealRead_(&users_[fd]);
      } else if (events & EPOLLOUT) {
        assert(users_.count(fd) > 0);
        dealWrite_(&users_[fd]);
      } else {
        LOG_ERROR("Unexpected event");
      }
    }
  }
}

void WebServer::sendError_(int fd, const char *info) {
  assert(fd > 0);
  int ret = send(fd, info, strlen(info), 0);
  if (ret < 0) {
    LOG_WARN("send error to client[%d] error!", fd);
  }
  close(fd);
}

void WebServer::closeConn_(HttpConn *client) {
  assert(client);
  LOG_INFO("Client[%d] quit!", client->getFd());
  epoller_->delFd(client->getFd());
  client->close_();
}

void WebServer::addClient_(int fd, sockaddr_in addr) {
  assert(fd > 0);
  users_[fd].init(fd, addr);
  if (timeoutMs_ > 0) {
    timer_->add(fd, timeoutMs_,
                std::bind(&WebServer::closeConn_, this, &users_[fd]));
  }
  epoller_->addFd(fd, EPOLLIN | connEvent_);
  setFdNonBlock(fd);
  LOG_INFO("Client[%d] in!", users_[fd].getFd());
}

void WebServer::dealListen_() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  do {
    int fd = accept(listenFd_, (struct sockaddr *)&addr, &len);
    if (fd <= 0) {
      return;
    } else if (HttpConn::userCount >= MAX_FD) {
      sendError_(fd, "Server busy!");
      LOG_WARN("Client is full!");
      return;
    }
    addClient_(fd, addr);
  } while (listenEvent_ & EPOLLET);
}

void WebServer::dealRead_(HttpConn *client) {
  assert(client);
  extentTime_(client);
  threadPool_->addTask(&WebServer::onRead_, this, client);
}

void WebServer::dealWrite_(HttpConn *client) {
  assert(client);
  extentTime_(client);
  threadPool_->addTask(&WebServer::onWrite_, this, client);
}

void WebServer::extentTime_(HttpConn *client) {
  assert(client);
  if (timeoutMs_ > 0) {
    timer_->adjust(client->getFd(), timeoutMs_);
  }
}

void WebServer::onRead_(HttpConn *client) {
  assert(client);
  int ret = -1;
  int readErrno = 0;
  ret = client->read(&readErrno);
  if (ret <= 0 && readErrno != EAGAIN) {
    closeConn_(client);
    return;
  }
  onProcess_(client);
}

void WebServer::onProcess_(HttpConn *client) {
  if (client->process()) {
    epoller_->modFd(client->getFd(), connEvent_ | EPOLLOUT);
  } else {
    epoller_->modFd(client->getFd(), connEvent_ | EPOLLIN);
  }
}

void WebServer::onWrite_(HttpConn *client) {
  assert(client);
  int ret = -1;
  int writeErrno = 0;
  ret = client->write(&writeErrno);
  if (client->toWriteBytes() == 0) {
    if (client->isKeepAlive()) {
      onProcess_(client);
      return;
    }
  } else if (ret < 0) {
    if (writeErrno == EAGAIN) {
      epoller_->modFd(client->getFd(), connEvent_ | EPOLLOUT);
      return;
    }
  }
  closeConn_(client);
}

bool WebServer::initSocket_() {
  int ret;
  struct sockaddr_in addr;
  if (port_ > 65535 || port_ < 1024) {
    LOG_ERROR("Port:%d error!", port_);
    return false;
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  struct linger openLinger = {0};
  if (openLinger_) {
    openLinger.l_onoff = 1;
    openLinger.l_linger = 1;
  }

  listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd_ < 0) {
    LOG_ERROR("Create socket error!", port_);
    return false;
  }

  ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &openLinger,
                   sizeof(openLinger));
  if (ret < 0) {
    close(listenFd_);
    LOG_ERROR("Init linger error!", port_);
    return false;
  }

  int optval = 1;
  ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,
                   sizeof(int));
  if (ret == -1) {
    LOG_ERROR("set socket setsockopt error!");
    close(listenFd_);
    return false;
  }

  ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    LOG_ERROR("Bind Port: %d setsockopt error!", port_);
    close(listenFd_);
    return false;
  }

  ret = listen(listenFd_, 6);
  if (ret < 0) {
    LOG_ERROR("Bind Port: %d setsockopt error!", port_);
    close(listenFd_);
    return false;
  }

  ret = epoller_->addFd(listenFd_, listenEvent_ | EPOLLIN);
  if (ret == 0) {
    LOG_ERROR("Add listen error!");
    close(listenFd_);
    return false;
  }

  setFdNonBlock(listenFd_);
  LOG_INFO("Server port:%d", port_);
  return true;
}

int WebServer::setFdNonBlock(int fd) {
  assert(fd > 0);
  return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}
