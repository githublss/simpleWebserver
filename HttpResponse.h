//
// Created by lss on 2020-06-12.
//

#ifndef HTTPSERVER_HTTPRESPONSE_H
#define HTTPSERVER_HTTPRESPONSE_H

#include "HttpRequest.h"
#include <unordered_map>
#include <string>

struct MimeType{
    MimeType(const std::string &str):type(str){};
    MimeType(const char *str):type(str){};

    std::string type;
};

extern std::unordered_map<std::string, MimeType> mimeMap;

class HttpResponse{
public:
    explicit HttpResponse(bool keep = true): statusCode(Unknow), keep_alive(keep), mimeType("text/html"), body(nullptr),
                                             version(HttpRequest::HTTP_11){};

    enum HttpStatusCode{
        Unknow, OK = 200, FORBIDDEN = 403, NOTFOUND = 404
    };

    void setStatusCode(HttpStatusCode code){
        statusCode = code;
    }

    void setBody(const char *buf){
        body = buf;
    }

    void setContentLength(int len){
        contentLength = len;
    }

    void setStatusMsg(const std::string &msg){
        statusMsg = msg;
    }

    void setFilePath(const std::string &path){
        filePath = path;
    }

    void setMimeType(const MimeType &type){
        mimeType = type;
    }

    void setKeepAlive(bool isAlive){
        keep_alive = isAlive;
    }

    void setVersion(HttpRequest::HTTP_VERSION version) {
        HttpResponse::version = version;
    }

    void addHeader(const std::string &key, const std::string &value){
        headers[key] = value;
    }

    HttpStatusCode getStatusCode() const {
        return statusCode;
    }

    HttpRequest::HTTP_VERSION getVersion() const {
        return version;
    }

    const std::string &getStatusMsg() const {
        return statusMsg;
    }

    bool isKeepAlive() const {
        return keep_alive;
    }

    const std::string &getFilePath() const {
        return filePath;
    }
    // 将相应版本信息、头信息、是否keep-alive信息写入到所传参数当中
    void appendBuffer(char *buffer) const;
private:
    HttpStatusCode statusCode;              // 状态码
    HttpRequest::HTTP_VERSION version;      // 版本信息
    std::string statusMsg;                  // 状态描述
    bool keep_alive;


private:
    // 是否keep-alive
    MimeType mimeType;                      // 响应的类型
    const char *body;                       // 响应正文
    int contentLength;                      // 正文长度
    std::string filePath;                   // 响应文件路径
    std::unordered_map<std::string,std::string> headers;    // 消息报头信息
};
#endif //HTTPSERVER_HTTPRESPONSE_H
