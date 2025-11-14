# Redis-Lite

A lightweight implementation of a Redis-like in-memory data store written in C++. This project demonstrates core networking concepts, data structures, multi-threading, and RESP (REdis Serialization Protocol) parsing.

## Authors

- Ridham1010 (Ridham Shah - B24CS1064)
- centauri1219 (Atharv Dhuvad - B24CS1085)
- shrey-shah299 (Shrey Shah - B24CS1111)

## Features

- **In-Memory Data Structures**: Hash tables, lists, and nested hash maps for efficient storage
- **TCP Server**: Multi-threaded TCP server supporting concurrent client connections
- **RESP Protocol**: Full support for Redis RESP protocol parsing and encoding
- **Multi-threading**: Handles multiple clients simultaneously using C++ threads
- **Socket Programming**: Implements low-level socket operations (bind, listen, accept)
- **Data Persistence**: Automatic background persistence with dump/load functionality
- **Key Expiration**: TTL support with automatic cleanup
- **Task Queue System**: Distributed task queue with priority-based processing
- **Web Dashboard**: Real-time monitoring and task management interface

## Architecture

The project consists of three main components:

### 1. Redis-Lite Server
Core server handling socket operations, client connections, and command processing using three primary data structures:
- **Key-Value Store**: `unordered_map<string, string>` for simple key-value pairs
- **List Store**: `unordered_map<string, vector<string>>` for list operations
- **Hash Store**: `unordered_map<string, unordered_map<string, string>>` for structured data

### 2. Redis Client (CLI)
Interactive command-line interface for sending commands to the server via RESP protocol.

### 3. Task Queue System
Distributed task queue with:
- **Producer**: Creates and queues tasks with priority levels
- **Workers**: Process tasks from priority queues
- **Web Dashboard**: Real-time visualization of task processing

## Prerequisites

- C++17 or higher
- g++ compiler
- pthread library
- Linux/Unix environment (for socket programming)
- Node.js and npm (for web dashboard only)

## Building the Project

### Build Server and Client

```bash
# Build everything (server + client)
make

# Build only server
make server

# Build only client
make client

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild
```

### Build Task Queue Components

```bash
cd task-queue
make

# This builds:
# - producer (task creator)
# - worker (task processor)
```

## Running Instructions

### Option 1: Basic Command Testing (Server + CLI)

Use this option if you want to test Redis commands interactively.

**Terminal 1 - Start Redis Server:**
```bash
./redis-lite

# Or with custom port:
./redis-lite 8080
```

You should see:
```
Database loaded dump.my_rdb
Server is listening on port 6379...
```

**Terminal 2 - Start Redis Client:**
```bash
./redis-cli

# Or connect to custom port:
./redis-cli -p 8080
```

**Test Commands:**
```
127.0.0.1:6379> SET username alice
OK
127.0.0.1:6379> GET username
alice
127.0.0.1:6379> LPUSH tasks task1
1
127.0.0.1:6379> HSET user:1 name John
1
127.0.0.1:6379> HGETALL user:1
name
John
127.0.0.1:6379> quit
```

### Option 2: Full System with Web Interface

Use this option to see the complete task queue system with real-time web dashboard.

**Terminal 1 - Start Redis Server:**
```bash
./redis-lite
```

**Terminal 2 - Start Workers:**
```bash
cd task-queue
./worker 3
```

This starts 3 worker threads that will process tasks from the queue.

**Terminal 3 - Start Backend Server:**
```bash
cd web-dashboard/backend
npm install     # First time only
node server.js
```

You should see:
```
Backend server running on http://localhost:3001
Connected to Redis-Lite at 127.0.0.1:6379
```

**Terminal 4 - Start Frontend:**
```bash
cd web-dashboard/frontend
npm install     # First time only
npm start
```

The React development server will start and automatically open your browser to:
```
http://localhost:3000
```

**Terminal 5 - Start Producer (Optional):**
```bash
cd task-queue
./producer
```

Use the interactive menu to create tasks. You can also create tasks directly from the web interface.

**What you'll see in the Web Dashboard:**
- Real-time Redis server status
- Task creation interface
- Worker status (idle/processing)
- Queue statistics by priority
- Live task processing updates

## Supported Commands

### Key-Value Operations
- `SET key value` - Store a key-value pair
- `GET key` - Retrieve value by key
- `DEL key` - Delete a key
- `KEYS` - List all keys
- `TYPE key` - Get the type of value stored at key
- `EXPIRE key seconds` - Set key expiration
- `RENAME oldkey newkey` - Rename a key

### List Operations
- `LPUSH key value` - Insert at the front of list
- `RPUSH key value` - Insert at the back of list
- `LPOP key` - Remove and return first element
- `RPOP key` - Remove and return last element
- `LLEN key` - Get list length
- `LINDEX key index` - Get element at index
- `LSET key index value` - Set element at index
- `LREM key count value` - Remove elements

### Hash Operations
- `HSET key field value` - Set hash field
- `HGET key field` - Get hash field value
- `HGETALL key` - Get all fields and values
- `HDEL key field` - Delete hash field
- `HEXISTS key field` - Check if field exists
- `HKEYS key` - Get all field names
- `HVALS key` - Get all values
- `HLEN key` - Get number of fields
- `HMSET key field1 value1 field2 value2` - Set multiple fields

