# TueServer
C++ high performance server
这是我学习了陈硕的muduo服务器后自己动手做的c++高性能服务器，实现了epoll + “one loop per thread”的模型。代码借鉴了https://github.com/linyacool/WebServer 。本服务器实现了简单的echo功能，可以与客户端建立tcp连接，接收客户端数据并返回给客户端。在实现过程中，我修改了linyacool/WebServer中writen的一点小bug，该bug导致在写过程中的死循环。
