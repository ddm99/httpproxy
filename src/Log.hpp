#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>

#include "Request.hpp"
#include "Response.hpp"
std::mutex loglock;
class Log {
  const std::string fileName = "var/log/erss/proxy.log";
  std::ofstream ofs;

 public:
  Log() {
    ofs.open(fileName, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
  }

  std::string getGMtime(time_t inputTime) {
    std::string result = asctime(gmtime(&inputTime));
    return result;
  }

  void writeToFile(std::string & content) {
    loglock.lock();
    ofs.open(fileName, std::ofstream::out | std::ofstream::app);
    if (ofs.is_open()) {
      ofs << content;
      ofs.close();
    }
    else {
      throw std::iostream::failure("Unable to open the log file");
    }
    loglock.unlock();
  }

  /*
In socket.hpp

std::string ipAddress(){
return std::string(host_info->ai_addr);
}
*/
  void writeRequest(size_t id, std::string request, std::string ipAddress) {
    std::string requestReceived = std::to_string(id) + ": " + request + " from " +
                                  ipAddress + "@" + getGMtime(time(0));
    writeToFile(requestReceived);
  }

  void writeBeforeSend(size_t id, std::string request, std::string url) {
    std::string beforeSend =
        std::to_string(id) + ": " + "Requesting " + request + " from " + url + "\n";
    writeToFile(beforeSend);
  }

  void writeAfterReceive(size_t id, std::string response, std::string url) {
    std::string afterSend =
        std::to_string(id) + ": " + "Received " + response + " from " + url + "\n";
    writeToFile(afterSend);
  }

  void writeTunnelClose(size_t id) {
    std::string toWrite = std::to_string(id) + ": " + "Tunnel closed\n";
    writeToFile(toWrite);
  }

  void writeNotinCache(size_t id) {
    std::string toWrite = std::to_string(id) + ": " + "not in cache\n";
    writeToFile(toWrite);
  }

  void writeinCacheExpired(size_t id, time_t inputTime) {
    std::string toWrite =
        std::to_string(id) + ": in cache, but expired at " + getGMtime(inputTime);
    writeToFile(toWrite);
  }

  void writeinCacheValid(size_t id) {
    std::string toWrite = std::to_string(id) + ": in cache, valid\n";
    writeToFile(toWrite);
  }

  void writeNotCacheable(size_t id) {
    std::string toWrite = std::to_string(id) + ": not cacheable because no-store\n";
    writeToFile(toWrite);
  }

  void writeCached(size_t id, time_t inputTime) {
    std::string toWrite =
        std::to_string(id) + ": cached, expires at " + getGMtime(inputTime);
    writeToFile(toWrite);
  }
};