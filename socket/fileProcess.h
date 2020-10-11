#pragma once
#ifndef FILEPROCESS_H
#define FILEPROCESS_H
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstring>
#include <filesystem>
#include<experimental/filesystem>

using namespace std;
class fileHandler
{
private:
    string requestType;
    string filename;
    string sendMsg;
    istringstream receiveMsg;
public:
    void test();
    fileHandler(string &msg);
    ~fileHandler();
    void getFilename();
    int fileToString();
    string getSendMsg();
};

#endif // !FILEPROCESS_H