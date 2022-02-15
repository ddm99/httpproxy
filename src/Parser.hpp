#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class Request{
public:
    std::string method;
    std::string pathname;
    std::string hostname;
    Request():
        method(std::string()),
        pathname(std::string()),
        hostname(std::string()){}
};

class Response{

};

class Parser{
    private:
    std::string message;
    std::auto_ptr<Request> parsed_message;

    public:
    Parser(std::string raw_message){
        message=raw_message;
        parsed_message= std::auto_ptr<Request>(new Request());
    }

    void parse_method(){
        parsed_message->method = message.substr(0,message.find(" "));
        std::cout<<parsed_message->method<<std::endl;
    }

    void parse_pathname(){
        size_t skip = message.find("http://") +7;
        size_t start = message.find("/",skip);
        size_t finish = message.find(" ",start);
        parsed_message->pathname = message.substr(start,finish-start);
        std::cout<<parsed_message->pathname<<std::endl;
    }

    void parse_hostname(){
        size_t start = message.find("http://") +7;
        size_t finish = message.find("/",start);
        parsed_message->hostname = message.substr(start,finish-start);
        std::cout<<parsed_message->hostname<<std::endl;
    }
};