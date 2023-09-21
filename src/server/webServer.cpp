#include "webServer.hpp"

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
      LOG_INFO("Port:%d,OpenLinger:%s", port_, OpenLinger ? "true" : "false");
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


void WebServer::initEventMode_(int trigMode){
  listenEvent_ = EPOLLRDHUP;
  connEvent_ = EPOLLONESHOT|EPOLLRDHUP;
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


void WebServer::start(){
  int timeMs = -1;
  if(!isClose_) { LOG_INFO("=================Server Start===================");}
  while (!isClose_) {
    if(timeoutMs_ > 0){
      timeMs = timer_->getNextTick();
    }
    int eventCnt = epoller_->wait(timeMs);
    for (int i=0; i<eventCnt; i++) {
            int fd = epoller_->getEventFd(i);
            uint32_t events = epoller_->getEvents(i);
      if(fd == listenFd_){
        dealListen_();
      }else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
        assert(users_.count(fd)>0);
        closeConn_(&users_[fd]);
      }else if(events & EPOLLIN){
        assert(users_.count(fd) > 0);
        dealRead_(&users_[fd]);
      }else if(events & EPOLLOUT){
        assert(users_.count(fd)>0);
        dealWrite_(&users_[fd]);
      }else{
        LOG_ERROR("Unexpected event");
      }
    }
  }
}
