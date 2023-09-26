### WebServer
    c++ 14 implements. 
#### Operate System
    Linux ubuntu 20.04 docker

#### WebServer git:(master) tree .
```bash
➜.
.
├── CMakeLists.txt
├── Readme.md
├── resources
│   ├── 400.html
│   ├── 403.html
│   ├── 404.html
│   ├── 405.html
│   ├── css
│   │   ├── animate.css
│   │   ├── bootstrap.min.css
│   │   ├── font-awesome.min.css
│   │   ├── magnific-popup.css
│   │   └── style.css
│   ├── error.html
│   ├── fonts
│   │   ├── FontAwesome.otf
│   │   ├── fontawesome-webfont.eot
│   │   ├── fontawesome-webfont.svg
│   │   ├── fontawesome-webfont.ttf
│   │   ├── fontawesome-webfont.woff
│   │   └── fontawesome-webfont.woff2
│   ├── images
│   │   ├── favicon.ico
│   │   ├── instagram-image1.jpg
│   │   ├── instagram-image2.jpg
│   │   ├── instagram-image3.jpg
│   │   ├── instagram-image4.jpg
│   │   ├── instagram-image5.jpg
│   │   └── profile-image.jpg
│   ├── index.html
│   ├── js
│   │   ├── bootstrap.min.js
│   │   ├── custom.js
│   │   ├── jquery.js
│   │   ├── jquery.magnific-popup.min.js
│   │   ├── magnific-popup-options.js
│   │   ├── smoothscroll.js
│   │   └── wow.min.js
│   ├── login.html
│   ├── picture.html
│   ├── register.html
│   ├── video
│   │   └── xxx.mp4
│   ├── video.html
│   └── welcome.html
├── src
│   ├── buffer
│   │   ├── buffer.cpp
│   │   ├── buffer.h
│   │   └── CMakeLists.txt
│   ├── CMakeLists.txt
│   ├── http
│   │   ├── CMakeLists.txt
│   │   ├── httpconn.cpp
│   │   ├── httpconn.h
│   │   ├── httprequest.cpp
│   │   ├── httprequest.h
│   │   ├── httpresponse.cpp
│   │   └── httpresponse.h
│   ├── log
│   │   ├── blockqueue.h
│   │   ├── CMakeLists.txt
│   │   ├── log.cpp
│   │   └── log.h
│   ├── main.cpp
│   ├── pool
│   │   ├── CMakeLists.txt
│   │   ├── sqlconnpool.cpp
│   │   ├── sqlconnpool.h
│   │   ├── sqlconnRAII.cpp
│   │   ├── sqlconnRAII.h
│   │   └── threadpool.h
│   ├── server
│   │   ├── CMakeLists.txt
│   │   ├── epoller.cpp
│   │   ├── epoller.h
│   │   ├── webserver.cpp
│   │   └── webserver.h
│   └── timer
│       ├── CMakeLists.txt
│       ├── heaptimer.cpp
│       └── heaptimer.h
├── test
│   ├── Makefile
│   └── test.cpp
└── webbench-1.5
    ├── Makefile
    ├── socket.c
    └── webbench.c
```

```bash

create database webserver;

use webserver;

create table user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

insert into user(username,password) values('peter','123456');

```

### dependance
```
cmake build-essential mysql
```

```
sudo apt install libmysqlclient-dev
```

```bash
cmake -B build && cd build && make && cd .. && ./bin/webServer
```
