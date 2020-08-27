#include <getopt.h>
#include <string>
#include "net/Server.h"
#include "net/EventLoop.h"
#include<iostream>
int main(int argc,char* argv[]){
    int threadNum=4;
    int port=80;
    std::string logPath="./tueServer.log";
    int opt;
    const char* options="t:l:p:";
    while((opt=getopt(argc,argv,options)) != -1){//return int 只读取单字母
        switch(opt){
            case 't': {
                break;
            }
            case 'l': {
                break;
            }
            case 'p': {
                break;
            }
            default:
                break;
        }
    }
    EventLoop mainLoop;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();
    return 0;
}