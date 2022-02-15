#include "Parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::ostringstream;

string readFileIntoString(const string& path) {
    ifstream input_file(path.c_str());
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int main()
{
    string filename("req.txt");
    string file_contents;

    file_contents = readFileIntoString(filename);
    // cout << file_contents << endl;
    Parser newparser(file_contents);
    newparser.parse_method();
    newparser.parse_hostname();
    newparser.parse_pathname();

}