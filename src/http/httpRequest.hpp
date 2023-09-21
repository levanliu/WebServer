#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../buffer/buffer.hpp"
#include "../log/log.hpp"
#include "../pool/sqlConnPool.hpp"
#include "../pool/sqlConnRAII.hpp"

class HttpRequest {
public:
  HttpRequest() { init(); }
  HttpRequest(HttpRequest &&) = delete;
  HttpRequest(const HttpRequest &) = delete;
  HttpRequest &operator=(HttpRequest &&) = delete;
  HttpRequest &operator=(const HttpRequest &) = delete;
  ~HttpRequest();

  enum PARSE_STATE {
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH,
  };

  enum HTTP_CODE {
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION,
  };

  void init();
  bool parse(Buffer &buff);

  std::string path() const;
  std::string &path();

  std::string method() const;
  std::string version() const;
  std::string getPost(const std::string &key) const;
  std::string getPost(const char *key) const;

  bool isKeepAlive() const;

private:
  bool parseRequestLine_(const std::string &line);
  void parseHeader_(const std::string &line);
  void parseBody_(const std::string &line);

  void parsePath_();
  void parsePost_();
  void parseFromUrlEncoded_();

  static bool userVerify(const std::string &name, const std::string &pwd,
                         bool isLogin);

  PARSE_STATE state_;

  std::string method_, path_, version_, body_;

  std::unordered_map<std::string, std::string> header_;
  std::unordered_map<std::string, std::string> post_;

  static const std::unordered_set<std::string> delete_HTML;
  static const std::unordered_map<std::string, int> delete_HTML_TAG;
  static int converHex(char ch);
};

