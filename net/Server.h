#pragma once
#include<memory>
#include"EventLoop.h"
#include"Channel.h"
#include"EventLoopThreadPool.h"
class Server{
    public:
        Server(EventLoop* loop_,int threadNum_,int port_);
        void start();
        void handleNewConn();
        void handleThisConn(){loop_->updatePoller(acceptChannel_);}
    private:
        EventLoop* loop_;
        int threadNum_;
        int port_;
        bool started_;
        std::shared_ptr<Channel> acceptChannel_;
        std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
        int listenFd_;
        static const int MAXFDS = 100000;
};