C++ high performance server 这是我学习了陈硕的muduo服务器后自己动手做的c++高性能服务器，实现了epoll + “one loop per thread”的模型。代码借鉴了https://github.com/linyacool/WebServer 。

ver 1.0实现了epoll + “one loop per thread”的模型。实现了echo协议，可以与客户端建立tcp连接，接收客户端数据并返回给客户端。服务器以事件回调的方式处理读写。

ver 2.0实现了定时踢走空闲连接功能，用于踢走长期空闲连接和未妥善关闭的半断开连接。linyacool的实现方法是用小顶堆保存连接时间戳，在每次poller响应事件的周期里处理过时时间戳。小顶堆的写复杂度是O(logn)，并且不会排重；当事件频繁发生时，会频繁地启动过期检查。

TUE采用了定时器+时间桶轮模型。新来的连接会创建一个计时器节点，采用shared_ptr保存。设过期周期为5秒，则timer中有5个桶，定时器会指向其中一个桶，并不停轮转。对于指到的桶，定时器会清除里面的计时器节点shared_ptr，当有新来连接或连接有新的响应时，将该连接的计时器节点的shared_ptr加入指向的前一个桶内。当连接的计时器节点的shared_ptr不存在于任何一个桶内时，计时器节点自动销毁，析构函数内关闭连接。时间桶轮的过期时间并不精确，有一秒内的误差，这对于踢走空闲连接应用来说是可以接受的。采用定时器，不需要频繁触发过期检查。桶的容器是unordered_set,读写复杂度为O(1),且在会排除一秒内重复的计时器。

ver 3.0实现了缓存日志系统。实现了LOG流输出日志，日志缓存采用懒汉单例实现，应用往log流里写，流导入到缓存，再在日志线程将缓存里的日志写硬盘。为了达到高效写，会尽量存够4KB日志再写硬盘。
