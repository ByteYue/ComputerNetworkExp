#pragma once

#ifndef CONFIG_H
#define CONFIG_H
#include<string>
using namespace std;

class Config
{
public:
    static const int BUFFERLENGTH;//maximun buffer length
    static const int CONNECTIONNUMS;//maximum connection socket nums
    static const string SERVERADDR;//IP address of server
    static const int port;//server port
    Config(/* args */);
    ~Config();

};
#endif // !CONFIG_H