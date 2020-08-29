#include"EventLoopThread.h"
#include<functional>

EventLoopThread::EventLoopThread():loop_(NULL), exiting_(false), 
thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
mutex_(), cond_(mutex_) {}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()//主线程调用
{
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while(loop_ == NULL)
            cond_.wait();
    }
    return loop_;
}

void EventLoopThread::threadFunc()//副线程调用
{
    EventLoop loop;//副线程创建
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
    loop_=NULL;
}