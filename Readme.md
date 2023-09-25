### WebServer
    c++ implements.
#### Operate System
    Linux

#### WebServer git:(master) tree .
➜.
├── CMakeLists.txt
├── Readme.md
├── src
│   ├── buffer
│   │   ├── buffer.cpp
│   │   ├── buffer.h
│   │   └── CMakeLists.txt
│   ├── CMakeLists.txt
│   ├── http
│   │   ├── CMakeLists.txt
│   │   ├── httpconn.cpp
│   │   ├── httpconn.h
│   │   ├── httprequest.cpp
│   │   ├── httprequest.h
│   │   ├── httpresponse.cpp
│   │   └── httpresponse.h
│   ├── log
│   │   ├── blockqueue.h
│   │   ├── CMakeLists.txt
│   │   ├── log.cpp
│   │   └── log.h
│   ├── main.cpp
│   ├── pool
│   │   ├── CMakeLists.txt
│   │   ├── sqlconnpool.cpp
│   │   ├── sqlconnpool.h
│   │   ├── sqlconnRAII.cpp
│   │   ├── sqlconnRAII.h
│   │   └── threadpool.h
│   ├── server
│   │   ├── CMakeLists.txt
│   │   ├── epoller.cpp
│   │   ├── epoller.h
│   │   ├── webserver.cpp
│   │   └── webserver.h
│   └── timer
│       ├── CMakeLists.txt
│       ├── heaptimer.cpp
│       └── heaptimer.h
├── test
│   ├── Makefile
│   └── test.cpp
└── webbench-1.5
    ├── Makefile
    ├── socket.c
    └── webbench.c

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
