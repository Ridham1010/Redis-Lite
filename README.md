# Redis-Lite

A lightweight implementation of a Redis-like in-memory data store written in C++. This project demonstrates core networking concepts, data structures, multi-threading, and RESP (REdis Serialization Protocol) parsing.

## Authors

Ridham1010 (Ridham Shah - B24CS1064)
centauri1219 (Atharv Dhuvad - B24CS1085)
shreyshah (Shrey Shah - B24CS1111)

## Features

- **TCP Server**: Multi-threaded TCP server supporting concurrent client connections
- **RESP Protocol**: Parses Redis RESP protocol for command processing
- **Multi-threading**: Handles multiple clients simultaneously using C++ threads
- **Socket Programming**: Implements low-level socket operations (bind, listen, accept)
- **Graceful Shutdown**: Signal handling for clean server termination
- **Background Persistence**: Thread for periodic database dumps (framework in place)

## Architecture

The project is organized into the following components:

- **RedisServer**: Core server handling socket operations and client connections
- **RedisCommandHandler**: Processes and executes Redis-like commands
- **RESP Parser**: Parses Redis protocol formatted commands

## Prerequisites

- C++17 or higher
- g++ compiler
- pthread library
- Linux/Unix environment (for socket programming)

## Building the Project

The project uses a Makefile for building:

```bash
# Build the project
make

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Build and run
make run
```

## Running the Server

By default, the server runs on port 6379 (standard Redis port):

```bash
./redis-lite
```

To run on a custom port:

```bash
./redis-lite 8080
```

## Testing with Redis CLI

You can test the server using the official Redis CLI:

```bash
redis-cli -p 6379
```

Or use telnet/netcat:

```bash
telnet localhost 6379
nc localhost 6379
```

## Project Structure

```
Redis-Lite/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── Redisserver.cpp          # Server implementation
│   └── RedisCommandHandler.cpp  # Command processing
├── include/
│   ├── RedisServer.h            # Server header
│   └── RedisCommandHandler.h    # Command handler header
├── build/                       # Build artifacts (generated)
├── Makefile                     # Build configuration
└── README.md
```

## How It Works

1. **Server Initialization**: Creates a TCP socket and binds to specified port
2. **Listening**: Waits for incoming client connections
3. **Client Handling**: Spawns a new thread for each connected client
4. **Command Processing**: 
   - Receives RESP-formatted commands
   - Parses command tokens
   - Processes commands (framework in place)
   - Sends RESP-formatted responses
5. **Graceful Shutdown**: Handles termination signals and closes connections

## RESP Protocol Support

The server supports the Redis RESP (REdis Serialization Protocol) format:

```
*2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n
```

- `*2` - Array with 2 elements
- `$4` - Bulk string of length 4
- `PING` - Command
- `TEST` - Argument

## Current Status

This is a work-in-progress implementation. The core server infrastructure and RESP parsing are complete. Command implementation (SET, GET, PING, etc.) is currently in development.

## Future Enhancements

- [ ] Implement core Redis commands (SET, GET, DEL, etc.)
- [ ] Add data persistence to disk
- [ ] Implement expiration/TTL support
- [ ] Add support for data structures (Lists, Sets, Hashes)
- [ ] Connection pooling and optimization
- [ ] Configuration file support
- [ ] Monitoring and statistics

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.



---

**Note**: This is an educational project to understand Redis internals and network programming concepts. For production use, please use the official Redis implementation.



