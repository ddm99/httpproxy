#include "Parser.hpp"

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
  string filename("req.txt");
  string file_contents;

  file_contents = readFileIntoString(filename);
  // cout << file_contents << endl;
  Parser newparser(file_contents);
  newparser.parse_method();
  newparser.parse_hostname();
  newparser.parse_pathname();
  std::string port = "80";
  Socket s1(newparser.getHostName().c_str(), port.c_str());
  s1.makeSocket();
  std::string message = "GET / HTTP/4.0";
  s1.sendtoServer(std::vector<char>(message.begin(), message.end()));
  std::vector<char> buffer(1024 * 1024);
  int status = recv(s1.getSocketFd(), buffer.data(), buffer.size(), 0);
  std::cout << status << " is the status\n";
  buffer[9] = 0;
  for (size_t i = 0; i < buffer.size(); i++) {
    std::cout << buffer[i];
  }
}