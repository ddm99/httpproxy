#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Request {
 public:
  std::string method;
  std::string pathname;
  std::string hostname;
  Request() : method(std::string()), pathname(std::string()), hostname(std::string()) {}
};

class Response {};

class Parser {
 private:
  std::string message;
  std::auto_ptr<Request> parsed_message;

 public:
  Parser(std::vector<char> raw_message) {
    message=std::string(raw_message.begin(),raw_message.end());
    parsed_message = std::auto_ptr<Request>(new Request());
  }

  void parse_method() {
    parsed_message->method = message.substr(0, message.find(" "));
    // std::cout << parsed_message->method << std::endl;
  }

  void parse_pathname() {
    size_t skip = message.find("http://") + 7;
    size_t start = message.find("/", skip);
    size_t finish = message.find(" ", start);
    parsed_message->pathname = message.substr(start, finish - start);
    // std::cout << parsed_message->pathname << std::endl;
  }

  void parse_hostname() {
    size_t start = message.find("http://") + 7;
    size_t finish = message.find("/", start);
    parsed_message->hostname = message.substr(start, finish - start);
    // std::cout << parsed_message->hostname << std::endl;
  }

  std::string getHostName() { return parsed_message->hostname; }

  std::string buildRequest() {
    std::string result;
    size_t start = message.find("Host");
    result = parsed_message->method + " " + parsed_message->pathname + " " +
              "HTTP/1.1\r\n" + message.substr(start);
    return result;
  }
};