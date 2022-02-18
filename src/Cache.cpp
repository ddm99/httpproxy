#include <unordered_map>

#include "Request.hpp"
#include "Response.hpp"

class Cache {
  std::unordered_map<std::string, Response> cacheStorage;

 public:
  Cache() { cacheStorage.insert(std::make_pair(std::string(), Response())); }

  void insertElement(std::string url, Response & urlResponse) {
    cacheStorage.insert(std::make_pair(url, urlResponse));
  }

  void updateRevalidation(Response & current) {
    time_t now = time(0);
    current.needRevalidation =
        (difftime(current.responseReceivedTime, now)) > current.maxAge;
  }

  Response * lookupElement(std::string urlToFind) {
    std::unordered_map<std::string, Response>::iterator result =
        cacheStorage.find(urlToFind);
    if (result != cacheStorage.end()) {
      updateRevalidation(result->second);
      return &(result->second);
    }
    return NULL;
  }
};