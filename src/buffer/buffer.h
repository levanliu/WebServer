#pragma once

#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
class Buffer {
public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t writableBytes() const;       
    size_t readableBytes() const ;
    size_t prependableBytes() const;

    const char* peek() const;
    void ensureWriteable(size_t len);
    void hasWritten(size_t len);

    void retrieve(size_t len);
    void retrieveUntil(const char* end);

    void retrieveAll() ;
    std::string retrieveAllToStr();

    const char* beginWriteConst() const;
    char* beginWrite();

    void append(const std::string& str);
    void append(const char* str, size_t len);
    void append(const void* data, size_t len);
    void append(const Buffer& buff);

    ssize_t readFd(int fd, int* Errno);
    ssize_t writeFd(int fd, int* Errno);

private:
    char* beginPtr_();
    const char* beginPtr_() const;
    void makeSpace_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;
};

