#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <cstring>

using namespace std;
class Socket{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  string hostname;
  string port;


};