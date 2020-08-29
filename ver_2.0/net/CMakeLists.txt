set(LIB_SRC
    Channel.cpp
    Data.cpp
    Epoll.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    IdleTimer.cpp
    Server.cpp
    Util.cpp
)



set( BASE ${PROJECT_SOURCE_DIR}/base)
message(STATUS "base addr = " ${BASE})
add_subdirectory(${BASE} baseOut)
add_library(libserver_net ${LIB_SRC})
target_link_libraries(libserver_net libserver_base)

set_target_properties(libserver_net PROPERTIES OUTPUT_NAME "net")