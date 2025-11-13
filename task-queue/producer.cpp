#include "redis_client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

// Task types
const std::vector<std::string> TASK_TYPES = {
    "send_email",
    "process_payment",
    "generate_report",
    "resize_image",
    "backup_database"
};

std::string generateTaskId() {
    static int counter = 1000;
    return "task:" + std::to_string(counter++);
}

std::string randomTaskType() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, TASK_TYPES.size() - 1);
    return TASK_TYPES[dis(gen)];
}

std::string randomPriority() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 100);
    int rand = dis(gen);
    
    if (rand <= 10) return "critical";
    if (rand <= 30) return "high";
    if (rand <= 70) return "normal";
    return "low";
}

void createTask(RedisClient& client, const std::string& taskType, const std::string& priority) {
    std::string taskId = generateTaskId();
    
    std::cout << "\n[PRODUCER] Creating task: " << taskId << std::endl;
    std::cout << "           Type: " << taskType << std::endl;
    std::cout << "           Priority: " << priority << std::endl;
    
    // Store task metadata in hash
    std::vector<std::pair<std::string, std::string>> fields = {
        {"type", taskType},
        {"priority", priority},
        {"status", "pending"},
        {"created_at", std::to_string(time(nullptr))}
    };
    
    client.hmset(taskId, fields);
    
    // Add to appropriate priority queue
    std::string queueName = "queue:" + priority;
    if (priority == "critical" || priority == "high") {
        client.lpush(queueName, taskId);  // Add to front (urgent)
    } else {
        client.rpush(queueName, taskId);  // Add to back (normal)
    }
    
    std::cout << "           ✓ Task queued in " << queueName << std::endl;
}

int main(int argc, char* argv[]) {
    int numTasks = 20;
    int delayMs = 1000;
    
    if (argc > 1) numTasks = std::stoi(argv[1]);
    if (argc > 2) delayMs = std::stoi(argv[2]);
    
    std::cout << "========================================\n";
    std::cout << "  TASK PRODUCER\n";
    std::cout << "========================================\n";
    std::cout << "Connecting to Redis-Lite at 127.0.0.1:6379...\n";
    
    RedisClient client;
    if (!client.connect()) {
        std::cerr << "Failed to connect to Redis server!\n";
        std::cerr << "Make sure ./redis-lite is running\n";
        return 1;
    }
    
    std::cout << "✓ Connected successfully!\n";
    std::cout << "\nProducing " << numTasks << " tasks...\n";
    std::cout << "========================================\n";
    
    for (int i = 0; i < numTasks; i++) {
        std::string taskType = randomTaskType();
        std::string priority = randomPriority();
        
        createTask(client, taskType, priority);
        
        // Small delay between tasks
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    
    std::cout << "\n========================================\n";
    std::cout << "✓ All tasks created!\n";
    std::cout << "========================================\n";
    
    // Show queue stats
    std::cout << "\nQueue Statistics:\n";
    std::cout << "  Critical: " << client.llen("queue:critical") << std::endl;
    std::cout << "  High:     " << client.llen("queue:high") << std::endl;
    std::cout << "  Normal:   " << client.llen("queue:normal") << std::endl;
    std::cout << "  Low:      " << client.llen("queue:low") << std::endl;
    
    return 0;
}
