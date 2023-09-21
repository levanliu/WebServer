#pragma once

#include "../buffer/buffer.hpp"
#include "blockDeque.hpp"
#include <assert.h>
#include <memory>
#include <mutex>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <thread>

class Log {
public:
  Log(Log &&) = delete;
  Log(const Log &) = delete;
  Log &operator=(Log &&) = delete;
  Log &operator=(const Log &) = delete;
  ~Log();

  void init(int level, const char *path = "./log", const char *suffic = ".log",
            int macQueueCapacity = 1024);

  static Log *getInstance();
  static void flushLogThread();

  void write(int level, const char *format, ...);
  void flush();

  int getLevel();
  void setLevel(int level);
  bool isOpen() { return isOpen_; }

private:
  Log();
  void appendLogLevelTitle_(int level);
  void asyncWrite_();

private:
  static const int LOG_PATH_LEN = 256;
  static const int LOG_NAME_LEN = 256;
  static const int MAX_LINES = 50000;

  const char *path_;
  const char *suffix_;

  int MAX_LENES_;
  int lineCount_;
  int toDay_;

  bool isOpen_;
  Buffer buff_;
  int level_;
  bool isAsync_;

  FILE *fp_;
  std::unique_ptr<BlockDeque<std::string>> deque_;
  std::unique_ptr<std::thread> writeThread_;
  std::mutex mtx_;
};

#define LOG_BASE(level, format, ...)\
    do {\
          Log *log = Log::getInstance(); \
          if (log->isOpen() && log->getLevel() <= level) { \
                   log->write(level, format, ##__VA_ARGS__); \
                   log->flush(); \
          } \
    } while (0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)}while (0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)}while (0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)}while (0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)}while (0);

