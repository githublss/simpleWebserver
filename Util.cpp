//
// Created by lss on 2020-05-25.
//
#include <cstring>
#include <csignal>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <zconf.h>
#include "Util.h"

//void err_print(const string& errString){
//    std::cout << errString <<" in file <" << __FILE__ << "> "<< "at " << __LINE__ << std::endl;
//}

/*
 * 将fd设置为非阻塞式I/O
 */
int setnonblocking(int fd){
    int flags;
    if((flags = fcntl(fd,F_GETFL,0)) < 0)
        std::cout<<"f_getfl error in firle: "<<__FILE__<<" "<<"at: "<<__LINE__<<std::endl;
    flags |= O_NONBLOCK;
    if(fcntl(fd,F_SETFL,flags) < 0)
        std::cout<<"F_SETFL error in firle: "<<__FILE__<<" "<<"at: "<<__LINE__<<std::endl;
    return flags;
}

std::string &ltrim(string &str){
    if(str.empty())
        return str;
    str.erase(0,str.find_first_not_of(" \t"));
    return str;
}

std::string &rtrim(string &str){
    if(str.empty())
        return str;
    str.erase(str.find_last_not_of(" \t")+1);
    return str;
}

// 对str的左右空格换行进行处理
std::string &trim(string &str){
    if(str.empty())
        return str;
    ltrim(str);
    rtrim(str);
    return str;
}

// 对sigpipe信号进行处理
void handle_for_sigpipe(){
    // 修改对SIGPIPE信号的动作
    struct sigaction sa;
    memset(&sa,'\0', sizeof(sa));
    // SIG_IGN 用于表示 指向函数的指针，要求一个整形参数且无返回值
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    std::cout<<"已完成对sigpipe信号的处理"<<endl;
    if(sigaction(SIGPIPE,&sa,NULL))
        return;
}

// 对当前默认路径检查其有效性
int check_base_path(const string &basePath){
    struct stat file;
    if(stat(basePath.c_str(),&file) == -1){
        return -1;
    }
    // 不是文件夹或没有读的权利
    if(!S_ISDIR(file.st_mode) || access(basePath.c_str(),R_OK) == -1){
        return -1;
    }
    return 0;
}
// 非阻塞模式下，防止写入文件大于发送缓冲区，循环写入数据
ssize_t writen(int fd,const void *vptr,size_t n){
    size_t nleft=0;
    ssize_t nwritten=0;
    const char *ptr;
    ptr = static_cast<const char *>(vptr);
    nleft = n;
    while (nleft > 0){
        // 返回值为0时表示连接关闭
        if((nwritten = ::write(fd,ptr,nleft)) <= 0){
            if(nwritten < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
                nwritten = 0;
            else
                return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}
