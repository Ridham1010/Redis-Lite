#include "../include/RedisServer.h"
#include "../include/RedisDatabase.h"
#include <iostream>
#include <thread>
using namespace std;
int main(int argc,char* argv[]){
    int port = 6379;
    if(argc>=2 )port =stoi(argv[1]); 

    //just for testing..whether database is loaded or not
    if(RedisDatabase::getInstance().load("dump.my_rdb"))
        cout<<"Database loaded dump.my_rdb\n";
    else 
        cout<<"No dump found or load failed ..starting with empty database\n";

    RedisServer server(port);
    thread persistanceThread([](){
        while(true){
            this_thread::sleep_for(chrono::seconds(300));
            
            if(!RedisDatabase::getInstance().dump("dump.my_rdb")){
                cerr<<"Error dumping database to disk\n";
            }
            else{
                cout<<"Database dumped to dump.my_rdb successfully\n";
            }    
        }
    });
    persistanceThread.detach();
}