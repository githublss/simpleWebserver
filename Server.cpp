//
// Created by lss on 2020-05-25.
//

#include <memory.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "Server.h"
#include "ThreadPool.h"
#include "Epoll.h"


char NOT_FOUND_PAGE[] = "<html>\n"
                        "<head><title>404 Not Found</title></head>\n"
                        "<body bgcolor=\"white\">\n"
                        "<center><h1>404 Not Found</h1></center>\n"
                        "<hr><center>Lss WebServer/0.3 (Linux)</center>\n"
                        "</body>\n"
                        "</html>";

char FORBIDDEN_PAGE[] = "<html>\n"
                        "<head><title>403 Forbidden</title></head>\n"
                        "<body bgcolor=\"white\">\n"
                        "<center><h1>403 Forbidden</h1></center>\n"
                        "<hr><center>Lss WebServer/0.3 (Linux)</center>\n"
                        "</body>\n"
                        "</html>";

char INDEX_PAGE[] = "<!DOCTYPE html>\n"
                    "<html>\n"
                    "<head>\n"
                    "    <title>Welcome to LC WebServer!</title>\n"
                    "    <style>\n"
                    "        body {\n"
                    "            width: 35em;\n"
                    "            margin: 0 auto;\n"
                    "            font-family: Tahoma, Verdana, Arial, sans-serif;\n"
                    "        }\n"
                    "    </style>\n"
                    "</head>\n"
                    "<body>\n"
                    "<h1>Welcome to LC WebServer!</h1>\n"
                    "<p>If you see this page, the lss webserver is successfully installed and\n"
                    "    working. </p>\n"
                    "\n"
                    "<p>For online documentation and support please refer to\n"
                    "    <a href=\"https://github.com/MarvinLe/WebServer\">lss WebServer</a>.<br/>\n"
                    "\n"
                    "<p><em>Thank you for using LC WebServer.</em></p>\n"
                    "</body>\n"
                    "</html>";

char TEST[] = "HELLO WORLD";

extern std::string basePath;

void HttpServer::run(int thread_num, int max_queue_size) {
    ThreadPool threadPool(thread_num,max_queue_size);

    // 创建epoll实例和事件集合
    int epoll_fd = Epoll::init(1024);

    // 监听文件描述符所映射的httpData
    std::shared_ptr<HttpData> httpData(new HttpData());
    httpData->epoll_fd = epoll_fd;

    serverSocket.epoll_fd = epoll_fd;
    __uint32_t event = (EPOLLIN | EPOLLET);

    // 监听套接字描述符添加入epoll实例中
    Epoll::addfd(epoll_fd,serverSocket.listen_fd,event,httpData);

    while(true){
        // 将serversocket中状态发生变化的连接从data_map中移除，并且返回结果
        std::vector<std::shared_ptr<HttpData>> events = Epoll::poll(serverSocket,1024,-1);

        for(auto &request:events){
            // bind使用对象成员函数的指针时，必须要知道该指针属于哪个对象，因此第二个参数为对象的地址
            // 将对请求任务的相应工作添加到线程池中的线程中
            threadPool.append(request,std::bind(&HttpServer::do_request,this,std::placeholders::_1));
            std::cout<<"have a event"<<std::endl;
        }
        // 处理定时器超时事件
        Epoll::timeManager.handle_expire_event();
    }
}

void HttpServer::do_request(std::shared_ptr<void> arg) {
    std::shared_ptr<HttpData> httpData = std::static_pointer_cast<HttpData>(arg);
    char buffer[BUFFERSIZE];
    bzero(buffer,BUFFERSIZE);
    int check_index = 0,read_index = 0,startLine = 0;
    ssize_t recv_data;
    HttpParse::PARSE_STATE parseState = HttpParse::PARSE_REQUESTLINE;
    while(true){
        recv_data = recv(httpData->clientSocket->fd,buffer + read_index,BUFFERSIZE - read_index,0); //
        //TODO 对于recv错误的处理合适吗
        if(recv_data == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            std::cout<<"reading failed"<<std::endl;
            return;
        }
        // 无可用数据，或者对等方已经按序结束
        if(recv_data == 0){
            std::cout<<"connection closed by peer"<<std::endl;
            break;
        }
        read_index += recv_data;
        // 通过对请求内容解析，得到请求的状态
        HttpParse::HTTP_CODE requestCode = HttpParse::parse_content(buffer,check_index,read_index,parseState,startLine,*httpData->request);
        if(requestCode == HttpParse::NO_REQUEST){
            std::cout<<"no request:wait...."<<endl;
            continue;
        }

        else if(requestCode == HttpParse::GET_REQUEST){
//            std::unordered_map<HttpRequest::HTTP_HEADER, std::string, HttpRequest::EnumClassHash>::iterator it = httpData->request->headers.find(HttpRequest::Connection);
            auto it = httpData->request->headers.find(HttpRequest::Connection);
            if(it != httpData->request->headers.end()){
                if(it->second == "keep-alive"){
                    httpData->response->setKeepAlive(true);
                    httpData->response->addHeader("Keep-Alive",std::string("timeout=20"));
                } else{
                    httpData->response->setKeepAlive(false);
                }
            }
            header(httpData);
            getMime(httpData);
            FileState fileState = static_file(httpData);
            //TODO 后期写入日志
            std::cout<<"--------fileState is:"<<fileState<<std::endl;
            send(httpData,fileState);
            std::cout<<"--------send success--------"<<std::endl;
            // 需要keep-Alive的进行设置添加timeNode
            if(httpData->response->isKeepAlive()){
                //TODO 不进行修改会怎样
                Epoll::modfd(httpData->epoll_fd,httpData->clientSocket->fd,Epoll::DEFAULT_EVENTS,httpData);
                Epoll::timeManager.addTimer(httpData,TimeManager::DEFAULT_TIME_OUT);
            }
        } else{
            std::cout<<"bad Request"<<std::endl;
        }
    }
}

