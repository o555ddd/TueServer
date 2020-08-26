#pragma once
#include<sys/epoll.h>
#include <functional>
#include<memory>
#include<string>
#include<unordered_map>

class EventLoop;
class Data;
class Channel{
    private:
        typedef std::function<void()> CallBack;
        EventLoop* loop_;
        int fd_;
        __uint32_t events_;//用户关心的事件
        __uint32_t revents_;//目前事件
        std::weak_ptr<Data> holder_;
        CallBack readHandler_;
        CallBack writeHandler_;
        CallBack errorHandler_;
        CallBack connHandler_;

    public:
        Channel(EventLoop* loop, int fd=0):loop_(loop),fd_(fd),events_(0){};
        
        void handleRead();
        void handleWrite();
        void handleError();
        void handleConn();
        void setHolder(std::shared_ptr<Data> holder){holder_=holder;}
        std::shared_ptr<Data> getHolder()
        {
            std::shared_ptr<Data> ret(holder_.lock());
            return ret;
        }

        void setReadHandler(CallBack &&readHandler){readHandler_=readHandler;}
        void setWriteHandler(CallBack &&writeHandler){writeHandler_=writeHandler;}
        void setErrorHandler(CallBack &&errorHandler){errorHandler_=errorHandler;}
        void setConnHandler(CallBack &&connHandler){connHandler_=connHandler;}

        void setRevents(__uint32_t ev){revents_=ev;}
        void setEvents(__uint32_t ev){events_=ev;}
        __uint32_t& getEvents(){return events_;}
        void handleEvents();
        int getFd(){return fd_;}
        void setFd(int fd){fd_=fd;};
};

typedef std::shared_ptr<Channel> SP_Channel;