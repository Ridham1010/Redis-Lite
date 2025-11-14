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
    bool flushAll();

    //KEY- value ops
    void set(const string& key,const string& value);
    bool get(const string& key, string& value);
    vector<string> keys();
    string type(const string& key);
    bool del(const string& key);

    bool expire(const string& key,const int seconds);
    void purgeExpired();
    //rename
    bool rename(const string oldKey,const string newKey);
    //list operations
    vector<string> lget(const string& key);
    ssize_t llen(const string& key);
    void lpush(const string& key, const string& value);
    void rpush(const string& key, const string& value);
    bool lpop(const string& key, string& value);
    bool rpop(const string& key, string& value);
    int lrem(const string& key, int count, const string& value);
    bool lindex(const string& key, int index, string& value);
    bool lset(const string& key, int index, const string& value);

    //Hash ops
    bool hset(const string& key, const string& field,const string& value);
    bool hget(const string& key, const string& field,string& value);
    bool hexists(const string& key,const string& field);
    bool hdel(const string& key, const string& field);
    unordered_map<string,string>hgetall(const string& key);
    vector<string> hkeys(const string& key);
    vector<string>hvals(const string& key);
    ssize_t hlen(const string& key);
    bool hmset(const string& key, const vector<pair<string, string>>& fieldValues);


    bool dump(const string& filename);
    bool load(const string& filename);
private:
    RedisDatabase() = default;
    ~RedisDatabase() = default;
    RedisDatabase(const RedisDatabase&) = delete;
    RedisDatabase& operator=(const RedisDatabase&) = delete;
    mutex db_mutex;
    unordered_map<string,string>kv_Store;
    unordered_map<string,vector<string>>list_store;
    unordered_map<string,unordered_map<string,string>>hash_Store;

    unordered_map<string,chrono::steady_clock::time_point> expiry_map;
};    

#endif