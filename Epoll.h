//
// Created by lss on 2020-06-10.
//

#ifndef HTTPSERVER_EPOLL_H
#define HTTPSERVER_EPOLL_H

#include <sys/epoll.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "HttpData.h"
#include "Socket.h"
#include "Timer.h"


// 对epoll的添加、删除、修改fd等基本操作进行封装
class Epoll{
public:
    // 初始化epoll中的events列表，返回epoll实例
    static int init(int max_events);
    // 将fd添加到epoll实例中，并注册要监听的事件类型，将fd添加到httpDataMap中
    static int addfd(int epoll_fd,int fd,__uint32_t events,std::shared_ptr<HttpData>);

    static int modfd(int epoll_fd,int fd,__uint32_t events,std::shared_ptr<HttpData>);

    static int delfd(int epoll_fd,int fd,__uint32_t events);
    // 调用epoll_wait方法，对events进行遍历，将httpData从HttpData_map转移到返回的结果中
    static std::vector<std::shared_ptr<HttpData>> poll(const ServerSocket &serverSocket,int max_event,int timeout);

    static void handleConnection(const ServerSocket &serverSocket);

    static std::unordered_map<int,std::shared_ptr<HttpData>> httpDataMap;
    static const int MAX_EVENTS;
    static epoll_event *events;
    static TimeManager timeManager;
    static const __uint32_t DEFAULT_EVENTS;
};




#endif //HTTPSERVER_EPOLL_H
