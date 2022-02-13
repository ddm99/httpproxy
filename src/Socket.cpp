#include "Socket.hpp"

int main() {
  Socket client("localhost", "4444");
  std::string newString = "edit this"; 
  std::vector<char> newVector(newString.begin(), newString.end()); 
  client.sendtoServer(newVector);
  //s1.serverSocket();
  //std::vector<char> newVector = s1.receiveFromClient();
  return 0;
}