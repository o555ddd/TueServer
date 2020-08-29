#include"EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int threadNum)
:baseLoop_(baseLoop), started_(false), threadNum_(threadNum), next_(0)
{
    if(threadNum_<=0)
    {
        abort();
    }
}

void EventLoopThreadPool::start()
{
    assert(baseLoop_->isInLoopThread());
    started_ = true;
    for(int i=0;i<threadNum_;++i)
    {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());//创建线程，运行loop
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(baseLoop_->isInLoopThread());
    assert(started_);
    EventLoop* loop = baseLoop_;
    if(!loops_.empty())
    {
        loop = loops_[next_];
        next_ = (next_+1) % threadNum_;
    }
    return loop;
}