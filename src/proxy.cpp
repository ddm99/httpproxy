#include <sys/select.h>
#include <sys/types.h>

#include "Parser.hpp"
#include "Socket.hpp"

int main() {
  Socket s(NULL, "4444");
  s.serverSocket();
  while (true) {
    int client_fd = s.connect2Client();
    std::vector<char> buffer(65535);
    s.read2Buffer(client_fd, buffer);
    Parser newparser(buffer);
    newparser.parseGetnPost();
    Socket s1(newparser.getHostName().c_str(), newparser.getPortNum().c_str());
    s1.connect2Server();
    buffer = newparser.buildRequest();
    int website_fd = s1.getSocketFd();
    send(website_fd, buffer.data(), buffer.size(), 0);
    s1.read2Buffer(website_fd, buffer);
    send(client_fd, buffer.data(), buffer.size(), 0);
  }
  return 0;
}
