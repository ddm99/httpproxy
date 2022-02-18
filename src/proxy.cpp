#include <sys/select.h>
#include <sys/types.h>

#include <cstdio>
#include <limits>

#include "Parser.hpp"
#include "Socket.hpp"

void use_connect(Parser & newparser, int client_fd) {
  Socket s1(newparser.getHostName().c_str(), "443");
  s1.connect2Server();
  int website_fd = s1.getSocketFd();
  std::string success = "HTTP/1.1 200 OK\r\n\r\n";
  send(client_fd, success.c_str(), success.size(), 0);
  int fdmax = (client_fd > website_fd) ? client_fd : website_fd;
  fd_set fdset;
  while (true) {
    FD_ZERO(&fdset);
    FD_SET(client_fd, &fdset);
    FD_SET(website_fd, &fdset);
    select(fdmax + 1, &fdset, NULL, NULL, NULL);
    std::vector<char> buffer(BUFSIZ);
    if (FD_ISSET(client_fd, &fdset)) {
      int len = recv(client_fd, buffer.data(), BUFSIZ, 0);
      if (len <= 0) {
        return;
      }
      while (len > 0) {
        int sendLength = send(website_fd, buffer.data(), len, 0);
        len = len - sendLength;
      }
    }
    else if (FD_ISSET(website_fd, &fdset)) {
      int len = recv(website_fd, buffer.data(), BUFSIZ, 0);
      if (len <= 0) {
        return;
      }
      while (len > 0) {
        int sendLength = send(client_fd, buffer.data(), len, 0);
        len = len - sendLength;
      }
    }
  }
}

void use_get(Parser & newparser, int client_fd) {
  Socket s1(newparser.getHostName().c_str(), "80");
  s1.connect2Server();
  std::vector<char> buffer(BUFSIZ);
  buffer = newparser.buildRequest();
  int website_fd = s1.getSocketFd();
  send(website_fd, buffer.data(), BUFSIZ, 0);
  std::vector<char> newbuffer;
  int len = 1;
  while (len > 0) {
    len = s1.read2Buffer(website_fd, buffer);
    newbuffer.insert(newbuffer.end(), buffer.begin(), buffer.begin() + len);
  }
  send(client_fd, newbuffer.data(), newbuffer.size(), 0);
}

int main() {
  Socket s(NULL, "4444");
  s.serverSocket();
  while (true) {
    int client_fd = s.connect2Client();
    std::vector<char> buffer(BUFSIZ);
    s.read2Buffer(client_fd, buffer);
    Parser newparser(buffer);
    newparser.parseGetnPost();
    if (newparser.getMethod() == "CONNECT") {
      use_connect(newparser, client_fd);
    }
    else if ((newparser.getMethod() == "GET") || (newparser.getMethod() == "POST")) {
      use_get(newparser, client_fd);
    }
  }
  return 0;
}