#include "Parser.hpp"
#include <fstream>
#include <cassert>
#include <iterator>

void ReadFromFile(std::vector<char> &x, const std::string &file_name)
{
    std::ifstream read_file(file_name);
    assert(read_file.is_open());

    std::copy(std::istream_iterator<char>(read_file), std::istream_iterator<char>(),
        std::back_inserter(x));

    read_file.close();
}

int main()
{
    std::vector<char> req;
    ReadFromFile(req, "req.txt");
    for (size_t i = 0; i < req.size(); i++) {
      std::cout << req[i];
    }
    std::cout << std::endl;
}