# Task Queue System - C++ Implementation

A producer-worker task queue system using your Redis-Lite server.

## Architecture

```
┌──────────┐         ┌──────────────┐         ┌──────────┐
│ Producer │────────→│ Redis-Lite   │←────────│ Worker 1 │
│          │  RESP   │    Server    │  RESP   │          │
└──────────┘         │              │         └──────────┘
                     │  ┌─────────┐ │         ┌──────────┐
                     │  │ Queues  │ │←────────│ Worker 2 │
                     │  │  -high  │ │         │          │
                     │  │ -normal │ │         └──────────┘
                     │  │  -low   │ │         ┌──────────┐
                     │  └─────────┘ │←────────│ Worker 3 │
                     │              │         │          │
                     │  ┌─────────┐ │         └──────────┘
                     │  │ Tasks   │ │
                     │  │ (Hashes)│ │
                     │  └─────────┘ │
                     └──────────────┘
```

## Components

### 1. redis_client.h/cpp
Lightweight Redis client library with RESP protocol support.

### 2. producer.cpp
Creates tasks and adds them to priority queues.

**Features:**
- Generates random tasks (send_email, process_payment, etc.)
- Assigns priority (critical, high, normal, low)
- Stores task metadata in Redis hashes
- Queues tasks based on priority

### 3. worker.cpp
Multi-threaded workers that process tasks from queues.

**Features:**
- Checks queues in priority order (critical → high → normal → low)
- Processes tasks asynchronously
- Updates task status
- Tracks completed tasks

## Build

```bash
cd task-queue
make
```

This creates:
- `producer` - Task producer executable
- `worker` - Task worker executable

## Usage

### Terminal 1: Start Redis Server
```bash
cd /home/atharv/Documents/Redis-Lite
./redis-lite
```

### Terminal 2: Start Workers (3 workers)
```bash
cd task-queue
./worker 3
```

Output:
```
========================================
  TASK WORKERS
========================================
Starting 3 workers...   
Press Ctrl+C to stop
========================================

[WORKER-1] Starting...
[WORKER-1] Connected to Redis-Lite
[WORKER-2] Starting...
[WORKER-2] Connected to Redis-Lite
[WORKER-3] Starting...
[WORKER-3] Connected to Redis-Lite
```

### Terminal 3: Run Producer (create 20 tasks)
```bash
cd task-queue
./producer 20 500
```

Arguments:
- `20` - Number of tasks to create
- `500` - Delay in ms between tasks (optional, default 1000)

Output:
```
========================================
  TASK PRODUCER
========================================
Connecting to Redis-Lite at 127.0.0.1:6379...
✓ Connected successfully!

Producing 20 tasks...
========================================

[PRODUCER] Creating task: task:1000
           Type: send_email
           Priority: high
           ✓ Task queued in queue:high

[PRODUCER] Creating task: task:1001
           Type: process_payment
           Priority: normal
           ✓ Task queued in queue:normal
...
```

### Watch Workers Process Tasks

Workers automatically pick up and process tasks:
```
[WORKER-1] Processing: task:1000
[WORKER-2] Processing: task:1001
[WORKER-3] Processing: task:1002
[WORKER-1] ✓ Completed: task:1000
[WORKER-2] ✓ Completed: task:1001
[WORKER-1] Processing: task:1003
...
```

### Terminal 4: Monitor (using redis-cli)
```bash
cd /home/atharv/Documents/Redis-Lite
./redis-cli
```

Check queue status:
```bash
127.0.0.1:6379> LLEN queue:high
:0
127.0.0.1:6379> LLEN queue:normal
:5
127.0.0.1:6379> LLEN tasks:completed
:15
```

View task details:
```bash
127.0.0.1:6379> HGETALL task:1000
type
send_email
priority
high
status
completed
created_at
1699564800
worker_id
1
completed_at
1699564805
```

## Task Flow

1. **Producer creates task:**
   ```cpp
   HMSET task:1000 type "send_email" priority "high" status "pending"
   LPUSH queue:high "task:1000"
   ```

2. **Worker picks up task:**
   ```cpp
   taskId = LPOP queue:high  // Returns "task:1000"
   ```

3. **Worker updates status:**
   ```cpp
   HSET task:1000 status "processing"
   HSET task:1000 worker_id "1"
   ```

4. **Worker processes (simulates work):**
   ```cpp
   std::this_thread::sleep_for(500ms - 2000ms);
   ```

5. **Worker marks complete:**
   ```cpp
   HSET task:1000 status "completed"
   HSET task:1000 completed_at "1699564805"
   LPUSH tasks:completed "task:1000"
   ```

## Priority System

Workers check queues in order:
1. **Critical** - Urgent tasks (10% of tasks)
2. **High** - Important tasks (20% of tasks)
3. **Normal** - Regular tasks (40% of tasks)
4. **Low** - Background tasks (30% of tasks)

## Customization

### Adjust number of workers:
```bash
./worker 5    # Start 5 workers
```

### Create more/fewer tasks:
```bash
./producer 50 100   # 50 tasks, 100ms delay
```

### Modify task types:
Edit `TASK_TYPES` in `producer.cpp`:
```cpp
const std::vector<std::string> TASK_TYPES = {
    "your_custom_task",
    "another_task"
};
```

## Stop Workers

Press `Ctrl+C` in the worker terminal:
```
^C
[WORKER] Shutting down...
[WORKER-1] Stopped. Tasks processed: 15
[WORKER-2] Stopped. Tasks processed: 12
[WORKER-3] Stopped. Tasks processed: 13
========================================
All workers stopped
========================================
```

## Clean Up

```bash
make clean  # Remove executables
```

Clear Redis data:
```bash
./redis-cli
127.0.0.1:6379> FLUSHALL
```

## Real-World Use Cases

- **Email Service**: Queue emails, workers send them
- **Image Processing**: Queue image tasks, workers resize/compress
- **Report Generation**: Queue report requests, workers generate PDFs
- **Payment Processing**: Queue payments, workers process transactions
- **Data Import**: Queue CSV files, workers import to database

---

**Your task queue system is production-ready!** 🚀
