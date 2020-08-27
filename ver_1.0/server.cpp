#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAXLINE 4096

int main(int argc, char** argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    int n;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("error sock: %s",strerror(errno));
        return 0;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(80);

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("error bind: %s",strerror(errno));
        return 0;
    }
    if(listen(listenfd, 10) == -1)
    {
        printf("error listen: %s",strerror(errno));
        return 0;
    }
    printf("--------waiting request--------------\n");
    while(true)
    {
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1)
        {
            printf("error accept: %s",strerror(errno));
        return 0;
        }
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n]='\0';
        printf("recv: %s\n",buff);
    }
}