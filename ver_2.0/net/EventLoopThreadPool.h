#pragma once
#include<memory>
#include<vector>
#include"EventLoopThread.h"
#include "base/noncopyable.h"
class EventLoopThreadPool : noncopyable {
    public:
        EventLoopThreadPool(EventLoop* baseLoop, int threadNum);
        ~EventLoopThreadPool(){std::cout << "~EventLoopThreadPool()"<<std::endl;}
        void start();
        EventLoop* getNextLoop();
    private:
        EventLoop* baseLoop_;
        bool started_;
        int threadNum_;
        int next_;
        std::vector<std::shared_ptr<EventLoopThread> > threads_;
        std::vector<EventLoop*> loops_;
};