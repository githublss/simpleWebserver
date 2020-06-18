//
// Created by lss on 2020-05-25.
//

#ifndef HTTPSERVER_SOCKET_H
#define HTTPSERVER_SOCKET_H

#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "Util.h"
class ClientSocket;

// 设置套接字选项，允许重用本地地址
void setReusePort(int fd);

// 服务端
class ServerSocket{
public:
    // 初始化套接字，并将监听套接字设置为非阻塞
    ServerSocket(int port = 8080,const char *ip = nullptr);
    ~ServerSocket();

    // 为监听描述符绑定地址和端口号
    void bind();

    // 调用listen函数将主动套接字转为被动套接字
    void listen();

    // 关闭socket文件描述符
    void close();

    /*
     * 包裹accept方法，传入客户端，并将accept函数创建返回的已连接套接字描述符赋值给客户端，
     * 并对非阻塞的监听套接字进行逻辑处理判断，返回已连接套接字描述符
     */
    int accept(ClientSocket &) const;

    sockaddr_in mAddr;      // 套接字地址
    int listen_fd;          // 监听套接字描述符
    int epoll_fd;           // epoll实例中包含所有的epoll_event
    int mPort;              // 绑定端口号
    const char *mIp;
};

// 客户端
class ClientSocket{
public:
    ClientSocket():fd(-1){};
    ~ClientSocket();

    void close();

    sockaddr_in mAddr;
    socklen_t mLen;
    int fd;
};

#endif //HTTPSERVER_SOCKET_H
