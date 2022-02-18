#include "Parser.hpp"

#include <sys/select.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Socket.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostringstream;
using std::string;

string readFileIntoString(const string & path) {
  ifstream input_file(path.c_str());
  if (!input_file.is_open()) {
    cerr << "Could not open the file - '" << path << "'" << endl;
  }
  return string((std::istreambuf_iterator<char>(input_file)),
                std::istreambuf_iterator<char>());
}

int main() {
  string filename("response.txt");
  string file_contents;
  file_contents = readFileIntoString(filename);
  std::vector<char> input(file_contents.begin(), file_contents.end());
  ResponseParser newparser(input);
  newparser.parseResponseWrapper();
}