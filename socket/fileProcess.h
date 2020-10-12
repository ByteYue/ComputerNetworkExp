#pragma once
#ifndef FILEPROCESS_H
#define FILEPROCESS_H
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstring>
#include<list>

using namespace std;
class fileHandler
{
private:
    string requestType;

    string sendMsg;
    istringstream receiveMsg;
public:
    string filename;
    //list<string>* msgs;
    void test();
    fileHandler(string &msg);
    ~fileHandler();
    //hand all the request and respond
    //int handler(const string& extName);
    //get the requested file name
    void getFilename();
    string getFileExtensionName();
    //transfer image
    void imageTransfer();
    //transfer html/text
    int htmlTransfer();
    string getSendMsg();
};

#endif // !FILEPROCESS_H