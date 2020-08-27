#include"Data.h"
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<iostream>
#include"Channel.h"
#include"EventLoop.h"
#include"Util.h"
const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;

Data::Data(EventLoop* loop, int connfd):loop_(loop),
    channel_(new Channel(loop, connfd)),fd_(connfd), error_(false)
{
    channel_->setReadHandler(std::bind(&Data::handleRead,this));
    channel_->setWriteHandler(std::bind(&Data::handleWrite,this));
    //channel_->setConnHandler(bind(&HttpData::handleConn,this));
}
void Data::newEvent()
{
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addToPoller(channel_);
}
void Data::handleRead()
{
    bool zero=false;
    int read_num = readn(fd_,inBuffer_,zero);
    if(read_num<0)
        std::cout << "Data read error" <<std::endl;
    else{
        outBuffer_+=inBuffer_;
        inBuffer_.clear();
        handleWrite();
    }
    
}

void Data::handleWrite()
{
    __uint32_t& events_ = channel_->getEvents();
    if(writen(fd_,outBuffer_)<0)
    {
        std::cout << "Data write error" <<std::endl;
    }
    if(outBuffer_.size() > 0 )
        events_ |= EPOLLOUT;
}