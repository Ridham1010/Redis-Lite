#ifndef MY_REDIS_CLIENT_H
#define MY_REDIS_CLIENT_H

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>          // handling streams and conversions
#include <cstdlib>
#include <cstring>
#include <unistd.h>         // close(), read(), write()
#include <sys/types.h>      // socket programming
#include <sys/socket.h>     // socket programming
#include <arpa/inet.h>      // network communication (inet_pton(), htonl())
#include <netdb.h>          // hostname resolution (getaddrinfo())

class Client {
private:
    const std::string host;
    int port;

public:
    // Constructor
    Client(const std::string &host, int port);

    // Getter and Setter
    std::string getHost() const;
    int getPort() const;

    // Functions
    int connectToServer();
    std::vector<std::string> splitArgs(const std::string &line);
    std::string buildRESPCommand(const std::vector<std::string> &args);
    bool parseAndPrintRedisReply(int sockfd);
};

// Util functions
namespace Utils {
    void printHelp();
    std::string trim(const std::string &s);
}

#endif // MY_REDIS_CLIENT_H