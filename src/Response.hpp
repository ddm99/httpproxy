#include <cstdio>
#include <cstring>
#include <string>

class Response {
 public:
  size_t maxAge;
  std::string Etag;
  time_t responseReceivedTime;
  bool needRevalidation;
  Response() :
      maxAge(0),
      Etag(std::string()),
      responseReceivedTime(time(0)),
      needRevalidation(false) {}
};
