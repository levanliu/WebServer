#include "buffer.h"

Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

size_t Buffer::readableBytes() const {
    return writePos_ - readPos_;
}
size_t Buffer::writableBytes() const {
    return buffer_.size() - writePos_;
}

size_t Buffer::prependableBytes() const {
    return readPos_;
}

const char* Buffer::peek() const {
    return beginPtr_() + readPos_;
}

void Buffer::retrieve(size_t len) {
    assert(len <= readableBytes());
    readPos_ += len;
}

void Buffer::retrieveUntil(const char* end) {
    assert(peek() <= end );
    retrieve(end - peek());
}

void Buffer::retrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

std::string Buffer::retrieveAllToStr() {
    std::string str(peek(), readableBytes());
    retrieveAll();
    return str;
}

const char* Buffer::beginWriteConst() const {
    return beginPtr_() + writePos_;
}

char* Buffer::beginWrite() {
    return beginPtr_() + writePos_;
}

void Buffer::hasWritten(size_t len) {
    writePos_ += len;
} 

void Buffer::append(const std::string& str) {
    append(str.data(), str.length());
}

void Buffer::append(const void* data, size_t len) {
    assert(data);
    append(static_cast<const char*>(data), len);
}

void Buffer::append(const char* str, size_t len) {
    assert(str);
    ensureWriteable(len);
    std::copy(str, str + len, beginWrite());
    hasWritten(len);
}

void Buffer::append(const Buffer& buff) {
    append(buff.peek(), buff.readableBytes());
}

void Buffer::ensureWriteable(size_t len) {
    if(writableBytes() < len) {
        makeSpace_(len);
    }
    assert(writableBytes() >= len);
}

ssize_t Buffer::readFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = writableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = beginPtr_() + writePos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    }
    else {
        writePos_ = buffer_.size();
        append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno) {
    size_t readSize = readableBytes();
    ssize_t len = write(fd, peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;
    return len;
}

char* Buffer::beginPtr_() {
    return &*buffer_.begin();
}

const char* Buffer::beginPtr_() const {
    return &*buffer_.begin();
}

void Buffer::makeSpace_(size_t len) {
    if(writableBytes() + prependableBytes() < len) {
        buffer_.resize(writePos_ + len + 1);
    } 
    else {
        size_t readable = readableBytes();
        std::copy(beginPtr_() + readPos_, beginPtr_() + writePos_, beginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == readableBytes());
    }
}