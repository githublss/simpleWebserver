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

class HttpData;
class TimeManager;

// 对epoll的添加、删除、修改fd等基本操作进行封装
class Epoll{
public:
    // 初始化epoll中的events列表，返回epoll实例
    static int init(int max_events);

    // 将fd添加到epoll实例中，并注册要监听的事件类型，将fd添加到httpDataMap中
    static int addfd(int epoll_fd,int fd,__uint32_t events,std::shared_ptr<HttpData>);

    // 修改epoll实例中的fd，同时修改fd中httpDataMap中的映射
    static int modfd(int epoll_fd,int fd,__uint32_t events,std::shared_ptr<HttpData>);

    // 修改epoll实例中的fd，同时修改fd中httpDataMap中的映射
    static int delfd(int epoll_fd,int fd,__uint32_t events);

    // 调用epoll_wait方法，对events进行遍历，将httpData从HttpData_map转移到返回的结果中
    static std::vector<std::shared_ptr<HttpData>> poll(const ServerSocket &serverSocket,int max_event,int timeout);

    // 对新建立的连接进行处理
    static void handleConnection(const ServerSocket &serverSocket);

    static std::unordered_map<int,std::shared_ptr<HttpData>> httpDataMap;   // 保存从连连接描述符到HttpData的映射
    static const int MAX_EVENTS;                        // 最大监听数目
    static epoll_event *events;                         // epoll_event数组，存放epoll_wait返回的结果
    static TimeManager timeManager;                     // timeNode管理
    static const __uint32_t DEFAULT_EVENTS;             //默认监听事件类型
};




#endif //HTTPSERVER_EPOLL_H
