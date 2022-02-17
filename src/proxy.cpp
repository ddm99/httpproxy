#include <sys/select.h>
#include <sys/types.h>

#include "Parser.hpp"
#include "Socket.hpp"

int main() {
  Socket s(NULL, "4444");
  s.serverSocket();
  int client_fd = s.connect2Client();
  Parser newparser(file_contents);
  newparser.parseGetnPost();
  std::cout << (newparser.getParsed_message())->hostname << "\n";
  Socket s1((newparser.getParsed_message())->hostname.c_str(), (newparser.getParsed_message())->portnum.c_str());

}
