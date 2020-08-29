#pragma once
#include <cstdlib>
#include <string>
ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string& inBuffer, bool& zero);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string& sbuff);
int setSocketNonBlocking(int fd);
void setSocketNoDelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);