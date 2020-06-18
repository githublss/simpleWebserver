//
// Created by lss on 2020-05-25.
//

#ifndef HTTPSERVER_SERVER_H
#define HTTPSERVER_SERVER_H

#include <memory>
#include "Socket.h"
#include "HttpParse.h"

class HttpData;

#define BUFFERSIZE 2048
class HttpServer{
public:
    // 访问文件状态
    enum FileState{
        FILE_OK,FILE_NOT_FOUND,FILE_FORBIDDEN
    };
    // 初始化时同时对服务端进行初始换，对服务端进行bind和listen
    explicit HttpServer(int port = 80,const char *ip = nullptr):serverSocket(port,ip){
        serverSocket.bind();
        serverSocket.listen();
    }
    // 服务器的启动函数，指定线程数和最大队列数
    void run(int thread_num,int max_queue_size = 10000);

    // 对httpData进行响应处理，通过对请求内容解析，得到请求的状态，并发送请求的资源
    void do_request(std::shared_ptr<void> arg);
private:
    using httpData_ptr = std::shared_ptr<HttpData>;

    // 根据request的头设置response基本的版本和server头信息
    void header(httpData_ptr httpData);

    // 根据访问路径判断请求的文件是否存在，如果相对路径文件存在并合法，
    // 就将httpData中的response设置为对应的文件，并返回文件的转态信息
    FileState static_file(httpData_ptr httpData);

    // 根据fileState将httpData中的response中指定要返回的文件，发送给httpData中的clientsocket
    void send(httpData_ptr httpData, FileState fileState);

    // 通过Url地址获取Mime 同时设置文件的相对文件路径到response
    void getMime(httpData_ptr httpData);

    void handleIndex();
    ServerSocket serverSocket;
};

#endif //HTTPSERVER_SERVER_H
