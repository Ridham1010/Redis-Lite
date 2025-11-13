#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class RedisClient {
private:
    std::string host;
    int port;
    int sockfd;

    std::string buildRESPCommand(const std::vector<std::string>& args);
    std::string readResponse();
    
public:
    RedisClient(const std::string& host = "127.0.0.1", int port = 6379);
    ~RedisClient();
    
    bool connect();
    void disconnect();
    
    // Command methods
    std::string command(const std::vector<std::string>& args);
    std::string hset(const std::string& key, const std::string& field, const std::string& value);
    std::string hmset(const std::string& key, const std::vector<std::pair<std::string, std::string>>& fields);
    std::string hget(const std::string& key, const std::string& field);
    std::string hgetall(const std::string& key);
    std::string lpush(const std::string& key, const std::string& value);
    std::string rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::string llen(const std::string& key);
    std::string get(const std::string& key);
    std::string set(const std::string& key, const std::string& value);
};

#endif
