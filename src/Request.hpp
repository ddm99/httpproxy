#include <cstdio>
#include <cstring>
#include <string>

class Request {
 public:
  std::string method;
  std::string pathname;
  std::string hostname;
  std::string portnum;
  std::string url;
  Request() :
      method(std::string()),
      pathname(std::string()),
      hostname(std::string()),
      portnum(std::string()),
      url(std::string()) {}
};