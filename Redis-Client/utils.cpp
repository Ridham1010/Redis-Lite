#include "my_redis_cli.h"

namespace Utils {
    // Print help
    void printHelp() {
        std::cout << "my_redis_cli 1.0.0\n"
                << "Usage: \n"
                << "      With arguments:            ./client -h <host> -p <port>\n"
                << "      Default Host (127.0.0.1):  ./client -p <port>\n"
                << "      Default Port (6379):       ./client -h <host>\n"
                << "To get help about Redis commands type:\n"
                << "      \"help @<group>\" to get a list of commands in <group>\n"
                << "      \"help <command>\" for help on <command>\n"
                << "      \"help <tab>\" to get a list of possible help topics\n"
                << "      \"quit\" to exit\n\n"
                << "To set my_redis_cli preferences:\n"
                << "      \":set hints\" enable online hints\n"
                << "      \":set nohints\" disable online hints\n"
                << "Set your preferences in ~/.myredisclirc\n"
                << std::endl;
    }

    // Trim leading/trailing whitespace
    std::string trim(const std::string &s) {
        auto start = s.find_first_not_of(" \t\n\r\f\v");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\n\r\f\v");
        return s.substr(start, end - start + 1);
    }
}