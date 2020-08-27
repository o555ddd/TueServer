#include"EventLoop.h"
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include<iostream>
#include"Util.h"

__thread EventLoop* t_loopInThisThread = 0;

int createEventFd()
{
    int evtfd = eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {
        std::cout << "failed in eventfd" << std::endl;
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    :looping_(false),poller_(new Epoll()),wakeupFd_(createEventFd()),
    quit_(false),eventHandling_(false),callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),pwakeupChannel_(new Channel(this,wakeupFd_))
{
    if(!t_loopInThisThread)
        t_loopInThisThread=this;
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadHandler(std::bind(&EventLoop::handleRead,this));
    pwakeupChannel_->setConnHandler(std::bind(&EventLoop::handleConn,this));
    poller_->epollAdd(pwakeupChannel_);
}

EventLoop::~EventLoop()
{
    close(wakeupFd_);
    t_loopInThisThread = NULL;
}
void EventLoop::handleConn(){updatePoller(pwakeupChannel_);}

void EventLoop::wakeup()//主线程调用
{
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_,(char*)(&one),sizeof(one));
    if(n!=sizeof(one))
        std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << std::endl;
}

void EventLoop::handleRead()//唤醒时调用
{
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof(one));
    if(n!=sizeof(one))
        std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8" << std::endl;
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);//*
}

void EventLoop::runInLoop(Functor&& cb)
{
    if(isInLoopThread())
    {
        cb();
    }
        
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(Functor&& cb)//传入新连接事件
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

void EventLoop::loop()
{
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<SP_Channel> ret;
    while(!quit_)
    {
        std::cout << "thread " << threadId_ << std::endl;
        ret.clear();
        ret=poller_->poll();
        eventHandling_=true;
        for(auto& it: ret)
            it->handleEvents();
        eventHandling_=false;
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(size_t i=0; i<functors.size(); ++i){
        functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}