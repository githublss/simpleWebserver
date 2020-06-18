//
// Created by lss on 2020-06-10.
//

#ifndef HTTPSERVER_TIMER_H
#define HTTPSERVER_TIMER_H

#include <queue>
#include <mutex>
#include <memory>

#include "HttpData.h"
#include "Epoll.h"

// 不声明会导致was not declared in this scope
class HttpData;

class TimerNode{
public:
    TimerNode(std::shared_ptr<HttpData> httpData, size_t timeout);
    ~TimerNode();

    bool isDeleted(){
        return deleted;
    }
    // 判断是否过期
    bool isExpire(){
        return expiredTime < current_msec;
    }

    // 获取过期时间
    std::size_t getExpireTime(){
        return expiredTime;
    }

    // 标记删除，并释放httpData
    void setDeleted(bool deleted);

    // 获取所指向的httpData
    std::shared_ptr<HttpData> getHttpData(){
        return httpData;
    }

    // 设置当前时间的毫秒表示
    static void current_time();

    static std::size_t current_msec;  // 当前时间的毫秒表示

private:
    bool deleted;
private:
    size_t expiredTime;             // 过期时间的时间点
    std::shared_ptr<HttpData> httpData;     // 关联的httpData
};
struct TimerCmp{
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b)const{
        return a->getExpireTime() > b->getExpireTime();
    }
};

class TimeManager{
public:
    using sharedPtr_TimeNode = std::shared_ptr<TimerNode> ;

    // 在时间队列中添加timeNode
    void addTimer(std::shared_ptr<HttpData> httpData, std::size_t timeout);

    // 将过期和被删除的TimeNode移除时间队列
    void handle_expire_event();

    const static std::size_t DEFAULT_TIME_OUT;
private:
    std::priority_queue<sharedPtr_TimeNode,std::deque<sharedPtr_TimeNode>,TimerCmp> timerQueue; // 以deque作为底层容器，来存放timeNode的优先队列
    std::mutex lock;             // 使用封装好的互斥变量锁
};
#endif //HTTPSERVER_TIMER_H
