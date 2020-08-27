#pragma once
#include"Channel.h"
#include<unordered_set>
#include<iostream>
class EventLoop;
class Epoll;
class TimeNode
{
    public:
        TimeNode(std::weak_ptr<Channel> channel,Epoll* poll);
        ~TimeNode();
    private:
        std::weak_ptr<Channel> channel_;
        Epoll* poll_;
};


class IdleTimer
{
    public:
        static const int period_ = 1;//check per 1 sec
        static const int EXPIRED_SEC = 5;
        typedef std::unordered_set< std::shared_ptr<TimeNode> > TimeNodeBlock;
        explicit IdleTimer(EventLoop* loop,Epoll* poll);
        void refresh(SP_Channel channel);
        void expiring();
        void addExpire(SP_Channel request);
    private:
        void handleRead();
        EventLoop* loop_;
        Epoll* poll_;
        int idx_;
        int fd_;
        TimeNodeBlock timeWheel_[EXPIRED_SEC];
        SP_Channel channel_;
};