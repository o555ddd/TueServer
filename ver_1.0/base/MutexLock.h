#pragma once
#include <pthread.h>
#include <assert.h>
#include "noncopyable.h"
class MutexLock : noncopyable
{
    public:
    MutexLock(){pthread_mutex_init(&mutex,NULL);}
    ~MutexLock(){
        //assert(holder_ == 0);
        pthread_mutex_destroy(&mutex);
    }
    void lock(){
        pthread_mutex_lock(&mutex);
    }
    void unlock(){
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_t* get(){return &mutex;}
    private:
        pthread_mutex_t mutex;
        friend class Condition;
};

class MutexLockGuard{
    public:
        explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex){mutex_.lock();}
        ~MutexLockGuard(){mutex_.unlock();}
    private:
        MutexLock& mutex_;
};