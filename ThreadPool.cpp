//
// Created by lss on 2020-06-11.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_s, int max_queue_s):max_queue_size(max_queue_s),thread_size(thread_s),started(0),shutdown_(0){
    if(thread_s <= 0 || thread_s > MAX_THREAD_SIZE)
        thread_s = 4;
    if(max_queue_s <= 0 || max_queue_s > MAX_QUEUE_SIZE)
        max_queue_size = MAX_QUEUE_SIZE;
    threads.resize(thread_size);
    for(int i=0;i<thread_size;++i){
        threads[i] = std::thread(worker,this);
        started++;
    }
}

ThreadPool::~ThreadPool() {

}

bool ThreadPool::append(std::shared_ptr<void> arg, std::function<void(std::shared_ptr<void>)> fun) {
    if(shutdown_){
        std::cout<<"ThreadPool has been take off!!!"<<std::endl;
        return false;
    }
    // 使用lock_guard锁定互斥量
    std::lock_guard<std::mutex> lockGuard(this->mutex_);

    if(request_queue.size() > MAX_QUEUE_SIZE){
        std::cout<<"limit is:"<<MAX_QUEUE_SIZE<<std::endl;
        std::cout<<"too much request"<<std::endl;
        return false;
    }
    ThreadTask threadTask;
    threadTask.arg = arg;
    threadTask.process = fun;
    request_queue.push_back(threadTask);
    condition.notify_one();

    return true;
}

void ThreadPool::shutdown(bool graceful) {
    if(shutdown_)
        std::cout<<"has been shutdown"<<std::endl;
    condition.notify_all();
    for(int i=0;i<threads.size();++i){
        if(threads[i].joinable())
            threads[i].join();
    }
}


//TODO 每个线程都使用run方法会不会有问题
void *ThreadPool::worker(void *args) {
    auto *pool = static_cast<ThreadPool *>(args);
    if(!pool)
        return nullptr;
    prctl(PR_SET_NAME,"EventLoopThread");

    pool->run();
    return nullptr;
}

void ThreadPool::run() {
    while(true){
        ThreadTask threadTask;
        {
            //TODO 这里不写this试试
            // 这里每个线程在获取任务的时候要加锁，防止不同的线程获取到相同的任务
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            while (request_queue.empty() && !shutdown_)
                condition.wait(uniqueLock);
            if(shutdown_ == immediate_mode || (shutdown_ == graceful_mode && request_queue.empty()))
                break;
            // 先进先出的方式从任务队列中取出任务
            threadTask = request_queue.front();
            request_queue.pop_front();
        }
        threadTask.process(threadTask.arg);
    }
}
