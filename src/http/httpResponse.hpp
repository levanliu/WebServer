#pragma once

#include_next <fcntl.h>
#include <string>
#include_next <sys/mman.h>
#include_next <sys/stat.h>
#include_next <unistd.h>
#include_next <unordered_map>

#include "../buffer/buffer.hpp"
#include "../log/log.hpp"

class HttpResponse {
public:
  HttpResponse();
  HttpResponse(HttpResponse &&) = delete;
  HttpResponse(const HttpResponse &) = delete;
  HttpResponse &operator=(HttpResponse &&) = delete;
  HttpResponse &operator=(const HttpResponse &) = delete;
  ~HttpResponse();

  void init(const std::string &srcDir, std::string &path,
            bool isKeepAlive = false, int code = -1);

  void makeResponse(Buffer &buff);
  void unmapFile();
  char *file();
  std::size_t fileLen() const;
  void errorContent(Buffer &buff, std::string message);
  int code() const { return code_; }

private:
  void addStateLine_(Buffer &buff);
  void addHeader_(Buffer &buff);
  void addContent_(Buffer &buff);

  void errorHtml_();
  std::string getFileType_();

  int code_;
  bool isKeepAlive_;
  std::string path_;
  std::string srcDir_;

  char *mmFile_;
  struct stat mmFileStat_;

  static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
  static const std::unordered_map<int, std::string> CODE_STATUS;
  static const std::unordered_map<int, std::string> CODE_PATH;
};
