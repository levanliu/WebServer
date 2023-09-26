

#pragma onceSQLCONNRAII_H
#define SQLCONNRAII_H
#include "sqlconnpool.h"

/* 资源在对象构造初始化 资源在对象析构时释放*/
class SqlConnRAII {
public:
    SqlConnRAII(MYSQL** sql, SqlConnPool *connpool) {
        assert(connpool);
        *sql = connpool->getConn();
        sql_ = *sql;
        connpool_ = connpool;
    }
    
    ~SqlConnRAII() {
        if(sql_) { connpool_->freeConn(sql_); }
    }
    
private:
    MYSQL *sql_;
    SqlConnPool* connpool_;
};

#endif //SQLCONNRAII_H