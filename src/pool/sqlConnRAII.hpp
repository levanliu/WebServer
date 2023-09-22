#pragma once

#include "sqlConnPool.hpp"
#include <cassert>
class SqlConnRAII {
public:
  SqlConnRAII(MYSQL **sql, SqlConnPool *connPool);
  SqlConnRAII(SqlConnRAII &&) = delete;
  SqlConnRAII(const SqlConnRAII &) = delete;
  SqlConnRAII &operator=(SqlConnRAII &&) = delete;
  SqlConnRAII &operator=(const SqlConnRAII &) = delete;
  ~SqlConnRAII();

private:
  MYSQL *sql_;
  SqlConnPool *connPool_;
};
