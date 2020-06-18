//
// Created by lss on 2020-06-10.
//

#ifndef HTTPSERVER_HTTPDATA_H
#define HTTPSERVER_HTTPDATA_H

#include <memory>
#include "Socket.h"
#include "Timer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
/*
 * 实现对一个请求的封装
 */

// 不声明会导致was not declared in this scope
class TimerNode;

// httpdata包括：请求，响应，客户端socket，epoll_fd
// 为了能安全的生成其他额外的 std::shared_ptr 实例，继承enable_shared_from_this
class HttpData:public std::enable_shared_from_this<HttpData>{
public:
    HttpData():epoll_fd(-1){}
    // 释放timer
    void closeTimer();

    // 为timeNode赋值
    void setTimer(std::shared_ptr<TimerNode> timer);


    std::shared_ptr<HttpRequest> request;
    std::shared_ptr<HttpResponse> response;
    std::shared_ptr<ClientSocket> clientSocket;
    int epoll_fd;
private:
    std::weak_ptr<TimerNode> timer;
};

#endif //HTTPSERVER_HTTPDATA_H
