cmake_minimum_required(VERSION 3.15)

project(threadpool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件
set(SRC_LIST
    src/taskqueue.cpp
    src/threadpool.cpp
    test.cpp
)

# 生成可执行文件
add_executable(threadpool ${SRC_LIST})

# 链接 pthread 库
target_link_libraries(threadpool pthread)