### WebServer
    c++ implements.
#### Operate System
    Linux


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
cmake -B build && cd build && make && cd .. && ./bin/Server
```
