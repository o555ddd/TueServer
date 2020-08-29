#pragma once
#include<sys/epoll.h>
#include<memory>
#include<unordered_map>
#include<vector>
#include"Channel.h"
#include"Data.h"
#include"IdleTimer.h"
class Epoll{
    public:
        Epoll(EventLoop* loop);
        ~Epoll(){};
        void epollAdd(SP_Channel request);
        void epollMod(SP_Channel request);
        void epollDel(SP_Channel request);
        std::vector<SP_Channel> poll();
        std::vector<SP_Channel> getEventsRequest(int eventNum);
        
        private:
            static const int MAXFDS = 100000;
            int epollFd_;
            EventLoop* loop_;
            std::vector<epoll_event> events_;
            SP_Channel fd2chan_[MAXFDS];
            std::shared_ptr<Data> fd2data_[MAXFDS];
            IdleTimer timer_;
};