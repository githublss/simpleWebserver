//
// Created by lss on 2020-05-25.
//

#ifndef HTTPSERVER_UTIL_H
#define HTTPSERVER_UTIL_H
//#pragma once

#include <string>
using namespace std;
/*
 * 用于输出错误信息
 */
void err_print(const string& errString);

std::string &ltrim(string &);

std::string &rtrim(string &);

// 对str的左右空格换行进行处理
std::string &trim(string &);

// 将fd设置为非阻塞
int setnonblocking(int fd);

// 对sigpipe信号进行处理
void handle_for_sigpipe();

// 对当前默认路径检查其有效性
int check_base_path(char *basePath);

#endif //HTTPSERVER_UTIL_H
