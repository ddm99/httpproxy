#include "Parser.hpp"

#include <sys/select.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Socket.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostringstream;
using std::string;

string readFileIntoString(const string & path) {
  ifstream input_file(path.c_str());
  if (!input_file.is_open()) {
    cerr << "Could not open the file - '" << path << "'" << endl;
  }
  return string((std::istreambuf_iterator<char>(input_file)),
                std::istreambuf_iterator<char>());
}

int main() {
  // string filename("req.txt");
  // string file_contents;
  // file_contents = readFileIntoString(filename);
  Socket s(NULL, "5896");
  s.serverSocket();
  std::vector<char> file_contents = s.receiveFromClient();
  Parser newparser(file_contents);
  newparser.parse_method();
  newparser.parse_hostname();
  newparser.parse_pathname();
  std::string port = "443";
  std::cout << newparser.getHostName() << "\n";
  Socket s1(newparser.getHostName().c_str(), port.c_str());
  int server_fd = s1.getSocketFd();
  int client_fd = s.getClient_connection_fd();
  std::cout << "S Status: " << s.getStatus() << s1.getStatus() << std::endl;
  std::cout << "Before sending to server\n";
  s.sendtoServer(file_contents);
  std::cout << "Before 200OK response\n";
  send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
  int fdmax = (client_fd > server_fd) ? client_fd : server_fd;
  // std::string newRequest = newparser.buildRequest();
  // std::cout << "Parsed Request:\n";
  // std::cout << newRequest;
  // s1.sendtoServer(std::vector<char>(newRequest.begin(), newRequest.end()));
  fd_set fdset;
  std::vector<char> buffer;
  while (true) {
    std::cout << "Reaching the while true loop\n";
    FD_ZERO(&fdset);
    FD_SET(client_fd, &fdset);
    FD_SET(server_fd, &fdset);
    select(fdmax + 1, &fdset, NULL, NULL, NULL);
    if (FD_ISSET(client_fd, &fdset)) {
      buffer = s.readBuffer(client_fd);
      send(server_fd, buffer.data(), buffer.size(), 0);
    }
    else if (FD_ISSET(server_fd, &fdset)) {
      buffer = s1.readBuffer(server_fd);
      send(client_fd, buffer.data(), buffer.size(), 0);
    }
  }
}