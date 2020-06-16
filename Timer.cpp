//
// Created by lss on 2020-06-10.
//

#include "Timer.h"

std::size_t TimerNode::current_msec = 0;
//20s
const std::size_t TimeManager::DEFAULT_TIME_OUT = 20 * 1000;

void TimerNode::current_time() {

}

TimerNode::TimerNode(std::shared_ptr<HttpData> httpData, size_t timeout) {

}

TimerNode::~TimerNode() {

}

void TimerNode::del() {

}

void TimeManager::addTimer(std::shared_ptr<HttpData> httpData, std::size_t timeout) {
    sharedPtr_TimeNode timeNode(new TimerNode(httpData,timeout));
    {
        lock_guard<std::mutex> guard(lock);
        timerQueue.push(timeNode);
        httpData->setTimer(timeNode);
    }
}

void TimeManager::handle_expire_event() {

    std::cout<<"handle event"<<std::endl;
}
