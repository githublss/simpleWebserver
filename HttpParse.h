//
// Created by lss on 2020-06-12.
//

#ifndef HTTPSERVER_HTTPPARSE_H
#define HTTPSERVER_HTTPPARSE_H

#include "HttpData.h"
#include "HttpRequest.h"

#define CR '\r'
#define LF '\n'
#define LINE_END '\0'
#define PASS

class HttpParse{
public:
    enum LINE_STATE{
        LINE_OK = 0, LINE_BAD, LINE_MORE
    };
    enum PARSE_STATE {
        PARSE_REQUESTLINE = 0, PARSE_HEADER, PARSE_BODY
    };
    enum HTTP_CODE {
        NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    static LINE_STATE parse_line(char *buffer, int &checkedIndex,int &readIndex);
    static HTTP_CODE parse_requestline(char *line,PARSE_STATE &parseState,HttpRequest &httpRequest);
    static HTTP_CODE parse_headers(char *line, PARSE_STATE &parseState, HttpRequest &request);
    static HTTP_CODE parse_body(char *body, HttpRequest &request);

    // http 请求入口，依次对request对象中的line，headers，body进行解析
    static HTTP_CODE parse_content(char *buffer,int &checkIndex,int &readIndex,PARSE_STATE &parseState,int &start_line,HttpRequest &request);
};
#endif //HTTPSERVER_HTTPPARSE_H
