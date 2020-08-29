#include"Channel.h"
#include"EventLoop.h"

#include "Epoll.h"

void Channel::handleRead(){
    if(readHandler_)
        readHandler_();
}
void Channel::handleWrite(){
    if(writeHandler_)
        writeHandler_();
}
void Channel::handleConn(){
    if(connHandler_)
        connHandler_();
}
void Channel::handleError(){
    if(errorHandler_)
        errorHandler_();
}

void Channel::handleEvents()
{
    events_=0;
    if(!(revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        events_=0;
        return;
    }
    if(revents_ & EPOLLERR)
    {
        handleError();
        events_=0;
        return;
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        handleRead();
    if(revents_ & EPOLLOUT)
        handleWrite();
    handleConn();
}
