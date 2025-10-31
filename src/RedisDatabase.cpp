#include "../include/RedisDatabase.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include<iterator>
#include<algorithm>

using namespace std;

RedisDatabase& RedisDatabase::getInstance() {
    static RedisDatabase instance;
    return instance;
}

bool RedisDatabase::flushAll(){
    //Resetting a cache or starting fresh -It clears all the stored keys.
    std::lock_guard<std::mutex>lock(db_mutex);
    kv_Store.clear();
    list_store.clear();
    hash_Store.clear();
    return true;
}

//KEY- value ops
void RedisDatabase::set(const std::string& key,const std::string& value){
    std::lock_guard<std::mutex>lock(db_mutex);
    kv_Store[key]=value;

}
bool RedisDatabase::get(const std::string& key, std::string& value){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    auto it=kv_Store.find(key);
    if(it!=kv_Store.end()){
        value = it->second;
        return true;
    }
    else{
        return false;
    }

}
std::vector<std::string> RedisDatabase::keys(){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    std:: vector<std::string> result;

    for(const  auto& pair:kv_Store){
        result.push_back(pair.first);
    }
    for(const  auto& pair:list_store){
        result.push_back(pair.first);
    }
    for(const  auto& pair:hash_Store){
        result.push_back(pair.first);
    }
    return result;

}
std::string RedisDatabase::type(const std::string& key){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    if(kv_Store.find(key) !=kv_Store.end())
        return "string";
    if(list_store.find(key) !=list_store.end())
        return "list";
    if(hash_Store.find(key) !=hash_Store.end())
        return "hash";
    else return "none";
    
}
bool RedisDatabase::del(const std::string& key){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    bool erased=false;
    erased |=kv_Store.erase(key)>0;
    erased |=list_store.erase(key)>0;
    erased !=hash_Store.erase(key)>0;
    return false;
}
//expire
bool RedisDatabase::expire(const std::string& key,int seconds){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    bool exist=(kv_Store.find(key)!=kv_Store.end())||
        (list_store.find(key)!=list_store.end())||
        (hash_Store.find(key)!=hash_Store.end());
    if(!exist) return false;
    expiry_map[key]=std::chrono::steady_clock::now()+ std::chrono::seconds(seconds);
    //It stores the exact future time (current time + given seconds) at which the key should expire into the expiry_map

    return true;
}
void RedisDatabase::purgeExpired() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = expiry_map.begin(); it != expiry_map.end(); ) {
        if (now > it->second) {
            // Remove from all stores
            kv_Store.erase(it->first);
            list_store.erase(it->first);
            hash_Store.erase(it->first);
            it = expiry_map.erase(it);
        } else {
            ++it;
        }
    }
}
//rename
bool RedisDatabase::rename(const std::string oldKey,const std::string newKey){
    std::lock_guard<std::mutex>lock(db_mutex);
    purgeExpired();
    bool found=false;

    auto itkv = kv_Store.find(oldKey);
    if(itkv !=kv_Store.end()){
         kv_Store[newKey]=itkv->second;
         kv_Store.erase(itkv);
         found=true;
    }
    auto itlist= list_store.find(oldKey);
    if(itlist !=list_store.end()){
         list_store[newKey]=itlist->second;
         list_store.erase(itlist);
         found=true;
    }
    auto iths = hash_Store.find(oldKey);
    if(iths !=hash_Store.end()){
         hash_Store[newKey]=iths->second;
         hash_Store.erase(iths);
         found=true;
    }
    auto itExpire = expiry_map.find(oldKey);
    if(itExpire !=expiry_map.end()){
         expiry_map[newKey]=itExpire->second;
         expiry_map.erase(itExpire);
         found=true;
    }
    return found;
}
//list operations
std::vector<std::string> RedisDatabase::lget(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end()) {
        return it->second; 
    }
    return {}; 
}

ssize_t RedisDatabase::llen(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end()) 
        return it->second.size();
    return 0;
}

