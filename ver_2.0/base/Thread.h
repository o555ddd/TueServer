#pragma once
#include<pthread.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<functional>
#include<memory>
#include<string>
#include "CountDownLatch.h"
#include "noncopyable.h"
class Thread : noncopyable 
{
    public:
        typedef std::function<void()> ThreadFunc;
        explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
        ~Thread();
        void start();
        int join();
        bool started() const{return started_;}
        pid_t tid() const {return tid_;}
        const std::string& name() const {return name_;}
    private:
        void setDefaultName();
        bool started_;
        bool joined_;
        pthread_t pthreadId_;//进程内唯一，不同进程内可能相同
        pid_t tid_;//全局唯一
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;
};