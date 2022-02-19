#ifndef RESPONSE_H
#define RESPONSE_H
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
class Response {
 public:
  size_t maxAge;
  std::string Etag;
  std::vector <char> content;
  time_t responseReceivedTime;
  bool needRevalidation;
  Response() :
      maxAge(0),
      Etag(std::string()),
      content(std::vector<char>()),
      responseReceivedTime(time(0)),
      needRevalidation(false) {}
};
#endif
