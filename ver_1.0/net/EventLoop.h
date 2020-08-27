#pragma once
#include<functional>
#include<memory>
#include<vector>
#include"Channel.h"
#include"Epoll.h"
#include"Util.h"
#include"base/Thread.h"
#include<iostream>
#include"base/CurrentThread.h"
class EventLoop{
    public:
        typedef std::function<void()> Functor;
        EventLoop();
        ~EventLoop();
        void loop();
        void quit();
        void runInLoop(Functor&& cb);
        void queueInLoop(Functor&& cb);
        bool isInLoopThread() const 
        {
            return threadId_ == CurrentThread::tid();
        }
        void shutdown(SP_Channel channel){shutDownWR(channel->getFd());}
        void removeFromPoller(SP_Channel channel){poller_->epollDel(channel);}
        void updatePoller(SP_Channel channel){poller_->epollMod(channel);}
        void addToPoller(SP_Channel channel){poller_->epollAdd(channel);}
    private:
        bool looping_;
        std::shared_ptr<Epoll> poller_;
        int wakeupFd_;
        bool quit_;
        bool eventHandling_;
        bool callingPendingFunctors_;
        const pid_t threadId_;
        std::shared_ptr<Channel> pwakeupChannel_;
        mutable MutexLock mutex_;
        std::vector<Functor> pendingFunctors_;
        void wakeup();
        void handleRead();
        void doPendingFunctors();
        void handleConn();
};