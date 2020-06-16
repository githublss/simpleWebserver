//
// Created by lss on 2020-05-25.
//

#include <strings.h>

#include "Socket.h"

/*
 * 设置套接字选项，下面设置了可以重用本地地址
 */
void setReusePort(int fd){
    int opt = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const void*)&opt, sizeof(opt));
}

ServerSocket::ServerSocket(int port, const char *ip):mPort(port),mIp(ip) {
    bzero(&mAddr, sizeof(mAddr));
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);
    if(ip != nullptr){
        ::inet_pton(AF_INET,ip,&mAddr.sin_addr);    //IPv4和IPv6都适用
    } else{
        mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    listen_fd = socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd == -1){
        std::cout << "creat socket error in file <" << __FILE__ << "> "<< "at " << __LINE__ << std::endl;
        exit(0);
    }
    setReusePort(listen_fd);
    setnonblocking(listen_fd);
}

void ServerSocket::bind() {
    int ret = ::bind(listen_fd,(struct sockaddr*)&mAddr, sizeof(mAddr));
    if(ret == -1){
        err_print("bin error");
        exit(0);
    }
}

void ServerSocket::listen() {
    int ret = ::listen(listen_fd,1024);
    if(ret == -1){
        err_print("listen error");
        exit(0);
    }
}

int  ServerSocket::accept(ClientSocket &clientSocket) const {
    // 先不管客户端的套接字地址协议
    int client_fd = ::accept(listen_fd, nullptr, nullptr);
    if(client_fd < 0){
        // 阻塞与非阻塞的不同
        if(errno == EWOULDBLOCK || errno == EAGAIN)
            return client_fd;
        err_print("accept error");
    }
    clientSocket.fd = client_fd;
    return client_fd;
}

void ServerSocket::close() {
    if(listen_fd >=0){
        ::close(listen_fd);
        listen_fd = -1;
    }
}

ServerSocket::~ServerSocket(){
    close();
}

void ClientSocket::close() {
    if(fd > 0){
        cout<<"ClientSocket:"<<fd<<" closed"<<endl;
        ::close(fd);
        fd = -1;
    }

}

ClientSocket::~ClientSocket() {
    close();
}
