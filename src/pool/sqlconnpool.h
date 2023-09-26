#pragma onceSQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h"

class SqlConnPool {
public:
    static SqlConnPool *getInstance();

    MYSQL *getConn();
    void freeConn(MYSQL * conn);
    int GetFreeConnCount();

    void init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int connSize);
    void closePool();

private:
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;
    int useCount_;
    int freeCount_;

    std::queue<MYSQL *> connQue_;
    std::mutex mtx_;
    sem_t semId_;
};


#endif // SQLCONNPOOL_H