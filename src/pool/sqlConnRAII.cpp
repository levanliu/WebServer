#include "SqlConnRAII.hpp"

sqlConnPool::SqlConnRAII(MYSQL **sql, SqlConnPool *connPool) {
  assert(connPool);
  *sql = connPool->getConn();
  sql_ = *sql;
  connPool_ = connPool;
}
SqlConnRAII::~SqlConnRAII() {
  if (sql_) {
    connPool_->freeConn(sql_);
  }
}
