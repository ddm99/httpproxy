#include <assert.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Request.hpp"
#include "Response.hpp"

class Parser {
 private:
  std::string message;
  std::unique_ptr<Request> parsed_message;

 public:
  Parser(std::vector<char> raw_message) {
    message = std::string(raw_message.begin(), raw_message.end());
    parsed_message = std::unique_ptr<Request>(new Request());
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
    if((start == std::string::npos)||(finish == std::string::npos)){
      throw std::invalid_argument("Cannot Parse Request\n");
    }
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
    if((start == std::string::npos)||(finish == std::string::npos)){
      throw std::invalid_argument("Cannot Parse Request\n");
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

  //Wrapper function for parsing user's request
  bool parseGetnPost() {
    try{
    parse_method();
    parse_hostname();
    parse_pathname();
    parsed_message->url = parsed_message->hostname + parsed_message->pathname;
    }catch (std::invalid_argument & e){
      std::cout<<e.what()<<std::endl;
      return false;
    }
    return true;
  }

  //Get Methods
  std::string getFirstline() {
    size_t finish = message.find("\n");
    std::string returnString = message.substr(0, finish - 1);
    return returnString;
  }
  std::string getUrl() { return parsed_message->url; }
  std::string getHostName() { return parsed_message->hostname; }
  std::string getMethod() { return parsed_message->method; }
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class ResponseParser {
  std::string responseMessage;
  std::unique_ptr<Response> responseParsed;

 public:
  ResponseParser(std::vector<char> response) {
    responseMessage = std::string(response.begin(), response.end());
    responseParsed = std::unique_ptr<Response>(new Response());
    responseParsed->content = response;
  }

  bool isCachable() {
    size_t status = responseMessage.find("200 OK");
    if (status == std::string::npos) {
      return false;
    }
    return true;
  }

  void parseMaxAge() {
    size_t start = responseMessage.find("max-age=") + 8;
    if (start == std::string::npos) {
      responseParsed->maxAge = 0;
    }
    else {
      size_t finish = responseMessage.find("\n", start);
      std::string tempMaxAge = responseMessage.substr(start, finish - start);
      responseParsed->maxAge = atoi(tempMaxAge.c_str());
    }
    std::cout << "max age is " << responseParsed->maxAge << std::endl;
  }

  void parseETag() {
    size_t start = responseMessage.find("Etag:") + 7;
    if (start == std::string::npos + 7) {
      responseParsed->Etag = "";
    }
    else {
      size_t finish = responseMessage.find("\n", start);
      responseParsed->Etag = responseMessage.substr(start, finish - start - 2);
    }
    std::cout << "Etag is " << responseParsed->Etag.c_str() << std::endl;
  }

  void parseResponseWrapper() {
    if (isCachable()) {
      parseMaxAge();
      parseETag();
    }
  }

  std::string getFirstline() {
    size_t finish = responseMessage.find("\n");
    std::string returnString = responseMessage.substr(0, finish - 1);
    return returnString;
  }

  Response getResponse() {
    Response result = *responseParsed;
    return result;
  }
};