#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class Socket {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  std::string hostname;
  std::string port;

 public:
  //Basic constructor for creating a socket with the specified hostname and port
  Socket(std::string anotherHostName, std::string anotherPort) {
    hostname = anotherHostName;
    port = anotherPort;
    makeSocket();
  }

  //Function responsible for making a socket
  void makeSocket() {
    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname.c_str(), port.c_str(), &host_info, &host_info_list);

    if (status != 0) {
      std::cerr << "Error: cannot get address info for host" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      // return -1;
    }  //if

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
      std::cerr << "Error: cannot create socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      // return -1;
    }  //if

    std::cout << status << "\n";
  }
};
