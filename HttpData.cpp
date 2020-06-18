//
// Created by lss on 2020-06-10.
//

#include "HttpData.h"

void HttpData::closeTimer() {
    // 首先判断Timer是否还在， 有可能已经超时释放
    if(timer.lock()){
        std::shared_ptr<TimerNode> tempTimer(timer.lock());
        tempTimer->setDeleted(true);
        // 断开weak_ptr
        timer.reset();
    }
}

void HttpData::setTimer(std::shared_ptr<TimerNode> timer) {
    this->timer = timer;
}

