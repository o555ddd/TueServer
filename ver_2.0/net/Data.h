#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include<functional>
#include<map>
#include<memory>
#include<string>
#include<unordered_map>
#include<iostream>
class EventLoop;
class Channel;

class Data : public std::enable_shared_from_this<Data>//安全地生成其他额外的 std::shared_ptr 实例
{
    public:
        Data(EventLoop* loop, int connfd);
        ~Data() {
            close(fd_);
            }
        void newEvent();
        int getFd(){return fd_;}
        std::shared_ptr<Channel> getChannel(){return channel_;}
    private:
        EventLoop* loop_;
        std::shared_ptr<Channel> channel_;
        int fd_;
        std::string inBuffer_;
        std::string outBuffer_;
        bool error_;
        void handleRead();
        void handleWrite();
};