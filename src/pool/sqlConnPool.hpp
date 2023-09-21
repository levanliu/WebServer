#pragma once

#include "../log/log.hpp"
#include <mutex>
#include <mysql/mysql.h>
#include <queue>
#include <semaphore.h>
#include <string>
#include <thread>

class SqlConnPool {
public:
  static SqlConnPool *getInstance();
  MYSQL *getConn();
  void freeConn(MYSQL *conn);
  int getFreeConnCount();
  void init(const char *host, int port, const char *user, const char *pwd,
            const char *dbName, int connSize);
  void closePool();

private:
  SqlConnPool();
  SqlConnPool(SqlConnPool &&) = delete;
  SqlConnPool(const SqlConnPool &) = delete;
  SqlConnPool &operator=(SqlConnPool &&) = delete;
  SqlConnPool &operator=(const SqlConnPool &) = delete;
  ~SqlConnPool();

  int MAX_CONN_;
  int useCount_;
  int freeCount_;

  std::queue<MYSQL*> connQue_;
  std::mutex mtx_;
  sem_t semId_;
};
