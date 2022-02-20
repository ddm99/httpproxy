#include <fstream>

#include "Request.hpp"
#include "Response.hpp"

class Log {
  const std::string fileName = "var/log/erss/proxy.log";
  std::ofstream ofs;

 public:
  Log() {
    std::cout << "Coming here\n";
    ofs.open(fileName, std::ofstream::out | std::ofstream::trunc);
  }

  void writeToFile(std::string content) {
    if (ofs.is_open()) {
      std::cout << "The file is open\n";
    }
    std::string actualContent = content + "\n";
    ofs << actualContent;
    ofs.close();
  }
};