void RedisDatabase::lpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    list_store[key].insert(list_store[key].begin(), value);
}

void RedisDatabase::rpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    list_store[key].push_back(value);
}

bool RedisDatabase::lpop(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end() && !it->second.empty()) {
        value = it->second.front();
        it->second.erase(it->second.begin());
        return true;
    }
    return false;
}
bool RedisDatabase::rpop(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end() && !it->second.empty()) {
        value = it->second.back();
        it->second.pop_back();
        return true;
    }
    return false;
}

int RedisDatabase::lrem(const std::string& key, int count, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    int removed = 0;
    auto it = list_store.find(key);
    if (it == list_store.end()) 
        return 0;

    auto& lst = it->second;

    if (count == 0) {
        // Remove all occurances
        auto new_end = std::remove(lst.begin(), lst.end(), value);
        removed = std::distance(new_end, lst.end());
        lst.erase(new_end, lst.end());
    } else if (count > 0) {
        // Remove from head to tail
        for (auto iter = lst.begin(); iter != lst.end() && removed < count; ) {
            if (*iter == value) {
                iter = lst.erase(iter);
                ++removed;
            } else {
                ++iter;
            }
        }
    } else {
        // Remove from tail to head (count is negative)
        for (auto riter = lst.rbegin(); riter != lst.rend() && removed < (-count); ) {
            if (*riter == value) {
                auto fwdIter = riter.base();
                --fwdIter;
                fwdIter = lst.erase(fwdIter);
                ++removed;
                riter = std::reverse_iterator<std::vector<std::string>::iterator>(fwdIter);
            } else {
                ++riter;
            }
        }
    }
    return removed;
}

bool RedisDatabase::lindex(const std::string& key, int index, std::string& value) {
    std::lock_guard<std::mutex>lock(db_mutex);
    auto it = list_store.find(key);
    if (it == list_store.end()) 
        return false;

    const auto& lst =it->second;
    int vecSize = static_cast<int>(lst.size());
    if (index<0)
        index=lst.size() + index;//to support neg indexing
    if (index <0||index>=static_cast<int>(lst.size()))//if index is neg then comparision may not work due to implicit conversion, hence staticast

        return false;
    
    value =lst[index];
    return true;
}

bool RedisDatabase::lset(const std::string& key, int index, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it == list_store.end()) 
        return false;

    auto& lst = it->second;
    if (index < 0)
        index = lst.size() + index;
    if (index < 0 || index >= static_cast<int>(lst.size()))
        return false;
    
    lst[index] = value;
    return true;
}

//Hash ops
    bool RedisDatabase::hset(const std::string& key, const std::string& field,const std::string& value){
        std::lock_guard<std::mutex> lock(db_mutex);
        hash_Store[key][field]=value;
        return true;
    }
    bool RedisDatabase::hget(const std::string& key, const std::string& field,std::string& value){
        std::lock_guard<std::mutex> lock(db_mutex);
        auto it =hash_Store.find(key);
        if(it !=hash_Store.end()){
            auto f=it->second.find(field);
            if(f !=it->second.end()){
                value=f->second;
                return true;
            }
        }
        return false;
    }
    bool RedisDatabase::hexists(const std::string& key,const std::string& field){
        std::lock_guard<std::mutex> lock(db_mutex);
        auto it =hash_Store.find(key);
        if(it !=hash_Store.end()){
            return it->second.find(field) != it->second.end();
        }
        return false;
    }
    bool RedisDatabase::hdel(const std::string& key, const std::string& field){
        std::lock_guard<std::mutex> lock(db_mutex);
        
        auto it =hash_Store.find(key);
        if(it !=hash_Store.end()){
            return it->second.erase(field)>0;
        }
        return false;
    }
    std::unordered_map<std::string,std::string> RedisDatabase::hgetall(const std::string& key){
        std::lock_guard<std::mutex> lock(db_mutex);
        if(hash_Store.find(key)!=hash_Store.end())
            return hash_Store[key];
        return {};
    }
    std::vector<std::string> RedisDatabase::hkeys(const std::string& key){
        std::lock_guard<std::mutex> lock(db_mutex);
        std:: vector<string> fields;
        auto it =hash_Store.find(key);
        if(it!=hash_Store.end()){
            for(const auto& pair:it->second)
                fields.push_back(pair.first);
        }
        return fields;
    }
    std::vector<std::string> RedisDatabase::hvals(const std::string& key){
        std::lock_guard<std::mutex> lock(db_mutex);
        std:: vector<string> values;
        auto it =hash_Store.find(key);
        if(it!=hash_Store.end()){
            for(const auto& pair:it->second)
                values.push_back(pair.second);
        }
        return values;
    }
    ssize_t RedisDatabase::hlen(const std::string& key){
        std::lock_guard<std::mutex> lock(db_mutex);
        auto it =hash_Store.find(key);
        return (it !=hash_Store.end()) ? it->second.size() :0; 
    }
    bool RedisDatabase::hmset(const std::string& key, const std::vector<std::pair<std::string, std::string>>& fieldValues){
        std::lock_guard<std::mutex> lock(db_mutex);
        for(const auto& pair :fieldValues){
            hash_Store[key][pair.first]=pair.second;
        }
        return true;
    }
