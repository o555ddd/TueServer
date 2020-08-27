cmake_minimum_required(VERSION 2.8)

project(Tue CXX)
set(CMAKE_BUILD_TYPE "DEBUG")
set(SRC main.cpp)
set(TESTSERV server.cpp)
set(ESERV EpollServer.cpp)
set(ECLI EpollClient.cpp)
set(CXX_FLAGS
    -g
    -Wall
    -std=c++11
    -D_PTHREADS
    -Wno-unused-parameter
)
include_directories(${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}})

add_subdirectory(net)
add_executable(TueServer ${SRC})
add_executable(server ${TESTSERV})
add_executable(EpollServer ${ESERV})
add_executable(EpollClient ${ECLI})
target_link_libraries(TueServer libserver_net)