### Utility Commands
- `PING` - Test server connection
- `ECHO message` - Echo the message back
- `FLUSHALL` - Clear all data

## Project Structure

```
Redis-Lite/
├── src/
│   ├── main.cpp                    # Server entry point
│   ├── Redisserver.cpp             # TCP server implementation
│   ├── RedisCommandHandler.cpp     # Command processing and RESP parsing
│   └── RedisDatabase.cpp           # Data structure implementations
├── include/
│   ├── RedisServer.h               # Server header
│   ├── RedisCommandHandler.h       # Command handler header
│   └── RedisDatabase.h             # Database header
├── Redis-Client/
│   ├── main.cpp                    # Client entry point
│   ├── my_redis_cli.cpp            # Client implementation
│   └── utils.cpp                   # Utility functions
├── task-queue/
│   ├── producer.cpp                # Task creator
│   ├── worker.cpp                  # Task processor
│   └── redis_client.cpp            # Redis client for task queue
├── web-dashboard/
│   ├── backend/
│   │   └── server.js               # Express API server
│   └── frontend/
│       └── src/                    # React application
├── build/                          # Build artifacts (generated)
├── Makefile                        # Build configuration
└── README.md
```

## How It Works

### Server Architecture

1. **Server Initialization**: Creates a TCP socket and binds to specified port (default: 6379)
2. **Listening**: Server enters listening state, waiting for incoming client connections
3. **Multi-threaded Client Handling**: 
   - For each client connection, spawns a new thread
   - Each thread independently handles one client's requests
   - Allows concurrent connections from multiple clients
4. **Command Processing**: 
   - Receives RESP-formatted commands from client
   - Parses command into tokens (command name + arguments)
   - Routes to appropriate handler based on command type
   - Executes operation on in-memory data structures
   - Returns RESP-formatted response
5. **Data Persistence**:
   - Background thread dumps database to disk every 300 seconds
   - On server startup, loads data from dump file
   - On shutdown, performs final dump to preserve data
6. **Thread Safety**: All database operations protected by mutex locks

### RESP Protocol

The server implements the Redis RESP (REdis Serialization Protocol) for client-server communication:

**Request Format:**
```
*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$5\r\nalice\r\n
```
- `*3` - Array with 3 elements
- `$3` - Bulk string of length 3
- `SET` - Command
- `$4` - Bulk string of length 4
- `name` - Key
- `$5` - Bulk string of length 5
- `alice` - Value

**Response Types:**
- Simple String: `+OK\r\n`
- Error: `-Error message\r\n`
- Integer: `:42\r\n`
- Bulk String: `$5\r\nalice\r\n`
- Array: `*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n`

### Task Queue System

The distributed task queue demonstrates a real-world application built on Redis-Lite:

1. **Producer** creates tasks with metadata (type, priority, status)
2. Tasks are stored as hashes: `HMSET task:1001 type email priority high status pending`
3. Task IDs are pushed to priority-based queues: `LPUSH queue:high task:1001`
4. **Workers** continuously poll queues in priority order (critical > high > normal > low)
5. Workers retrieve tasks using `LPOP`, update status to "processing", perform work, then mark as "completed"
6. **Web Dashboard** queries Redis for real-time status updates via the backend API

## Data Structure Implementation

### Key-Value Store
- **Structure**: `unordered_map<string, string>`
- **Time Complexity**: O(1) average for SET/GET
- **Use Cases**: Simple key-value pairs, session tokens, configuration

### List Store
- **Structure**: `unordered_map<string, vector<string>>`
- **Time Complexity**: 
  - LPUSH: O(n) (must shift elements)
  - RPUSH: O(1) (append to end)
  - LPOP/RPOP: O(n)/O(1)
- **Use Cases**: Task queues, message queues, activity feeds

### Hash Store
- **Structure**: `unordered_map<string, unordered_map<string, string>>`
- **Time Complexity**: O(1) average for HSET/HGET
- **Use Cases**: User profiles, product details, structured entities

## Troubleshooting

### Server won't start
```bash
# Check if port is already in use
sudo lsof -ti:6379 | xargs kill -9

# Or use a different port
./redis-lite 8080
```

### Build errors
```bash
# Install required packages
sudo apt-get install g++ make

# Verify C++ version (requires C++17)
g++ --version
```

### Connection refused
```bash
# Ensure server is running
ps aux | grep redis-lite

# Check server is listening
netstat -tuln | grep 6379
```

### Web dashboard issues
```bash
# Install Node.js dependencies
cd web-dashboard/backend && npm install
cd web-dashboard/frontend && npm install

# Check backend is running on port 3001
curl http://localhost:3001/api/status
```

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.

## Acknowledgments

This project was created as an educational exercise to understand:
- Low-level network programming with sockets
- Multi-threaded server architecture
- Protocol design and implementation (RESP)
- In-memory data structure design
- Distributed systems concepts (task queues)

---

**Note**: This is an educational project to understand Redis internals and network programming concepts. For production use, please use the official Redis implementation.



