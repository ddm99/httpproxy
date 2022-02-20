#include "Parser.hpp"

#include <sys/select.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Log.hpp"
#include "Socket.hpp"
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostringstream;
using std::string;

int main() {
  Log newLog;
  std::string timenow = asctime(newLog.getGMtime(0));
  newLog.writeToFile(timenow);
  return 0;
}