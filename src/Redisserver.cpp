#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include "../include/RedisDatabase.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<cstring>
#include<thread>
#include<signal.h>
#include<vector>


using namespace std; 

static RedisServer* globalServer =nullptr;
void signalHandler(int signum){
    if (globalServer){
        cout<<"Caught signal "<<signum <<", shutting down...\n";
        globalServer->shutdown();
    }
    exit(signum);
}
void RedisServer::setupSignalHandler(){
    signal(SIGINT,signalHandler); //ctrl+c
}
RedisServer ::RedisServer(int port) :port(port) ,server_socket(-1) ,running(true){
    globalServer =this;
    setupSignalHandler();
}

void RedisServer ::shutdown(){
    running=false;
    if(server_socket !=-1){
        //Before shut down persist the database
        if(RedisDatabase::getInstance().dump("dump.my_rdb")){
            cout<<"Database dumped to dump.my_rdb\n";
        }
        else{
            cerr<<"Error dumping database\n";
        }
        close(server_socket);
    }
    cout <<"Server Shutdown complete! \n";

}

void RedisServer::run() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error Creating Server Socket\n";
        return;
    }
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in serverAddr{};      
    serverAddr.sin_family =AF_INET;   
    serverAddr.sin_port =htons(port); // Port
    serverAddr.sin_addr.s_addr = INADDR_ANY; 

    if (::bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error Binding Server Socket\n";
        return;
    }
    if (listen(server_socket, 10) < 0) {
        cerr << "Error Listening On Server Socket\n";
        return;
    }
    cout<<"Server is listening on port " << port << "...\n";


    vector<thread> threads;
    RedisCommandHandler cmdHandler;

    while(running){
        int client_socket = accept(server_socket,nullptr, nullptr);
        if (client_socket<0){//unsuccesful
            if (running) cerr << "Couldnt accept client connection\n";
        break;
        }
        threads.emplace_back([client_socket, &cmdHandler](){
            char buffer[1024];
            while (true){
                memset(buffer,0,sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) -1, 0); 
                if (bytes<=0) break;
                string request(buffer,bytes);
                string response = cmdHandler.processCommand(request);
                send(client_socket,response.c_str(),response.size(),0);
            }
            close(client_socket);
        });
    }

    for (auto& t:threads){
        if (t.joinable())t.join();


    }
    if(RedisDatabase::getInstance().dump("dump.my_rdb")){
        cout<<"Database dumped to dump.my_rdb\n";
    }
    else{
        cerr<<"Error dumping database\n";
    }
}