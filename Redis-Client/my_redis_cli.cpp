#include "my_redis_cli.h"

/*
Constructor
*/
Client::Client(const std::string &host, int port) : host(host), port(port) {}

/*
Getter and Setter
*/
std::string Client::getHost() const { return host; }
int Client::getPort() const { return port; }

/*
Connect to Redis at the given host:port, returning the socket FD.
Returns -1 on error.
*/
int Client::connectToServer() {
    // Resolve host
    struct addrinfo hints, *res = nullptr;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    std::string portStr = std::to_string(port);
    int err = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (err != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(err) << "\n";
        return -1;
    }

    int sockfd = -1;
    for (auto p = res; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            // Connected
            break;
        }
        close(sockfd);
        sockfd = -1;
    }
    freeaddrinfo(res);

    if (sockfd == -1) {
        std::cerr << "Could not connect to " << host << ":" << port << "\n";
    }
    return sockfd;
}

/* 
Split a string by spaces into tokens
*/
std::vector<std::string> Client::splitArgs(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

/*
Convert a vector of strings into a single RESP command.

For example, if args = ["SET", "hello", "world"]:
    *3\r\n
    $3\r\n
    SET\r\n
    $5\r\n
    hello\r\n
    $5\r\n
    world\r\n
*/
std::string Client::buildRESPCommand(const std::vector<std::string> &args) {
    // Example of array length: *3
    std::ostringstream oss;
    oss << "*" << args.size() << "\r\n";
    for (const auto &arg : args) {
        oss << "$" << arg.size() << "\r\n" << arg << "\r\n";
    }
    return oss.str();
}

/*
Parse and print a Redis response from the server (RESP protocol).
This function reads from the socket until it has parsed a complete reply.

Returns true if the response was parsed successfully, false otherwise.
*/
bool Client::parseAndPrintRedisReply(int sockfd) {
    // We read from the socket line by line (and more if needed for bulk/arrays).
    // A robust approach might store partial data in a buffer; this is simplified.

    // Read the first byte to determine the type of Redis reply.
    char firstByte;
    ssize_t bytesRead = recv(sockfd, &firstByte, 1, 0);
    if (bytesRead <= 0) {
        std::cerr << "(Error) No response or connection closed.\n";
        return false;
    }

    switch (firstByte) {
    case '+': {
        // Simple string; read up to CRLF
        std::string line;
        // Read until we hit \r\n
        char c;
        while (true) {
            ssize_t r = recv(sockfd, &c, 1, 0);
            if (r <= 0) {
                std::cerr << "(Error) Incomplete simple string.\n";
                return false;
            }
            if (c == '\r') {
                // Next should be '\n'
                recv(sockfd, &c, 1, 0);
                break;
            }
            line.push_back(c);
        }
        std::cout << line << "\n";
        return true;
    }
    case '-': {
        // Error message
        std::string line;
        char c;
        while (true) {
            ssize_t r = recv(sockfd, &c, 1, 0);
            if (r <= 0) {
                std::cerr << "(Error) Incomplete error string.\n";
                return false;
            }
            if (c == '\r') {
                recv(sockfd, &c, 1, 0);
                break;
            }
            line.push_back(c);
        }
        std::cerr << "(Error) " << line << "\n";
        return true;
    }
    case ':': {
        // Integer
        std::string line;
        char c;
        while (true) {
            ssize_t r = recv(sockfd, &c, 1, 0);
            if (r <= 0) {
                std::cerr << "(Error) Incomplete integer.\n";
                return false;
            }
            if (c == '\r') {
                recv(sockfd, &c, 1, 0);
                break;
            }
            line.push_back(c);
        }
        std::cout << line << "\n";
        return true;
    }
    case '$': {
        // Bulk string
        // First read the length (until CRLF)
        std::string lengthStr;
        char c;
        while (true) {
            ssize_t r = recv(sockfd, &c, 1, 0);
            if (r <= 0) {
                std::cerr << "(Error) Incomplete bulk length.\n";
                return false;
            }
            if (c == '\r') {
                // Next should be '\n'
                recv(sockfd, &c, 1, 0);
                break;
            }
            lengthStr.push_back(c);
        }
        int length = std::stoi(lengthStr);

        if (length == -1) {
            // Null bulk string
            std::cout << "(nil)\n";
            return true;
        }

        // Read 'length' bytes + CRLF
        std::string bulk;
        bulk.resize(length);
        ssize_t totalRead = 0;
        while (totalRead < length) {
            bytesRead = recv(sockfd, &bulk[totalRead], length - totalRead, 0);
            if (bytesRead <= 0) {
                std::cerr << "(Error) Incomplete bulk data.\n";
                return false;
            }
            totalRead += bytesRead;
        }
        // consume the trailing \r\n
        recv(sockfd, &c, 1, 0);
        recv(sockfd, &c, 1, 0);

        std::cout << bulk << "\n";
        return true;
    }
    case '*': {
        // Array
        std::string arrayCountStr;
        char c;
        while (true) {
            ssize_t r = recv(sockfd, &c, 1, 0);
            if (r <= 0) {
                std::cerr << "(Error) Incomplete array length.\n";
                return false;
            }
            if (c == '\r') {
                recv(sockfd, &c, 1, 0);
                break;
            }
            arrayCountStr.push_back(c);
        }
        int arrayCount = std::stoi(arrayCountStr);
        if (arrayCount == -1) {
            std::cout << "(nil)\n";
            return true;
        }
        for (int i = 0; i < arrayCount; ++i) {
            if (!parseAndPrintRedisReply(sockfd)) {
                return false;
            }
        }
        return true;
    }
    default: {
        std::cerr << "(Error) Unknown reply type: " << firstByte << "\n";
        return false;
    }
    }
}