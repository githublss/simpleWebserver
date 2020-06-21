# simpleWebserver
基于C++11实现的轻量级webserver服务器

支持GET、HEAD方法处理静态资源，并发模型采用的单进程 Reactor 非阻塞方式运行。

[测试运行实例](http://106.53.11.204:8080/)
### 使用
```
mkdir cmake-build
cd cmake-build
cmake ..
make
```
默认使用相对路径作为工作路径，可以设置指定为绝对路径。

### 开发环境
- 操作系统：Ubuntu16
- IDE集成环境：Windows下使用clion远程开发
- 编译器：gcc 5.4

### 代码统计使用工具 cloc
![QQ截图20200620161048](https://raw.githubusercontent.com/githublss/image/master/image/QQ%E6%88%AA%E5%9B%BE20200620161048.png)

查看端口被占用情况：netstat -tlunp



Now:
主要功能：
- 使用epoll 非阻塞io 边缘触发实现高并发处理请求，使用Reactor模型
- 使用线程池降低频繁创建线程带来的系统开销。
- 使用优先队列管理定时器，采用惰性删除策略。
- 使用C++11封装的thread、mutex、condition_variable实现了线程的同步互斥

Feature:
- 完成日志系统
- 支持Cookie
- 完善异常处理

参考：

[A C++ High Performance Web Server](https://github.com/linyacool/WebServer)
[High Performance HTTP WebServer](https://github.com/linw7/TKeed)