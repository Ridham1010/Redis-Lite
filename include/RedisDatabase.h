#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H
#include <string>
#include <mutex>//thread safety --prevent race conditions
#include <unordered_map>
#include<chrono>
#include <vector>

using namespace std;

class RedisDatabase {
public:
    // get the singleton instance
    static RedisDatabase& getInstance();
    // Persistance: Dump/ load the database from a file

    //Commands for interacting on the server
    bool flushAll();

    //KEY- value ops
    void set(const std::string& key,const std::string& value);
    bool get(const std::string& key, std::string& value);
    std::vector<std::string> keys();
    std::string type(const std::string& key);
    bool del(const std::string& key);
    //expire

    bool expire(const std::string& key,const int seconds);
    void purgeExpired();
    //rename
    bool rename(const std::string oldKey,const std::string newKey);
    //list operations
    std::vector<std::string> lget(const std::string& key);
    ssize_t llen(const std::string& key);
    void lpush(const std::string& key, const std::string& value);
    void rpush(const std::string& key, const std::string& value);
    bool lpop(const std::string& key, std::string& value);
    bool rpop(const std::string& key, std::string& value);
    int lrem(const std::string& key, int count, const std::string& value);
    bool lindex(const std::string& key, int index, std::string& value);
    bool lset(const std::string& key, int index, const std::string& value);

    //Hash ops
    bool hset(const std::string& key, const std::string& field,const std::string& value);
    bool hget(const std::string& key, const std::string& field,std::string& value);
    bool hexists(const std::string& key,const std::string& field);
    bool hdel(const std::string& key, const std::string& field);
    std::unordered_map<std::string,std::string>hgetall(const std::string& key);
    std::vector<std::string> hkeys(const std::string& key);
    std::vector<std::string>hvals(const std::string& key);
    ssize_t hlen(const std::string& key);
    bool hmset(const std::string& key, const std::vector<std::pair<std::string, std::string>>& fieldValues);


    bool dump(const std::string& filename);
    bool load(const std::string& filename);
private:
    RedisDatabase() = default;
    ~RedisDatabase() = default;
    RedisDatabase(const RedisDatabase&) = delete;
    RedisDatabase& operator=(const RedisDatabase&) = delete;
    mutex db_mutex;
    unordered_map<string,string>kv_Store;
    unordered_map<string,vector<string>>list_store;
    unordered_map<string,unordered_map<string,string>>hash_Store;

    std::unordered_map<std::string,std::chrono::steady_clock::time_point> expiry_map;
};    

#endif