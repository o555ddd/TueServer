#include"Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<functional>
#include<string.h>
#include"Util.h"
#include<stdio.h>
Server::Server(EventLoop* loop,int threadNum,int port)
    :
    loop_(loop),
    threadNum_(threadNum),
    port_(port),
    started_(false),
    acceptChannel_(new Channel(loop)),
    eventLoopThreadPool_(new EventLoopThreadPool(loop,threadNum)),
    listenFd_(socket_bind_listen(port))
{
    acceptChannel_->setFd(listenFd_);
    if(setSocketNonBlocking(listenFd_)<0)
    {
        perror("set socket non block failed");
        abort();//Aborted (core dumped)
    }
}

void Server::start()
{
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler(std::bind(&Server::handleNewConn,this));
    acceptChannel_->setConnHandler(std::bind(&Server::handleThisConn,this));
    loop_->addToPoller(acceptChannel_);
    started_ = true;
}

void Server::handleNewConn()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr,0,sizeof(struct sockaddr_in));
    int acceptFd=0;
    while((acceptFd=accept(listenFd_,(struct sockaddr*)&client_addr,&client_addr_len))>0)
    {//non blocking 写法
        //EventLoop* loop=loop_;
        EventLoop* loop=eventLoopThreadPool_->getNextLoop();
        if(acceptFd>=MAXFDS)
        {
            close(acceptFd);
            continue;
        }
        if(setSocketNonBlocking(acceptFd)<0)
        {
            std::cout << "set non blocking failed";
            return;
        }
        setSocketNoDelay(acceptFd);
        std::shared_ptr<Data> reqInfo(new Data(loop,acceptFd));
        reqInfo->getChannel()->setHolder(reqInfo);
        loop->queueInLoop(std::bind(&Data::newEvent,reqInfo));
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}