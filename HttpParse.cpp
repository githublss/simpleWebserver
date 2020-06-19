//
// Created by lss on 2020-06-15.
//

#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "HttpParse.h"

std::unordered_map<std::string, HttpRequest::HTTP_HEADER> HttpRequest::headerMap = {
        {"HOST",                      HttpRequest::Host},
        {"USER-AGENT",                HttpRequest::User_Agent},
        {"CONNECTION",                HttpRequest::Connection},
        {"ACCEPT-ENCODING",           HttpRequest::Accept_Encoding},
        {"ACCEPT-LANGUAGE",           HttpRequest::Accept_Language},
        {"ACCEPT",                    HttpRequest::Accept},
        {"CACHE-CONTROL",             HttpRequest::Cache_Control},
        {"UPGRADE-INSECURE-REQUESTS", HttpRequest::Upgrade_Insecure_Requests}
};

HttpParse::HTTP_CODE
HttpParse::parse_content(char *buffer, int &checkIndex, int &readIndex, HttpParse::PARSE_STATE &parseState,
                         int &start_line, HttpRequest &request) {
    LINE_STATE lineState = LINE_OK;
    HTTP_CODE httpCode = NO_REQUEST;
    while((lineState = parse_line(buffer,checkIndex,readIndex)) == LINE_OK){
        char *temp = buffer + start_line;
        start_line = checkIndex;

        switch (parseState){
            case PARSE_REQUESTLINE:{
                httpCode = parse_requestline(temp,parseState,request);
                if(httpCode == BAD_REQUEST)
                    return BAD_REQUEST;
                break;
            }
            case PARSE_HEADER:{
                httpCode = parse_headers(temp,parseState,request);
                if(httpCode == BAD_REQUEST)
                    return BAD_REQUEST;
                else if (httpCode == GET_REQUEST)
                    return GET_REQUEST;
                break;
            }
            case PARSE_BODY:{
                httpCode = parse_body(temp,request);
                if(httpCode == BAD_REQUEST)
                    return BAD_REQUEST;
                else if (httpCode == GET_REQUEST)
                    return GET_REQUEST;
                break;
            }
            default:
                return INTERNAL_ERROR;
        }
    }
    if(lineState == LINE_MORE)
        return NO_REQUEST;
    else
        return BAD_REQUEST;
}

HttpParse::LINE_STATE HttpParse::parse_line(char *buffer, int &checkedIndex, int &readIndex) {
    char temp;
    for(;checkedIndex < readIndex;++checkedIndex){
        temp = buffer[checkedIndex];
        if(temp == CR){
            if(checkedIndex + 1 == readIndex){
                return LINE_MORE;
            }
            if(buffer[checkedIndex + 1] == LF){
                buffer[checkedIndex++] = LINE_END;
                buffer[checkedIndex++] = LINE_END;
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_MORE;
}

// 对请求行进行解析
HttpParse::HTTP_CODE
HttpParse::parse_requestline(char *line, HttpParse::PARSE_STATE &parseState, HttpRequest &httpRequest) {
    // 检测line中第一个空格或者\t，返回字符
    char *url = strpbrk(line," \t");
    if(!url){
        return BAD_REQUEST;
    }
    // 将method和url分隔开
    *(url++) = '\0';
    char *method = line;
    if(strcasecmp(method,"GET") == 0){
        httpRequest.method = HttpRequest::GET;
    } else if (strcasecmp(method,"POST") == 0){
        httpRequest.method = HttpRequest::POST;
    } else if (strcasecmp(method,"PUT") == 0){
        httpRequest.method = HttpRequest::PUT;
    } else
        return BAD_REQUEST;
    // url第一个不是空格或者\t的下标
    url += strspn(url," \t");

    char *version = strpbrk(url," \t");
    if(!version)
        return BAD_REQUEST;
    *(version++) = '\0';
    version += strspn(version," \t");
    if(strncasecmp("HTTP/1.1",version,8) == 0){
        httpRequest.version = HttpRequest::HTTP_11;
    } else if (strncasecmp("HTTP/1.0",version,8) == 0)
        httpRequest.version = HttpRequest::HTTP_10;
    else
        return BAD_REQUEST;
    // 判断utl地址是否包含域名信息，并处理
    if(strncasecmp(url,"http://",7) == 0){
        url += 7;
        // 第一次出现字符/的位置
        url = strchr(url,'/');
    } else if (strncasecmp(url,"/",1) == 0){
        PASS;
    } else
        return BAD_REQUEST;

    if(!url || *url != '/'){
        return BAD_REQUEST;
    }
    httpRequest.url = std::string(url);
    parseState = PARSE_HEADER;

    return NO_REQUEST;
}

// 对请求头进行解析
HttpParse::HTTP_CODE HttpParse::parse_headers(char *line, HttpParse::PARSE_STATE &parseState, HttpRequest &request) {
    if(*line == '\0'){
        if(request.method == HttpRequest::GET)
            return GET_REQUEST;
        parseState = PARSE_BODY;
        return NO_REQUEST;
    }
    // 设置的大一点防止溢出
    char key[100],value[300];
    //TODO value的获取可能有问题
    sscanf(line,"%[^:]:%[^:]",key,value);
    decltype(HttpRequest::headerMap)::iterator it;
    std::string keys(key);
    std::transform(keys.begin(),keys.end(),keys.begin(),::toupper);
    std::string values(value);
    // 测试打印所有头信息
//    std::cout<<trim(keys)<<':'<<trim(values)<<endl;
    if((it = HttpRequest::headerMap.find(trim(keys))) != (HttpRequest::headerMap.end())){
        request.headers.insert(std::make_pair(it->second,trim(values)));
    } else{
        std::cout<<key<<" is not support"<<std::endl;
    }
    return NO_REQUEST;
}

HttpParse::HTTP_CODE HttpParse::parse_body(char *body, HttpRequest &request) {
    request.content = body;
    return GET_REQUEST;
}

