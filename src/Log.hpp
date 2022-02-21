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
    std::cout << "Coming here\n";
    ofs.open(fileName, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
  }

  std::string getGMtime(size_t maxage) {
    time_t now = time(0);
    time(&now);
    now += maxage;
    std::string result = asctime(gmtime(&now));
    return result;
  }

  void writeToFile(std::string & content) {
    loglock.lock();
    ofs.open(fileName, std::ofstream::out | std::ofstream::app);
    if (ofs.is_open()) {
      std::cout << "The file is open\n";
    }
    ofs << content;
    ofs.close();
    loglock.unlock();
  }

  /*
In socket.hpp

std::string ipAddress(){
return std::string(host_info->ai_addr);
}
*/
  void writeRequest(size_t id, std::string request, std::string ipAddress) {
    std::string requestReceived =
        std::to_string(id) + ": " + request + " from " + ipAddress + "@" + getGMtime(0);
    writeToFile(requestReceived);
  }

  void writeBeforeSend(size_t id, std::string request, std::string url) {
    std::string beforeSend =
        std::to_string(id) + ": " + " Requesting " + request + " from " + url;
    writeToFile(beforeSend);
  }
  void writeAfterSend(size_t id, std::string response, std::string url) {
    std::string afterSend =
        std::to_string(id) + ": " + " Received " + response + " from " + url;
    writeToFile(afterSend);
  }
};