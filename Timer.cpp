//
// Created by lss on 2020-06-10.
//

#include <sys/time.h>
#include "Timer.h"

// 当前时间，毫秒
std::size_t TimerNode::current_msec = 0;
//20*1000 微秒
const std::size_t TimeManager::DEFAULT_TIME_OUT = 20 * 1000;



void TimerNode::current_time() {
    struct timeval current;
    gettimeofday(&current, nullptr);
    current_msec = current.tv_sec * 1000 + current.tv_usec / 1000;
}
// 初始化，同时设置过期时间
TimerNode::TimerNode(std::shared_ptr<HttpData> httpData, size_t timeout = 0):deleted(false),httpData(httpData) {
    current_time();
    expiredTime = current_msec + timeout;
}

TimerNode::~TimerNode() {
    if(httpData){
        auto it = Epoll::httpDataMap.find(httpData->clientSocket->fd);
        if(it != Epoll::httpDataMap.end())
            Epoll::httpDataMap.erase(it);
    }
}

void TimerNode::setDeleted(bool deleted) {
    if(deleted)
        httpData.reset();
    TimerNode::deleted = deleted;
}

void TimeManager::addTimer(std::shared_ptr<HttpData> httpData, std::size_t timeout) {
    sharedPtr_TimeNode timeNode(new TimerNode(httpData,timeout));
    {
        lock_guard<std::mutex> guard(lock);
        timerQueue.push(timeNode);
        // 通过调用setTimer函数将httpData与新创建的timeNode连接起来，自动与旧的timeNode断开连接
        httpData->setTimer(timeNode);
    }
}

void TimeManager::handle_expire_event() {
    lock_guard<std::mutex> guard(lock);
    TimerNode::current_time();
    while (!timerQueue.empty()){
        sharedPtr_TimeNode timeNode = timerQueue.top();
        if(timeNode->isExpire() || timeNode->isDeleted()){
            timerQueue.pop();
        } else
            break;
    }
    std::cout<<"#######过期事件处理完成,now timerQueue.size:"<<timerQueue.size()<<std::endl;
}
