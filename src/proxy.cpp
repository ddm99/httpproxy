#include <sys/select.h>
#include <sys/types.h>

#include <cstdio>
#include <limits>
#include <thread>

#include "Cache.hpp"
#include "Log.hpp"
#include "Parser.hpp"
#include "Socket.hpp"
#define BUFSIZE 65536
Log newLog;
void use_connect(Parser & newparser, int client_fd, size_t id) {
  try {
    Socket s1(newparser.getHostName().c_str(), "443");
    s1.connect2Server();
    int website_fd = s1.getSocketFd();
    std::string success = "HTTP/1.1 200 OK\r\n\r\n";
    send(client_fd, success.c_str(), success.size(), 0);
    int fdmax = (client_fd > website_fd) ? client_fd : website_fd;
    fd_set fdset;
    while (true) {
      FD_ZERO(&fdset);
      FD_SET(client_fd, &fdset);
      FD_SET(website_fd, &fdset);
      select(fdmax + 1, &fdset, NULL, NULL, NULL);
      std::vector<char> buffer(BUFSIZE);
      if (FD_ISSET(client_fd, &fdset)) {
        int len = recv(client_fd, buffer.data(), BUFSIZE, 0);
        if (len <= 0) {
          try {
            newLog.writeTunnelClose(id);
          }
          catch (std::iostream::failure & e) {
            std::cerr << e.what();
            return;
          }
          return;
        }
        while (len > 0) {
          int sendLength = send(website_fd, buffer.data(), len, 0);
          len = len - sendLength;
        }
      }
      else if (FD_ISSET(website_fd, &fdset)) {
        int len = recv(website_fd, buffer.data(), BUFSIZE, 0);
        if (len <= 0) {
          try {
            newLog.writeTunnelClose(id);
          }
          catch (std::iostream::failure & e) {
            std::cerr << e.what();
            return;
          }

          return;
        }
        while (len > 0) {
          int sendLength = send(client_fd, buffer.data(), len, 0);
          len = len - sendLength;
        }
      }
    }
  }
  catch (std::runtime_error & e) {
    std::cerr << e.what();
    return;
  }
}

void use_get(Parser & newparser, int client_fd, Cache & cacheStorage, size_t id) {
  Response * cachedResponse = cacheStorage.lookupElement(newparser.getUrl());
  Socket s1(newparser.getHostName().c_str(), "80");
  s1.connect2Server();
  std::vector<char> buffer(BUFSIZE);
  buffer = newparser.buildRequest();
  int website_fd = s1.getSocketFd();
  if ((cachedResponse != NULL) && (!cachedResponse->needRevalidation)) {
    try {
      newLog.writeinCacheValid(id);
    }
    catch (std::iostream::failure & e) {
      std::cerr << e.what();
      return;
    }
    send(client_fd, cachedResponse->content.data(), cachedResponse->content.size(), 0);
    std::cout << "returning a cached response" << std::endl;
    std::string returnedResponse(cachedResponse->content.begin(),
                                 cachedResponse->content.end());
    std::cout << returnedResponse << "\n"
              << "cached response end" << std::endl;
  }
  else {
    try {
      newLog.writeBeforeSend(id, newparser.getFirstline(), newparser.getHostName());
    }
    catch (std::iostream::failure & e) {
      std::cerr << e.what();
      return;
    }

    send(website_fd, buffer.data(), BUFSIZE, 0);
    std::vector<char> newbuffer;
    int len = 1;
    while (len > 0) {
      len = s1.read2Buffer(website_fd, buffer);
      if (len > 0) {
        newbuffer.insert(newbuffer.end(), buffer.begin(), buffer.begin() + len);
      }
    }
    ResponseParser rParser(newbuffer);
    rParser.parseResponseWrapper();
    try {
      newLog.writeAfterReceive(id, rParser.getFirstline(), newparser.getHostName());
    }
    catch (std::iostream::failure & e) {
      std::cerr << e.what();
      return;
    }

    if (rParser.isCachable()) {
      Response newResponse = rParser.getResponse();
      if (cachedResponse != NULL) {
        try {
          newLog.writeinCacheExpired(
              id, (cachedResponse->responseReceivedTime + (int)cachedResponse->maxAge));
        }
        catch (std::iostream::failure & e) {
          std::cerr << e.what();
          return;
        }
        cacheStorage.updateValue(newparser.getUrl(), newResponse);
        std::cout << "Updated the response value for url: " << newparser.getUrl()
                  << std::endl;
      }
      else {
        try {
          newLog.writeNotinCache(id);
        }
        catch (std::iostream::failure & e) {
          std::cerr << e.what();
          return;
        }

        cacheStorage.insertElement(newparser.getUrl(), newResponse);
        std::cout << "Inserting new response for url: " << newparser.getUrl()
                  << std::endl;
      }
      try {
        newLog.writeCached(id, newResponse.responseReceivedTime + newResponse.maxAge);
      }
      catch (std::iostream::failure & e) {
        std::cerr << e.what();
        return;
      }
    }
    else {
      try {
        newLog.writeNotCacheable(id);
      }
      catch (std::iostream::failure & e) {
        std::cerr << e.what();
        return;
      }
    }
    send(client_fd, newbuffer.data(), newbuffer.size(), 0);
  }
}

