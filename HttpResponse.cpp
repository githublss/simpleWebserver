//
// Created by lss on 2020-06-15.
//

#include "HttpResponse.h"

std::unordered_map<std::string, MimeType> mimeMap = {
        {".html", "text/html"},
        {".xml", "text/xml"},
        {".xhtml", "application/xhtml+xml"},
        {".txt", "text/plain"},
        {".rtf", "application/rtf"},
        {".pdf", "application/pdf"},
        {".word", "application/msword"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".au", "audio/basic"},
        {".mpeg", "video/mpeg"},
        {".mpg", "video/mpeg"},
        {".avi", "video/x-msvideo"},
        {".gz", "application/x-gzip"},
        {".tar", "application/x-tar"},
        {".css", "text/css"},
        {"", "text/plain"},
        {"default","text/plain"}
};
void HttpResponse::appendBuffer(char *buffer) const {
    if(version == HttpRequest::HTTP_11)
        sprintf(buffer,"HTTP/1.1 %d %s\r\n",statusCode,statusMsg.c_str());
    else
        sprintf(buffer,"HTTP/1.0 %d %S\r\n",statusCode,statusMsg.c_str());
    for(auto it:headers)
        sprintf(buffer,"%s%s:%s\r\n",buffer,it.first.c_str(),it.second.c_str());
    sprintf(buffer,"%sContent-type: %s\r\n",buffer,mimeType.type.c_str());
    if(isKeepAlive())
        sprintf(buffer,"%sConnection: keep-alive\r\n",buffer);
    else
        sprintf(buffer,"%sConnection: close\r\n",buffer);
}

