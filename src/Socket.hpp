#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
class Socket {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  const char * hostname;
  const char * port;

 public:
  //Basic constructor for creating a socket with the specified hostname and port
  Socket(const char * anotherHostName, const char * anotherPort) {
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

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);

    if (status != 0) {
      throw std::runtime_error("Cannot get address info for host");
      // return -1;
    }  //if

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
      throw std::runtime_error("Cannot create socket");
    }  //if
  }

  //Establishes a socket for the server side
  int serverSocket() {
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      throw std::runtime_error("Unable to bind socket");
    }  //if

    status = listen(socket_fd, 100);
    if (status == -1) {
      throw std::runtime_error("Cannot listen on socket");
    }  //if

    std::cout << "Waiting for connection on port " << port << std::endl;
    return 0;
  }

  int getStatus() { return status; }

  std::string getIP(struct sockaddr * sa) {
    char ipChar[INET_ADDRSTRLEN];
    struct sockaddr_in * ipAddressStruct = (struct sockaddr_in *)sa;
    inet_ntop(AF_INET, &(ipAddressStruct->sin_addr), ipChar, INET_ADDRSTRLEN);
    std::string result = std::string(ipChar);
    return result;
  }

  std::pair<int, std::string> connect2Client() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      throw std::runtime_error("Cannot accept client connection");
      // return -1;
    }
    std::pair<int, std::string> result =
        std::make_pair(client_connection_fd, getIP((struct sockaddr *)&socket_addr));
    return result;
  }

  size_t read2Buffer(int client_fd, std::vector<char> & buffer) {
    size_t len = recv(client_fd, buffer.data(), buffer.size(), 0);
    for (size_t i = 0; i < buffer.size(); i++) {
      std::cout << buffer[i];
    }
    std::cout << std::endl;
    return len;
  }

  void connect2Server() {
    std::cout << "Connecting to " << hostname << " on port " << port << "..."
              << std::endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      throw std::runtime_error("Cannot connect to server");
    }
  }

  int getSocketFd() { return socket_fd; }

  struct addrinfo getHost_info() {
    return host_info;
  };

  ~Socket() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  }
};
