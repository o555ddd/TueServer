#include"IdleTimer.h"
#include <sys/timerfd.h>
#include<iostream>
#include<unistd.h>
#include"Channel.h"
#include"EventLoop.h"
#include"Epoll.h"
TimeNode::TimeNode(std::weak_ptr<Channel> channel,Epoll* poll):channel_(channel),poll_(poll)
{}
TimeNode::~TimeNode()
{
    std::shared_ptr<Channel> spChannel=channel_.lock();
    if(spChannel)
        poll_->epollDel(spChannel);
}
IdleTimer::IdleTimer(EventLoop* loop,Epoll* poll):loop_(loop),poll_(poll), idx_(0)
{
    fd_=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    struct timespec now;
    struct itimerspec expire;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
    {
        std::cout << "clock_gettime error" << std::endl;
        abort();
    }
    expire.it_value.tv_sec = period_; // it_value指的是第一次到期的时间
    expire.it_value.tv_nsec = 0; 
    expire.it_interval.tv_sec = period_;
    expire.it_interval.tv_nsec = 0;
    if (timerfd_settime(fd_, 0, &expire, NULL) == -1)
    {
        std::cout << "timerfd_settime error" << std::endl;
        abort();
    }
    channel_.reset(new Channel(loop_,fd_));
    channel_->setReadHandler(std::bind(&IdleTimer::handleRead,this));
    channel_->setEvents(EPOLLIN | EPOLLET);
    poll_->epollAdd(channel_);
}
void IdleTimer::addExpire(SP_Channel request)
{
    std::shared_ptr<TimeNode> timeNode(new TimeNode(request,poll_));
    timeWheel_[(idx_+EXPIRED_SEC-1)%EXPIRED_SEC].insert(timeNode);//timenode count=1
    request->setTimeNode(timeNode);
}
void IdleTimer::expiring()
{
    timeWheel_[idx_].clear();//timenode count++
    idx_=(idx_+1)%EXPIRED_SEC;
}
void IdleTimer::refresh(SP_Channel request)
{
    std::shared_ptr<TimeNode> timeNode=request->getTimeNode().lock();
    if(timeNode)
        timeWheel_[(idx_+EXPIRED_SEC-1)%EXPIRED_SEC].insert(timeNode);//timenode count++
}
void IdleTimer::handleRead()
{
    loop_->queueInLoop(std::bind(&IdleTimer::expiring,this));//dopending时expire
    uint64_t howmany;
    ssize_t n = read(fd_, &howmany, sizeof howmany);
    //std::cout << "TimerQueue::handleRead() " << howmany << std::endl;
    if (n != sizeof howmany)
    {
        std::cout << "TimerQueue::handleRead() reads " << n << " bytes instead of 8" << std::endl;
    }
}