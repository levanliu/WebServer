#include "sqlConnPool.hpp"
#include <cassert>
#include <mutex>
#include <mysql/mysql.h>
#include <pthread.h>
#include <semaphore.h>

SqlConnPool::SqlConnPool() {
  useCount_ = 0;
  freeCount_ = 0;
}

SqlConnPool *SqlConnPool::getInstance() {
  SqlConnPool *connPool = nullptr;
  if (connPool == nullptr) {
    std::mutex mtx_;
    auto lock = std::unique_lock<std::mutex>(mtx_);
    if (connPool == nullptr) {
      connPool = new SqlConnPool();
    }
  }
  return connPool;
}

void SqlConnPool::init(const char *host, int port, const char *user,
                       const char *pwd, const char *dbName, int connSize = 10) {
  assert(connSize > 0);
  for (int i = 0; i < connSize; i++) {
    MYSQL *sql = nullptr;
    sql = mysql_init(sql);
    if (!sql) {
      LOG_ERROR("MySQL init error");
      assert(sql);
    }

    sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);

    if(!sql){
      LOG_ERROR("MySql Connect error");
    }
    connQue_.push(sql);
  }
  MAX_CONN_ = connSize;
  sem_init(&semId_, 0, MAX_CONN_);
}

MYSQL* SqlConnPool::getConn(){
  MYSQL* sql = nullptr;
  if(connQue_.empty()){
    LOG_WARN("SqlConnPool busy!");
    return nullptr;
  }
  sem_wait(&semId_);
  {
    auto lock = std::unique_lock<std::mutex>(mtx_);
    sql = connQue_.front();
    connQue_.pop();
  }
  return sql;
}


void SqlConnPool::freeConn(MYSQL* sql)
{
  assert(sql);
  auto lock = std::unique_lock<std::mutex>(mtx_);
  connQue_.push(sql);
  sem_post(&semId_);
}

void SqlConnPool::closePool()
{
  auto lock = std::unique_lock<std::mutex>(mtx_);
  while (!connQue_.empty()) {
    auto item = connQue_.front();
    connQue_.pop();
    mysql_close(item);
  }
  mysql_library_end();
}

int SqlConnPool::getFreeConnCount(){
  auto lock = std::unique_lock<std::mutex>(mtx_);
  return connQue_.size();
}

SqlConnPool::~SqlConnPool()
{
  closePool();
}
