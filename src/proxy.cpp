#include <sys/select.h>
#include <sys/types.h>
#include <cstdio>
#include "Parser.hpp"
#include "Socket.hpp"

// int main() {
//   Socket s(NULL, "4444");
//   s.serverSocket();
//   while (true) {
//     int client_fd = s.connect2Client();
//     std::vector<char> reqbuffer(65535);
//     s.read2Buffer(client_fd, reqbuffer);
//     Parser newparser(reqbuffer);
//     newparser.parseGetnPost();
//     Socket s1(newparser.getHostName().c_str(), newparser.getPortNum().c_str());
//     s1.connect2Server();
//     reqbuffer = newparser.buildRequest();
//     int website_fd = s1.getSocketFd();
//     send(website_fd, reqbuffer.data(), reqbuffer.size(), 0);
//     // std::vector<char> newbuffer;
//     // while(s1.read2Buffer(website_fd, buffer)!= 0){
//     // newbuffer.insert(newbuffer.end(),buffer.begin(),buffer.end());
//     // }
//     // send(client_fd, newbuffer.data(), newbuffer.size(), 0);
//     std::vector<char> resbuffer(65535);
//     s1.read2Buffer(website_fd, resbuffer);
//     send(client_fd, resbuffer.data(), resbuffer.size(), 0);
//   }
//   return 0;
// }
void use_connect(Parser &newparser,int client_fd){
  Socket s1(newparser.getHostName().c_str(), "443");
  s1.connect2Server();
    int website_fd = s1.getSocketFd();
    std::string success= "HTTP/1.1 200 OK\r\n\r\n";
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
      int len =recv(client_fd, buffer.data(), BUFSIZ, 0);
      if ( len<= 0) {
        return;
      }
      send(website_fd, buffer.data(), len, 0);
    }
    else if (FD_ISSET(website_fd, &fdset)) {
      int len = recv(website_fd, buffer.data(), BUFSIZ, 0);
      if ( len<= 0) {
        return;
      }
      send(client_fd, buffer.data(), len, 0);
    }
}
}

void use_get(Parser &newparser,int client_fd){
  Socket s1(newparser.getHostName().c_str(), "80");
    s1.connect2Server();
    std::vector<char> buffer(BUFSIZ);
    buffer = newparser.buildRequest();
    int website_fd = s1.getSocketFd();
    send(website_fd, buffer.data(), BUFSIZ, 0);
    s1.read2Buffer(website_fd, buffer);
    send(client_fd, buffer.data(), BUFSIZ, 0);
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
    if(newparser.getMethod()=="CONNECT"){
      use_connect(newparser,client_fd);
    }
    else if((newparser.getMethod()=="GET")||(newparser.getMethod()=="POST")){
      use_get(newparser,client_fd);
    }
  //   Socket s1(newparser.getHostName().c_str(), "443");
  //   s1.connect2Server();
  //   int website_fd = s1.getSocketFd();
  //   std::string success= "HTTP/1.1 200 OK\r\n\r\n";
  //   send(client_fd, success.c_str(), success.size(), 0);
  //   int fdmax = (client_fd > website_fd) ? client_fd : website_fd;
  //   fd_set fdset;
  //   while (true) {
  //   FD_ZERO(&fdset);
  //   FD_SET(client_fd, &fdset);
  //   FD_SET(website_fd, &fdset);
  //   std::cout << "loop start\n";
  //   select(fdmax + 1, &fdset, NULL, NULL, NULL);
  //   if (FD_ISSET(client_fd, &fdset)) {
  //     int len =recv(client_fd, buffer.data(), BUFSIZ, 0);
  //     if ( len< 0) {
  //       break;
  //     }
  //     send(website_fd, buffer.data(), len, 0);
  //   }
  //   else if (FD_ISSET(website_fd, &fdset)) {
  //     std::cout<<"receive from website\n";
  //     int len = recv(website_fd, buffer.data(), BUFSIZ, 0);
  //     if ( len< 0) {
  //       break;
  //     }
  //     send(client_fd, buffer.data(), len, 0);
  //   }

  }
  return 0;
}