/*
Memory->file --dump()--when we close the server
File->memory --load()--when we start the server

--Server data handling--
k=Key value
L=list
H=hash

*/
bool RedisDatabase::dump(const std::string& filename) {
    lock_guard<mutex> lock(db_mutex);
    ofstream ofs(filename,ios::binary);//opens the file in binary format
    if(!ofs) return false;
    for(const auto& kv:kv_Store){
        ofs<<"K"<<kv.first<<" "<<kv.second<<"\n";
    }
    for(const auto& kv:list_store){
        ofs<<"L"<<kv.first;
        for(const auto& item:kv.second){
            ofs<<" "<<item;
        }
        ofs<<"\n";
    }
    for(const auto& kv:hash_Store){
        ofs<<"H"<<kv.first;
        for(const auto&  field_val :kv.second){
            ofs<<" "<<field_val.first<<":"<<field_val.second;
        }
        ofs<<"\n";
    }
    return true; 
}
/*

Each line encodes a record;
Key-Value (K)
kv_store["name"] = "Alice";
kv_store["city"] = "Berlin";

List (L)
list_store["fruits"] = {"apple", "banana", "orange"};
list_store["colors"] = {"red", "green", "blue"};

Hash (H)
hash_store["user:100"] = {
    {"name", "Bob"},
    {"age", "30"},
    {"email", "bob@example.com"}
};

hash_store["user:200"] = {
    {"name", "Eve"},
    {"age", "25"},
    {"email", "eve@example.com"}
};
*/
bool RedisDatabase::load(const std::string& filename) {
    lock_guard<mutex> lock(db_mutex);

    ifstream ifs(filename,ios::binary);
    if(!ifs)return false;

    kv_Store.clear();
    list_store.clear();
    hash_Store.clear();
    string line;
    while(getline(ifs,line)){
        istringstream iss(line);
        char type;
        iss >> type;
        if(type=='k'){
            string key ,value;
            iss>>key>>value;
            kv_Store[key]=value;
        }
        else if(type=='L'){
            string key;
            iss>>key;
            string item;
            vector<string>list;
            while(iss>> item){
                list.push_back(item);
            }
            list_store[key]=list;
        }
        else if(type=='H'){
            string key;
            iss>>key;
            unordered_map<string,string> hash;
            string pair;
            while(iss>>pair){
                auto pos=pair.find(':');
                if(pos !=string ::npos){
                    string field=pair.substr(0,pos);
                    string value=pair.substr(pos+1);
                    hash[field]=value;
                }
            }
            hash_Store[key]=hash;
        }
    }

    return true; 
}