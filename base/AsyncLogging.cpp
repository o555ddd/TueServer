#include"AsyncLogging.h"
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<functional>
#include"LogFile.h"

AsyncLogging::AsyncLogging(std::string logFileName, int flushInterval)
:flushInterval_(flushInterval), running_(false), basename_(logFileName),
thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
mutex_(), cond_(mutex_), curBuffer_(new Buffer), nextBuffer_(new Buffer),
buffers_(), latch_(1)
{
    assert(logFileName.size() > 1);
    curBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);//预留16个
}

void AsyncLogging::start()
{
    running_ = true;
    thread_.start();
    latch_.wait();
}

void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify();
    thread_.join();
}

void AsyncLogging::append(const char* logline, int len)//前端
{
    MutexLockGuard lock(mutex_);
    if(curBuffer_->avail() > len)
        curBuffer_->append(logline,len);
    else
    {
        buffers_.push_back(curBuffer_);
        curBuffer_.reset();
        if (nextBuffer_)
            curBuffer_=std::move(nextBuffer_);
        else
            curBuffer_.reset(new Buffer);
        curBuffer_->append(logline,len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc()//后端
{
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            MutexLockGuard lock(mutex_);
            if(buffers_.empty())//unusual usage!
                cond_.waitForSeconds(flushInterval_);
            buffers_.push_back(curBuffer_);
            curBuffer_.reset();
            curBuffer_=std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
                nextBuffer_ = std::move(newBuffer2);
        }

        assert(!buffersToWrite.empty());
        if(buffersToWrite.size() > 25)//避峰
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        for(size_t i=0; i<buffersToWrite.size(); ++i)
            output.append(buffersToWrite[i]->data(),buffersToWrite[i]->length());
        if(buffersToWrite.size()>2)
            buffersToWrite.resize(2);
        
        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}