### WebServer
    c++ implements.
#### Operate System
    Linux

#### WebServer git:(master) tree .
➜.
├── CMakeLists.txt
├── Readme.md
└── src
    ├── buffer
    │   ├── buffer.cpp
    │   ├── buffer.hpp
    │   └── CMakeLists.txt
    ├── CMakeLists.txt
    ├── http
    │   ├── CMakeLists.txt
    │   ├── httpConn.cpp
    │   ├── httpConn.hpp
    │   ├── httpParse.hpp
    │   ├── httpRequest.cpp
    │   ├── httpRequest.hpp
    │   ├── httpResponse.cpp
    │   └── httpResponse.hpp
    ├── log
    │   ├── blockDeque.cpp
    │   ├── blockDeque.hpp
    │   ├── CMakeLists.txt
    │   ├── log.cpp
    │   └── log.hpp
    ├── main.cpp
    ├── pool
    │   ├── CMakeLists.txt
    │   ├── sqlConnPool.cpp
    │   ├── sqlConnPool.hpp
    │   ├── sqlConnRAII.cpp
    │   ├── sqlConnRAII.hpp
    │   └── threadPool.hpp
    ├── server
    │   ├── CMakeLists.txt
    │   ├── epoller.cpp
    │   ├── epoller.hpp
    │   ├── webServer.cpp
    │   └── webServer.hpp
    └── timer
        ├── CMakeLists.txt
        ├── heapTimer.cpp
        └── heapTimer.hpp

```bash

create database webserver;

use webserver;

create table user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

insert into user(username,password) values('levan','123456');

```

```
sudo apt install libmysqlclient-dev
```

```bash
cmake -B build && cd build && make && cd .. && ./bin/webServer
```
