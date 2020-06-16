//
// Created by lss on 2020-06-11.
//

#ifndef HTTPSERVER_THREADPOOL_H
#define HTTPSERVER_THREADPOOL_H

#include <iostream>

#include <sys/prctl.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <thread>
#include <future>

// 最大线程数和最大队列数
const int MAX_THREAD_SIZE = 1024;
const int MAX_QUEUE_SIZE = 10000;

using shutdownMode = enum{
    immediate_mode = 1,
    graceful_mode = 2
};

// 线程任务结构体
struct ThreadTask {
    std::function<void(std::shared_ptr<void>)> process;     // 实际传入的是Server::do_request;
    std::shared_ptr<void> arg;                              // 实际应该是HttpData对象
};

class ThreadPool{
public:
    ThreadPool(int thread_s, int max_queue_s);
    ~ThreadPool();

    bool append(std::shared_ptr<void> arg, std::function<void(std::shared_ptr<void>)> fun);
    // 完成所有任务以后关闭线程池
    void shutdown(bool graceful);

private:
    static void *worker(void *args);
    void run();
    //TODO 使用库函数中的packaged_task
    using Task = std::function<void()>;
    std::mutex mutex_;  // 互斥体
    std::condition_variable_any condition;
    int thread_size;
    int max_queue_size;
    int started;
    int shutdown_;
    std::vector<std::thread> threads;
    std::list<ThreadTask> request_queue;
};

#endif //HTTPSERVER_THREADPOOL_H
