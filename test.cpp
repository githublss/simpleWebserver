//
// Created by lss on 2020-06-18.
//
#include "Util.h"
#include <string>
#include <iostream>

using namespace std;

void test_ltrim() {
    std::string s = "  \thelloworld\t ";
    ltrim(s);
    cout<<s<<endl;
    if (s == "helloworld\t ") {
        cout << "ltrim ok" << endl;
    } else {
        cout << "test ltrim faild" << endl;
    }
}
void test_rtrim() {
    std::string s = "  \thelloworld\t ";
    rtrim(s);
    cout<<s<<endl;
    if (s == "  \thelloworld") {
        cout << "rtrim ok" << endl;
    } else {
        cout << "test rtrim faild" << endl;
    }
}

void test_trim() {
    std::string s = "  \t1\t  ";
    trim(s);
    cout<<s<<endl;
    if (s == "1") {
        cout << "trim ok" << endl;
    } else {
        cout << "test trim faild" << endl;
    }
}



int main1() {
    test_ltrim();
    test_rtrim();
    test_trim();
}
