#include "httpConn.hpp"
using namespace std;

const char* HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn() {
    fd_ = -1;
    addr_ = { 0 };
    isClose_ = true;
};

HttpConn::~HttpConn() {
    close_();
};

void HttpConn::init(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    writeBuff_.retrieveAll();
    readBuff_.retrieveAll();
    isClose_ = false;
    LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, getIP(), getPort(), (int)userCount);
}

void HttpConn::close_() {
    response_.unmapFile();
    if(isClose_ == false){
        isClose_ = true;
        userCount--;
        close(fd_);
        LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, getIP(), getPort(), (int)userCount);
    }
}

int HttpConn::getFd() const {
    return fd_;
};

struct sockaddr_in HttpConn::getAddr() const {
    return addr_;
}

const char* HttpConn::getIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int HttpConn::getPort() const {
    return addr_.sin_port;
}

std::size_t HttpConn::read(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuff_.readFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

std::size_t HttpConn::write(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = writev(fd_, iov_, iovCnt_);
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len  == 0) { break; } /* 传输结束 */
        else if(static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t*) iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if(iov_[0].iov_len) {
                writeBuff_.retrieveAll();
                iov_[0].iov_len = 0;
            }
        }
        else {
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuff_.retrieve(len);
        }
    } while(isET || toWriteBytes() > 10240);
    return len;
}

bool HttpConn::process() {
    request_.init();
    if(readBuff_.readableBytes() <= 0) {
        return false;
    }
    else if(request_.parse(readBuff_)) {
        LOG_DEBUG("%s", request_.path().c_str());
        response_.init(srcDir, request_.path(), request_.isKeepAlive(), 200);
    } else {
        response_.init(srcDir, request_.path(), false, 400);
    }

    response_.makeResponse(writeBuff_);
    /* 响应头 */
    iov_[0].iov_base = const_cast<char*>(writeBuff_.peek());
    iov_[0].iov_len = writeBuff_.readableBytes();
    iovCnt_ = 1;

    /* 文件 */
    if(response_.fileLen() > 0  && response_.file()) {
        iov_[1].iov_base = response_.file();
        iov_[1].iov_len = response_.fileLen();
        iovCnt_ = 2;
    }
    LOG_DEBUG("filesize:%d, %d  to %d", response_.fileLen() , iovCnt_, toWriteBytes());
    return true;
}
