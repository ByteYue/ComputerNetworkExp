//#pragma once
#ifndef FILEPROCESS_H
#define FILEPROCESS_H
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstring>
#include<list>
#include<unordered_map>

using std::string;
using std::istringstream;
using std::unordered_map;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::end;
class fileHandler
{
private:
    istringstream receiveMsg;
public:
    string requestType;
    string sendMsg;
    string protocolVersion;
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
    //void imageTransfer();
    //transfer html/text
    void notFound();
    void notSupport();
    void redirect();
    bool htmlTransfer();
    string getSendMsg();

    static const unordered_map<string ,string >respondFileType;
    //bool fileExist();
};

#endif // !FILEPROCESS_H