void use_post(Parser & newparser, int client_fd, size_t id) {
  Socket s1(newparser.getHostName().c_str(), "80");
  s1.connect2Server();
  std::vector<char> buffer(BUFSIZE);
  buffer = newparser.buildRequest();
  int website_fd = s1.getSocketFd();
  try {
    newLog.writeBeforeSend(id, newparser.getFirstline(), newparser.getHostName());
  }
  catch (std::iostream::failure & e) {
    std::cerr << e.what();
    return;
  }

  send(website_fd, buffer.data(), BUFSIZE, 0);
  std::vector<char> newbuffer;
  int len = 1;
  while (len > 0) {
    len = s1.read2Buffer(website_fd, buffer);
    if (len > 0) {
      newbuffer.insert(newbuffer.end(), buffer.begin(), buffer.begin() + len);
    }
  }
  ResponseParser rParser(newbuffer);
  rParser.parseResponseWrapper();
  try {
    newLog.writeAfterReceive(id, rParser.getFirstline(), newparser.getHostName());
  }
  catch (std::iostream::failure & e) {
    std::cerr << e.what();
    return;
  }
  send(client_fd, newbuffer.data(), newbuffer.size(), 0);
}

void threadConnections(int client_fd,
                       Cache * cacheStorage,
                       size_t id,
                       std::string ipAddress) {
  std::vector<char> buffer(BUFSIZE);
  recv(client_fd, buffer.data(), BUFSIZE, 0);
  Parser newparser(buffer);
  try {
    newLog.writeRequest(id, newparser.getFirstline(), ipAddress);
  }
  catch (std::iostream::failure & e) {
    std::cout << e.what();
    return;
  }
  if (newparser.parseGetnPost()) {
    if (newparser.getMethod() == "CONNECT") {
      use_connect(newparser, client_fd, id);
    }
    else if (newparser.getMethod() == "GET") {
      use_get(newparser, client_fd, *cacheStorage, id);
    }
    else if (newparser.getMethod() == "POST") {
      use_post(newparser, client_fd, id);
    }
  }
}

/*
Socket.h return IP addres
we have id
first log: received REQUEST from USER IP @ TIME
second log: 
*/

int main() {
  Socket s(NULL, "4444");
  s.serverSocket();
  Cache cacheStorage;
  size_t ids = 0;
  std::pair<int, std::string> clientInfo;
  while (true) {
    clientInfo = s.connect2Client();
    //  threadConnections(client_fd,&cacheStorage);
    std::thread connection(
        &threadConnections, clientInfo.first, &cacheStorage, ids, clientInfo.second);
    connection.detach();
    ids++;
  }
  return 0;
}