#include "Socket.hpp"

int main() {
  Socket server(NULL, "4444");
  server.serverSocket();
  std::vector<char> Vector = server.receiveFromClient();
  Socket client("localhost", "4444");
  std::string newString = "edit this"; 
  std::vector<char> newVector(newString.begin(), newString.end()); 
  client.sendtoServer(newVector);
  return 0;
}