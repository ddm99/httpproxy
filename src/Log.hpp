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

  const tm * getGMtime(size_t maxage) {
    time_t now = time(0);
    time(&now);
    now += maxage;
    return gmtime(&now);
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
  void writeRequest(size_t id, std::string request) {}
};