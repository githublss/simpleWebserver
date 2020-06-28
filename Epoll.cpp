//
// Created by lss on 2020-06-10.
//
#include "Epoll.h"



std::unordered_map<int,std::shared_ptr<HttpData>> Epoll::httpDataMap;
const int Epoll::MAX_EVENTS = 1000;
epoll_event *Epoll::events;

// 可读 | ET模 | 保证一个socket连接在任一时刻只被一个线程处理
const __uint32_t Epoll::DEFAULT_EVENTS = (EPOLLIN | EPOLLET | EPOLLONESHOT);

TimeManager Epoll::timeManager;

int Epoll::init(int max_events) {
    int epoll_fd = ::epoll_create(max_events);
    if(epoll_fd == -1){
        std::cout<<"eopll creat error"<<std::endl;
        exit(-1);
    }
    events = new epoll_event[max_events];
    return epoll_fd;
}

int Epoll::addfd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData> httpData) {
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    httpDataMap[fd] = httpData;
    int ret = ::epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
    if(ret < 0){
        std::cout<<"eopll add error"<<std::endl;
        httpDataMap[fd].reset();
        return -1;
    }
    return 0;
}

int Epoll::modfd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData> httpData) {
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    httpDataMap[fd] = httpData;
    int ret = ::epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&event);
    if(ret < 0){
        std::cout<<"eopll mod error"<<std::endl;
        httpDataMap[fd].reset();
        return -1;
    }
    return 0;
}

int Epoll::delfd(int epoll_fd, int fd, __uint32_t events) {
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = ::epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&event);
    if(ret < 0){
        std::cout<<"eopll del error"<<std::endl;
        httpDataMap[fd].reset();
        return -1;
    }
    auto it = httpDataMap.find(fd);
    if(it != httpDataMap.end()){
        httpDataMap.erase(it);
    }
    return 0;
}

std::vector<std::shared_ptr<HttpData>> Epoll::poll(const ServerSocket &serverSocket, int max_event, int timeout) {
    int event_num = epoll_wait(serverSocket.epoll_fd,events,max_event,timeout);
    if(event_num < 0){
        std::cout<<"eventNum is:"<<event_num<<" epoll_wait error:"<<errno<<std::endl;
        exit(-1);
    }

    std::vector<std::shared_ptr<HttpData>> httpDatas;
    for(int i=0;i<event_num;++i){
        int fd = events[i].data.fd;
        if(fd == serverSocket.listen_fd){
            handleConnection(serverSocket);
        } else{
            // 发生错误或被挂断
            if((events[i].events & EPOLLERR) ||(events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP)){
                auto it = httpDataMap.find(fd);
                if(it != httpDataMap.end()){
                    it->second->closeTimer();
                }
                continue;
            }
            auto it = httpDataMap.find(fd);
            if(it != httpDataMap.end()){
                if((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI)){
                    httpDatas.push_back(it->second);
                    it->second->closeTimer();
                    httpDataMap.erase(it);
                }
            }else{
                std::cout<<"没有找到第二次长连接"<<std::endl;
                ::close(fd);
                continue;
            }
        }
    }

    return httpDatas;
}

void Epoll::handleConnection(const ServerSocket &serverSocket) {
    std::shared_ptr<ClientSocket> tempClient(new ClientSocket());
    int clientFd;
//    clientFd = serverSocket.accept(*tempClient);
    while((clientFd = serverSocket.accept(*tempClient)) > 0) {
        std::cout << "new fd:" << clientFd << std::endl;
        int ret = setnonblocking(tempClient->fd);
        //TODO 没有设置成功就关闭的方法是不是有点不合适
        if (ret < 0) {
            std::cout << "setnonblocking error" << std::endl;
            tempClient->close();
            continue;
        }
        std::shared_ptr<HttpData> httpDataPtr(new HttpData());
        httpDataPtr->request = std::shared_ptr<HttpRequest>(new HttpRequest());
        httpDataPtr->response = std::shared_ptr<HttpResponse>(new HttpResponse());

        std::shared_ptr<ClientSocket> clientSocketPtr(new ClientSocket());
        clientSocketPtr.swap(tempClient);
        httpDataPtr->clientSocket = clientSocketPtr;
        httpDataPtr->epoll_fd = serverSocket.epoll_fd;

        // 将一个连接文件描述符添加到epoll实例中
        addfd(serverSocket.epoll_fd, clientSocketPtr->fd, DEFAULT_EVENTS, httpDataPtr);
        timeManager.addTimer(httpDataPtr, TimeManager::DEFAULT_TIME_OUT);
    }
    std::cout<<"已连接描述符已处理完完毕"<<clientFd<<std::endl;
}
