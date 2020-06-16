//
// Created by lss on 2020-06-10.
//

#ifndef HTTPSERVER_NONCOPYABLE_H
#define HTTPSERVER_NONCOPYABLE_H

// 复制构造函数和赋值构造函数定义为删除函数
class NonCopyable{
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

#endif //HTTPSERVER_NONCOPYABLE_H
