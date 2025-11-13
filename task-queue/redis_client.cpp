#include "redis_client.h"

RedisClient::RedisClient(const std::string& host, int port) 
    : host(host), port(port), sockfd(-1) {}

RedisClient::~RedisClient() {
    disconnect();
}

std::string RedisClient::buildRESPCommand(const std::vector<std::string>& args) {
    std::ostringstream oss;
    oss << "*" << args.size() << "\r\n";
    for (const auto& arg : args) {
        oss << "$" << arg.size() << "\r\n" << arg << "\r\n";
    }
    return oss.str();
}

std::string RedisClient::readResponse() {
    char buffer[4096];
    ssize_t bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        return "";
    }
    buffer[bytes] = '\0';
    return std::string(buffer);
}

bool RedisClient::connect() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return false;
    }

    if (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed to " << host << ":" << port << "\n";
        return false;
    }

    return true;
}

void RedisClient::disconnect() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

std::string RedisClient::command(const std::vector<std::string>& args) {
    std::string cmd = buildRESPCommand(args);
    if (send(sockfd, cmd.c_str(), cmd.size(), 0) < 0) {
        return "";
    }
    return readResponse();
}

std::string RedisClient::hset(const std::string& key, const std::string& field, const std::string& value) {
    return command({"HSET", key, field, value});
}

std::string RedisClient::hmset(const std::string& key, const std::vector<std::pair<std::string, std::string>>& fields) {
    std::vector<std::string> args = {"HMSET", key};
    for (const auto& field : fields) {
        args.push_back(field.first);
        args.push_back(field.second);
    }
    return command(args);
}

std::string RedisClient::hget(const std::string& key, const std::string& field) {
    return command({"HGET", key, field});
}

std::string RedisClient::hgetall(const std::string& key) {
    return command({"HGETALL", key});
}

std::string RedisClient::lpush(const std::string& key, const std::string& value) {
    return command({"LPUSH", key, value});
}

std::string RedisClient::rpush(const std::string& key, const std::string& value) {
    return command({"RPUSH", key, value});
}

std::string RedisClient::lpop(const std::string& key) {
    return command({"LPOP", key});
}

std::string RedisClient::rpop(const std::string& key) {
    return command({"RPOP", key});
}

std::string RedisClient::llen(const std::string& key) {
    return command({"LLEN", key});
}

std::string RedisClient::get(const std::string& key) {
    return command({"GET", key});
}

std::string RedisClient::set(const std::string& key, const std::string& value) {
    return command({"SET", key, value});
}
