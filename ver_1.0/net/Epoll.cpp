#include"Epoll.h"
#include<assert.h>
#include<errno.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<deque>
#include<queue>
#include"Util.h"
#include<arpa/inet.h>
#include<iostream>
#include"Channel.h"

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll(): epollFd_(epoll_create1(EPOLL_CLOEXEC)),events_(EVENTSNUM)
{
    assert(epollFd_>0);
}

void Epoll::epollAdd(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    fd2chan_[fd]=request;
    fd2data_[fd] = request->getHolder();
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epollAdd error");
        fd2chan_[fd].reset();
    }
}

void Epoll::epollMod(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        perror("epollMod error");
        fd2chan_[fd].reset();
    }
}

void Epoll::epollDel(SP_Channel request)
{
    int fd = request->getFd();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, NULL) < 0)//kernel 2.6.9后del不需要event
    {
        perror("epollMod error");
    }
    fd2chan_[fd].reset();
    fd2data_[fd].reset();
}

std::vector<SP_Channel> Epoll::poll()
{
    while(true)
    {
        int event_count=epoll_wait(epollFd_,&*events_.begin(),events_.size(),EPOLLWAIT_TIME);
        if(event_count<0)
            perror("epoll wait error");
        std::vector<SP_Channel> req_data=getEventsRequest(event_count);
        if(req_data.size()>0)
            return req_data;
    }
}

std::vector<SP_Channel> Epoll::getEventsRequest(int events_num)
{
    std::vector<SP_Channel> req_data;
    for(int i=0; i<events_num; ++i)
    {
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd2chan_[fd];
        if(cur_req)
        {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            req_data.push_back(cur_req);
        }
    }

    return req_data;
}