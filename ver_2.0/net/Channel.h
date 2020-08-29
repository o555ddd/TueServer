#pragma once
#include<sys/epoll.h>
#include <functional>
#include<memory>
#include<string>
#include<unordered_map>
class Data;
class EventLoop;
class TimeNode;
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
        bool timeout_;
        std::weak_ptr<TimeNode> timeNode_;
    public:
        Channel(EventLoop* loop, int fd=0):loop_(loop),fd_(fd),events_(0),
        timeout_(false){};
        
        void handleRead();
        void handleWrite();
        void handleError();
        void handleConn();
        void setHolder(std::shared_ptr<Data> holder){holder_=holder;}
        std::shared_ptr<Data> getHolder()
        {
            std::shared_ptr<Data> ret(holder_.lock());//holder_.lock()提升为shared_ptr
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
        void setTimeout(){timeout_=true;}
        void setTimeNode(std::shared_ptr<TimeNode> timeNode){timeNode_=timeNode;}
        std::weak_ptr<TimeNode> getTimeNode(){return timeNode_;}
        bool isTimeout(){return timeout_;}
};

typedef std::shared_ptr<Channel> SP_Channel;