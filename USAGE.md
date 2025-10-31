# Redis-Lite Usage Guide

## 📦 Building the Project

### Build Everything (Server + Client)
```bash
make
```
This creates:
- `redis-lite` (server executable)
- `redis-cli` (client executable)

### Build Only Server
```bash
make server
```

### Build Only Client
```bash
make client
```

### Clean and Rebuild
```bash
make rebuild
```

### Clean Build Files
```bash
make clean
```

---

## 🚀 Running the Server

### Start Server on Default Port (6379)
```bash
./redis-lite
```
Or:
```bash
make run-server
```

### Start Server on Custom Port
```bash
./redis-lite 8080
```

**What you'll see:**
```
No dump found or load failed ..starting with empty database
Server is listening on port 6379...
```

**The server will:**
- ✅ Load data from `dump.my_rdb` if it exists
- ✅ Listen for client connections on port 6379 (or specified port)
- ✅ Handle multiple clients concurrently (multi-threaded)
- ✅ Auto-save database every 5 minutes to `dump.my_rdb`

---

## 💻 Running the Client

### Interactive Mode (REPL)
```bash
./redis-cli
```
Or:
```bash
make run-client
```

**What you'll see:**
```
127.0.0.1:6379> 
```

Now you can type Redis commands:
```bash
127.0.0.1:6379> PING
PONG
127.0.0.1:6379> SET name Alice
OK
127.0.0.1:6379> GET name
Alice
127.0.0.1:6379> quit
Goodbye.
```

### Connect to Custom Host/Port
```bash
# Connect to localhost on port 8080
./redis-cli -h 127.0.0.1 -p 8080

# Connect to remote server
./redis-cli -h 192.168.1.100 -p 6379
```

### One-Shot Command Mode
Execute a single command and exit:
```bash
./redis-cli -h 127.0.0.1 -p 6379 GET mykey
```

---

## 🎮 Complete Workflow Example

### Terminal 1: Start Server
```bash
cd /home/atharv/Documents/Redis-Lite
make                    # Build everything
./redis-lite            # Start server
```

### Terminal 2: Use Client
```bash
cd /home/atharv/Documents/Redis-Lite
./redis-cli             # Start interactive client

# Now interact:
127.0.0.1:6379> SET user:1 "Alice"
OK
127.0.0.1:6379> GET user:1
Alice
127.0.0.1:6379> LPUSH tasks "task1"
:1
127.0.0.1:6379> LPUSH tasks "task2"
:2
127.0.0.1:6379> LGET tasks
task2
task1
```

---

## 📝 Available Commands

### Basic Commands
- `PING` - Test connection
- `ECHO <message>` - Echo back message
- `FLUSHALL` - Clear all data

### Key-Value Operations
- `SET key value` - Set a key
- `GET key` - Get a key's value
- `DEL key` - Delete a key
- `KEYS` - List all keys
- `TYPE key` - Get key type
- `EXPIRE key seconds` - Set TTL
- `RENAME oldkey newkey` - Rename a key

### List Operations
- `LPUSH key value` - Add to list front
- `RPUSH key value` - Add to list back
- `LPOP key` - Remove from front
- `RPOP key` - Remove from back
- `LGET key` - Get all list elements
- `LLEN key` - Get list length
- `LINDEX key index` - Get element at index
- `LSET key index value` - Set element at index
- `LREM key count value` - Remove elements

### Hash Operations
- `HSET key field value` - Set hash field
- `HGET key field` - Get hash field
- `HGETALL key` - Get all hash fields
- `HDEL key field` - Delete hash field
- `HEXISTS key field` - Check if field exists
- `HKEYS key` - Get all field names
- `HVALS key` - Get all values
- `HLEN key` - Get number of fields
- `HMSET key f1 v1 f2 v2...` - Set multiple fields

---

## 🔧 Task Queue System Usage

### Setup Task Queues

**Terminal 1 - Server:**
```bash
./redis-lite
```

**Terminal 2 - Producer (Create Tasks):**
```bash
./redis-cli

# Create a task with metadata
127.0.0.1:6379> HMSET task:1001 type "send_email" to "user@example.com" status "pending"
OK

# Add to high-priority queue
127.0.0.1:6379> LPUSH queue:high "task:1001"
:1

# Create more tasks
127.0.0.1:6379> HMSET task:1002 type "process_video" file "video.mp4" status "pending"
OK
127.0.0.1:6379> RPUSH queue:normal "task:1002"
:1
```

**Terminal 3 - Worker (Process Tasks):**
```bash
./redis-cli

# Worker polls for tasks (check high priority first)
127.0.0.1:6379> LPOP queue:high
task:1001

# Get task details
127.0.0.1:6379> HGETALL task:1001
type
send_email
to
user@example.com
status
pending

# Update status
127.0.0.1:6379> HSET task:1001 status "processing"
:1

# ... do the work (send email) ...

# Mark complete
127.0.0.1:6379> HSET task:1001 status "completed"
:1
127.0.0.1:6379> LPUSH tasks:completed "task:1001"
:1
```

**Terminal 4 - Monitor:**
```bash
./redis-cli

# Check queue sizes
127.0.0.1:6379> LLEN queue:high
:0
127.0.0.1:6379> LLEN queue:normal
:1
127.0.0.1:6379> LLEN tasks:completed
:1
```

---

## 🐛 Troubleshooting

### Server won't start
```bash
# Check if port is already in use
lsof -i :6379

# Kill existing process
kill -9 <PID>

# Or use a different port
./redis-lite 8080
```

### Client can't connect
```bash
# Verify server is running
ps aux | grep redis-lite

# Check if listening on port
netstat -tuln | grep 6379

# Try connecting with verbose output
./redis-cli -h 127.0.0.1 -p 6379
```

### Build errors
```bash
# Clean and rebuild
make clean
make

# Check for missing dependencies
g++ --version  # Should be C++17 compatible
```

---

## 📊 File Structure After Build

```
Redis-Lite/
├── redis-lite          ← Server executable
├── redis-cli           ← Client executable
├── dump.my_rdb         ← Database persistence file (auto-created)
├── build/              ← Server object files
│   ├── main.o
│   ├── RedisServer.o
│   ├── RedisDatabase.o
│   └── RedisCommandHandler.o
├── build-client/       ← Client object files
│   ├── main.o
│   ├── my_redis_cli.o
│   └── utils.o
├── src/                ← Server source code
├── Redis-Client/       ← Client source code
└── include/            ← Header files
```

---

## 🎯 Quick Reference

| Action | Command |
|--------|---------|
| **Build everything** | `make` |
| **Start server** | `./redis-lite` |
| **Start client** | `./redis-cli` |
| **Custom port** | `./redis-lite 8080` |
| **Connect to custom port** | `./redis-cli -p 8080` |
| **Clean build** | `make clean` |
| **Rebuild all** | `make rebuild` |
| **View help in client** | Type `help` in REPL |
| **Exit client** | Type `quit` or Ctrl+D |
| **Stop server** | Ctrl+C |

---

## 💡 Tips

1. **Always start the server first** before connecting clients
2. **Use multiple terminals** to simulate producer/worker pattern
3. **Data persists** across server restarts (saved in `dump.my_rdb`)
4. **Auto-save** happens every 5 minutes
5. **Multi-client support** - open as many clients as you need
6. **Task queues** work best with separate priority levels (critical, high, normal, low)

---

Happy Redis-Lite-ing! 🚀
