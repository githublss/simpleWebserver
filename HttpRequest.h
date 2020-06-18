//
// Created by lss on 2020-06-12.
//

#ifndef HTTPSERVER_HTTPREQUEST_H
#define HTTPSERVER_HTTPREQUEST_H

#include <unordered_map>
#include <string>
#include <iostream>

class HttpRequest;

std::ostream &operator<<(std::ostream &,const HttpRequest &);

class HttpRequest{
public:
    // 一个请求的http版本信息
    enum HTTP_VERSION {
        HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT
    };
    // 一个请求的方法
    enum HTTP_METHOD {
        GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT
    };
    // 请求头
    enum HTTP_HEADER {
        Host = 0,
        User_Agent,
        Connection,
        Accept_Encoding,
        Accept_Language,
        Accept,
        Cache_Control,
        Upgrade_Insecure_Requests
    };
    // 自定义可调用对象作为哈希函数
    struct EnumClassHash{
        template <typename T>
        std::size_t operator()(T t)const {
            return static_cast<std::size_t >(t);
        }
    };

    HttpRequest(std::string url = std::string(""),HTTP_METHOD method = METHOD_NOT_SUPPORT,HTTP_VERSION version = VERSION_NOT_SUPPORT):
    method(method),version(version),url(url),content(nullptr),headers(std::unordered_map<HTTP_HEADER,std::string,EnumClassHash>{}){};
    friend std::ostream &operator<<(std::ostream &,const HttpRequest &);

    static std::unordered_map<std::string, HTTP_HEADER> headerMap;  // 保存字段值到字段名序列号的映射

    HTTP_METHOD method;     // 请求方法
    HTTP_VERSION version;   // HTTP版本
    std::string url;        // 请求url
    char *content;          // 请求内容
    std::unordered_map<HTTP_HEADER, std::string, EnumClassHash> headers;   // 用于保存请求头的信息

};
#endif //HTTPSERVER_HTTPREQUEST_H
