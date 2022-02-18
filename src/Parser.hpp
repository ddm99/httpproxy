#include <assert.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Request {
 public:
  std::string method;
  std::string pathname;
  std::string hostname;
  std::string portnum;
  Request() :
      method(std::string()),
      pathname(std::string()),
      hostname(std::string()),
      portnum(std::string()) {}
};

class Parser {
 private:
  std::string message;
  std::auto_ptr<Request> parsed_message;

 public:
  Parser(std::vector<char> raw_message) {
    message = std::string(raw_message.begin(), raw_message.end());
    parsed_message = std::auto_ptr<Request>(new Request());
    parsed_message->portnum = "80";
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
    size_t start = std::string::npos;
    size_t finish = std::string::npos;
    start = message.find("Host:") + 6;
    if (parsed_message->method == "CONNECT") {
      finish = message.find(":", start);
    }
    else {
      finish = message.find("\r\n", start);
    }
    parsed_message->hostname = message.substr(start, finish - start);
    //std::cout << parsed_message->hostname << std::endl;
  }

  std::vector<char> buildRequest() {
    std::string result;
    size_t start = message.find("Host");
    result = parsed_message->method + " " + parsed_message->pathname + " " +
             "HTTP/1.1\r\n" + message.substr(start);
    std::vector<char> newResult(result.begin(), result.end());
    return newResult;
  }

  void parseGetnPost() {
    parse_method();
    parse_hostname();
    parse_pathname();
  }

  std::string getHostName() { return parsed_message->hostname; }
  std::string getMethod() { return parsed_message->method; }

  std::auto_ptr<Request> getParsed_message() { return parsed_message; }
};

class Response {
 public:
  size_t maxAge;
  std::string Etag;
  time_t responseReceivedTime;
  Response() : maxAge(0), Etag(std::string()), responseReceivedTime(time(0)) {}
};

class ResponseParser {
  std::string responseMessage;
  std::auto_ptr<Response> responseParsed;

 public:
  ResponseParser(std::vector<char> response) {
    responseMessage = std::string(response.begin(), response.end());
    responseParsed = std::auto_ptr<Response>(new Response());
  }

  size_t convertStringToNumber(std::string originalString) {
    std::stringstream newStream(originalString);
    size_t result;
    newStream >> result;
    return result;
  }

  void parseMaxAge() {
    size_t start = responseMessage.find("max-age=");
    if (start == std::string::npos) {
      responseParsed->maxAge = 0;
    }
    else {
      size_t finish = responseMessage.find('\r\n', start + 8);
      std::string tempMaxAge = responseMessage.substr(start, finish - start);
      responseParsed->maxAge = convertStringToNumber(tempMaxAge);
    }
    std::cout << responseParsed->maxAge << std::endl;
  }
};