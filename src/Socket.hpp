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

  int connect2Client() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
      // return -1;
    }
  return client_connection_fd;
    // return readBuffer(client_connection_fd);
  }

<<<<<<< HEAD
  std::vector<char> readBuffer(int client_fd) {
    std::vector<char> buffer(1024 * 1024);
    recv(client_fd, buffer.data(), buffer.size(), 0);
    for (size_t i = 0; i < buffer.size(); i++) {
      std::cout << buffer[i];
=======
  int read2Buffer(int client_fd,std::vector<char> *buffer) {
    recv(client_fd,buffer->data(),buffer->size(),0);
    for (size_t i = 0; i < buffer->size(); i++) {
      std::cout << (**buffer);
      ++buffer;
>>>>>>> a45c5aaf5cd558fc6f05b77599303da512ec6728
    }
    std::cout << std::endl;
    return buffer;
  }

  void sendtoServer(std::vector<char> request) {
    std::cout << "Connecting to " << hostname << " on port " << port << "..."
              << std::endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot connect to socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    }  //if

    // const char * message = request.data();
    // send(socket_fd, message, strlen(message), 0);
  }

  int getSocketFd() { return socket_fd; }

  int getClient_connection_fd() { return client_connection_fd; }

  struct addrinfo getHost_info() {
    return host_info;
  };

  ~Socket() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  }
};
