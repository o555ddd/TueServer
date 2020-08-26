#!/bin/sh

set -x #把它下面的命令打印到屏幕

SOURCE_DIR=`pwd` #当前目录
BUILD_DIR=./build
BUILD_TYPE=Debug
#&& 前面成功才执行后面的
mkdir -p $BUILD_DIR/$BUILD_TYPE \
    && cd $BUILD_DIR/$BUILD_TYPE \
    && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            $SOURCE_DIR \
    && make 

