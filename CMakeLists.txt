cmake_minimum_required(VERSION 3.5.1)
project(httpServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_CXX_STANDARD 14)

add_executable(httpServer main.cpp Util.h Util.cpp Server.h Server.cpp Socket.h Socket.cpp HttpData.h HttpData.cpp Epoll.h Epoll.cpp NonCopyable.h Timer.h Timer.cpp ThreadPool.h ThreadPool.cpp HttpResponse.h HttpRequest.h HttpParse.h HttpParse.cpp HttpRequest.cpp HttpResponse.cpp)