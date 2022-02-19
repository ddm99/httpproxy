#include <map>

#include "Request.hpp"
#include "Response.hpp"
#include <mutex>
std::mutex thlock;
class Cache {
  std::map<std::string, Response> cacheStorage;

 public:
  Cache() { cacheStorage.insert(std::make_pair(std::string(), Response())); }

  void insertElement(std::string url, Response urlResponse) {
    thlock.lock();
    cacheStorage.insert(std::make_pair(url, urlResponse));
    thlock.unlock();
  }

  void updateRevalidation(Response & current) {
    time_t now = time(0);
    current.needRevalidation =
        (difftime(current.responseReceivedTime, now)) > current.maxAge;
  }

  void updateValue(std::string url, Response & newResponse) {
    thlock.lock();
    cacheStorage[url] = newResponse;
    thlock.unlock();
  }

  Response * lookupElement(std::string urlToFind) {
    std::map<std::string, Response>::iterator result = cacheStorage.find(urlToFind);
    if (result != cacheStorage.end()) {
      updateRevalidation(result->second);
      return &(result->second);
    }
    return NULL;
  }
};