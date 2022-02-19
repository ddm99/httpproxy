all:test proxy
test: Parser.cpp
	g++ -o test -pthread -ggdb3 -Wall -Werror -pedantic -std=gnu++11 Parser.cpp

proxy: proxy.cpp
	g++ -o proxy -pthread -ggdb3 -Wall -Werror -pedantic -std=gnu++11 proxy.cpp