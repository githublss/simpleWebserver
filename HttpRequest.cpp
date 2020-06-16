//
// Created by lss on 2020-06-15.
//

#include "HttpRequest.h"

std::ostream &operator<<(std::ostream &os, const HttpRequest &request) {
    os<<"method:"<<request.method<<std::endl;
    os<<"url:"<<request.url<<std::endl;
    os<<"version:"<<request.version<<std::endl;
    for(auto it:request.headers){
        os<<it.first<<":"<<it.second<<std::endl;
    }
    return os;
}

