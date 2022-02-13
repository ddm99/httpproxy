#include "Socket.hpp"

int main() {
  Socket s1(NULL, "4444");
  s1.serverSocket();
  std::vector<char> newVector = s1.receiveFromServer();
  //   std::cout << newVector;
  return 0;
}