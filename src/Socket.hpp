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

  //Establishes a socket for the server side
  int serverSocket() {
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot bind socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return -1;
    }  //if

    status = listen(socket_fd, 100);
    if (status == -1) {
      std::cerr << "Error: cannot listen on socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return -1;
    }  //if

    std::cout << "Waiting for connection on port " << port << std::endl;
    return 0;
  }

  int getStatus() { return status; }

  //Method to receive data from the server
  std::vector<char> receiveFromServer() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
      // return -1;
    }  //if

    std::vector<char> buffer(5000);
    int result = recv(client_connection_fd, buffer.data(), buffer.size(), 0);
    buffer[9] = 0;
    std::cout << result << "is the result\n";
    for (size_t i = 0; i < buffer.size(); i++) {
      std::cout << buffer[i];
    }
    std::cout << std::endl;
    // std::cout << buffer;
    // std::cout << "Server received: " << buffer << std::endl;

    return buffer;
  }

  ~Socket() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  }
};