void HttpServer::header(std::shared_ptr<HttpData> httpData) {
    if(httpData->request->version == HttpRequest::HTTP_11)
        httpData->response->setVersion(HttpRequest::HTTP_11);
    else
        httpData->response->setVersion(HttpRequest::HTTP_10);
    httpData->response->addHeader("Server","lss server");
}

void HttpServer::send(HttpServer::httpData_ptr httpData, HttpServer::FileState fileState) {
    char header[BUFFERSIZE];
    bzero(header,BUFFERSIZE);
    const char *internalError = "Internal error";
    struct stat fileStat;

    //将已确定的相应头信息写入到header中,末尾是否有格式空字符
    httpData->response->appendBuffer(header);
    std::cout<<"响应文件路径为："<<httpData->response->getFilePath()<<endl;
    if(fileState == FILE_NOT_FOUND){
        if(httpData->response->getFilePath() == std::string("/")){
            sprintf(header,"%sContent-length:%d\r\n\r\n",header,strlen(INDEX_PAGE));
            sprintf(header,"%s%s",header,INDEX_PAGE);
        } else{
            sprintf(header,"%sContent-length:%d\r\n\r\n",header,strlen(NOT_FOUND_PAGE));
            sprintf(header,"%s%s",header,NOT_FOUND_PAGE);
        }
        ::send(httpData->clientSocket->fd,header,strlen(header),0);
        return;
    }

    if(fileState == FILE_FORBIDDEN){
        sprintf(header,"%sContent-length:%d\r\n\r\n",header,strlen(FORBIDDEN_PAGE));
        sprintf(header,"%s%s",header,FORBIDDEN_PAGE);
        ::send(httpData->clientSocket->fd,header,strlen(header),0);
        return;
    }

    if(stat(httpData->response->getFilePath().c_str(),&fileStat) < 0){
        sprintf(header,"%sContent-length:%d\r\n\r\n",header,strlen(internalError));
        sprintf(header,"%s%s",header,internalError);
        ::send(httpData->clientSocket->fd,header,strlen(header),0);
        return;
    }

    int filefd = ::open(httpData->response->getFilePath().c_str(),O_RDONLY);
    // 文件打开错误
    if(filefd < 0){
        sprintf(header,"%sContent-length:%d\r\n\r\n",header,strlen(internalError));
        sprintf(header,"%s%s",header,internalError);
        ::send(httpData->clientSocket->fd,header,strlen(header),0);
        return;
    }
    sprintf(header,"%sContent-length:%d\r\n\r\n",header,fileStat.st_size);
    ::send(httpData->clientSocket->fd,header,strlen(header),0);
    // 通过存储映射读取文件向客户端发送数据
    void *mapBuffer = mmap(NULL,fileStat.st_size,PROT_READ,MAP_PRIVATE,filefd,0);
    ::send(httpData->clientSocket->fd,mapBuffer,fileStat.st_size,0);
    munmap(mapBuffer,fileStat.st_size);
    close(filefd);
    return;
}

HttpServer::FileState HttpServer::static_file(HttpServer::httpData_ptr httpData) {
    struct stat fileStat;
    char file[basePath.size() + strlen(httpData->response->getFilePath().c_str()) + 1];
    strcpy(file,basePath.c_str());
    strcat(file,httpData->response->getFilePath().c_str());
    std::cout<<"+++"<<file<<std::endl;
    // 没有指明访问资源或文件不存在
    if(httpData->response->getFilePath() == "/" | (stat(file,&fileStat) < 0)){
        httpData->response->setMimeType(MimeType("text/html"));
        if(httpData->response->getFilePath() == "/"){
            httpData->response->setStatusCode(HttpResponse::OK);
            httpData->response->setStatusMsg("OK");
        }else{
            httpData->response->setStatusCode(HttpResponse::NOTFOUND);
            httpData->response->setStatusMsg("Not Found");
            std::cout<<"Not Found error"<<std::endl;
        }
        return FILE_NOT_FOUND;
    }
    // 不是普通文件的时候
    if(!S_ISREG(fileStat.st_mode)){
        httpData->response->setMimeType(MimeType("text/html"));
        httpData->response->setStatusCode(HttpResponse::FORBIDDEN);
        httpData->response->setStatusMsg("Forbidden");
        return FILE_FORBIDDEN;
    }
    // 正常文件
    httpData->response->setStatusCode(HttpResponse::OK);
    httpData->response->setStatusMsg("OK");
    httpData->response->setFilePath(file);
    std::cout<<"+++"<<file<<std::endl;
    return FILE_OK;
}

void HttpServer::getMime(HttpServer::httpData_ptr httpData) {
    std::string filePath = httpData->request->url;
    std::string mime;
    int pos;
    // 将所有参数删除
    if((pos = filePath.rfind('?')) != std::string::npos)
        filePath.erase(filePath.rfind('?'));
    // 获取到访问资源的类型名
    if(filePath.rfind('.') != std::string::npos)
        mime = filePath.substr(filePath.rfind('.'));

    decltype(mimeMap)::iterator it;
    if((it = mimeMap.find(mime)) != mimeMap.end()){
        httpData->response->setMimeType(it->second);
    } else
        httpData->response->setMimeType(mimeMap.find("default")->second);
    httpData->response->setFilePath(filePath);
}

void HttpServer::handleIndex() {
    return;
}

