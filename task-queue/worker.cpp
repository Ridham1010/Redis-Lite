#include "redis_client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <atomic>
#include <csignal>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    std::cout << "\n[WORKER] Shutting down...\n";
    running = false;
}

std::string parseTaskId(const std::string& resp) {
    // Parse RESP bulk string: $10\r\ntask:1001\r\n
    if (resp.find("$-1") != std::string::npos) {
        return "";  // Null response
    }
    
    size_t pos = resp.find("\r\n");
    if (pos == std::string::npos) return "";
    
    std::string data = resp.substr(pos + 2);
    pos = data.find("\r\n");
    if (pos == std::string::npos) return data;
    
    return data.substr(0, pos);
}

std::string getNextTask(RedisClient& client) {
    // Check queues in priority order
    std::vector<std::string> queues = {
        "queue:critical",
        "queue:high", 
        "queue:normal",
        "queue:low"
    };
    
    for (const auto& queue : queues) {
        std::string resp = client.lpop(queue);
        std::string taskId = parseTaskId(resp);
        if (!taskId.empty()) {
            return taskId;
        }
    }
    
    return "";  // No tasks available
}

void processTask(RedisClient& client, const std::string& taskId, int workerId) {
    std::cout << "[WORKER-" << workerId << "] Processing: " << taskId << std::endl;
    
    // Update task status to processing
    client.hset(taskId, "status", "processing");
    client.hset(taskId, "worker_id", std::to_string(workerId));
    
    // Update worker status (for dashboard tracking)
    std::string workerKey = "worker:" + std::to_string(workerId);
    client.hset(workerKey, "status", "processing");
    client.hset(workerKey, "current_task", taskId);
    client.hset(workerKey, "last_seen", std::to_string(time(nullptr)));
    
    // Get task details
    std::string details = client.hgetall(taskId);
    
    // Simulate work (different task types take different time)
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1500));
    
    // Mark task as completed
    client.hset(taskId, "status", "completed");
    client.hset(taskId, "completed_at", std::to_string(time(nullptr)));
    
    // Update worker status to idle
    client.hset(workerKey, "status", "idle");
    client.hset(workerKey, "current_task", "");
    client.hset(workerKey, "last_seen", std::to_string(time(nullptr)));
    
    // Add to completed list
    client.lpush("tasks:completed", taskId);
    
    std::cout << "[WORKER-" << workerId << "] ✓ Completed: " << taskId << std::endl;
}

void workerLoop(int workerId) {
    RedisClient client;
    
    std::cout << "[WORKER-" << workerId << "] Starting...\n";
    
    if (!client.connect()) {
        std::cerr << "[WORKER-" << workerId << "] Failed to connect!\n";
        return;
    }
    
    std::cout << "[WORKER-" << workerId << "] Connected to Redis-Lite\n";
    
    // Register worker on startup
    std::string workerKey = "worker:" + std::to_string(workerId);
    client.hset(workerKey, "status", "idle");
    client.hset(workerKey, "current_task", "");
    client.hset(workerKey, "started_at", std::to_string(time(nullptr)));
    client.hset(workerKey, "last_seen", std::to_string(time(nullptr)));
    
    int tasksProcessed = 0;
    
    while (running) {
        std::string taskId = getNextTask(client);
        
        if (!taskId.empty()) {
            processTask(client, taskId, workerId);
            tasksProcessed++;
        } else {
            // No tasks, update heartbeat
            client.hset(workerKey, "status", "idle");
            client.hset(workerKey, "last_seen", std::to_string(time(nullptr)));
            
            // Wait a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    // Cleanup: remove worker from registry on shutdown
    client.command({"DEL", workerKey});
    
    std::cout << "[WORKER-" << workerId << "] Stopped. Tasks processed: " << tasksProcessed << "\n";
}

int main(int argc, char* argv[]) {
    int numWorkers = 3;
    
    if (argc > 1) {
        numWorkers = std::stoi(argv[1]);
    }
    
    signal(SIGINT, signalHandler);
    
    std::cout << "========================================\n";
    std::cout << "  TASK WORKERS\n";
    std::cout << "========================================\n";
    std::cout << "Starting " << numWorkers << " workers...\n";
    std::cout << "Press Ctrl+C to stop\n";
    std::cout << "========================================\n\n";
    
    std::vector<std::thread> workers;
    
    // Start worker threads
    for (int i = 1; i <= numWorkers; i++) {
        workers.emplace_back(workerLoop, i);
    }
    
    // Wait for all workers to finish
    for (auto& worker : workers) {
        worker.join();
    }
    
    std::cout << "\n========================================\n";
    std::cout << "All workers stopped\n";
    std::cout << "========================================\n";
    
    return 0;
}
