#include"fileProcess.h"
#include <algorithm>
//test function
void fileHandler::test(){
    cout<<this->requestType<<endl;
    cout<<this->filename<<endl;
    this->htmlTransfer();
    cout<<this->sendMsg<<endl;
}
//construct an istringstream constance receiveMsg
fileHandler::fileHandler(string &msg){
    this->receiveMsg=istringstream(msg);
    this->getFilename();
    //this->msgs= new list<string>();
}

fileHandler::~fileHandler(){
    //if(this->msgs!= nullptr) {
    //    delete this->msgs;
    //    this->msgs= nullptr;
    //}
}

/*
//hand all the procedure
int fileHandler::handler(const string &extName) {
    if(extName==".html")
        return this->htmlTransfer();
    else if(extName==".jpg")
        return this->imageTransfer();
}
 */

//get the request type and request filename
void fileHandler::getFilename(){
    this->receiveMsg >> this->requestType >> this->filename;
    this->filename=this->filename.substr(1);
}

//get the file extension name
string fileHandler::getFileExtensionName() {
    int pos=this->filename.find('.',0);
    return this->filename.substr(pos);
}
//get the respond message
int fileHandler::htmlTransfer(){
    //this->getFilename();
    string a="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
    string conLen;
    string conType="Content-Type: text/html\r\n";
    long size;
    fstream fs(this->filename.c_str(), ios::in|ios::binary);
    stringstream ss;
    ss<<fs.rdbuf();
    fs.seekg(0,ios::end);
    size=fs.tellg();
    conLen=to_string(size);
    fs.close();
    this->sendMsg=a+"Content-Length: "+conLen+"\r\n"+conType+"\r\n"+ss.str();
    return this->sendMsg.length();
}

/*
void fileHandler::imageTransfer() {
    string a="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\r";
    string conLen;
    string conType="Content-Type: image/jpeg\r\n";
    long size;
    fstream fs(this->filename.c_str(), ios::in | ios::binary);
    stringstream ss;
    ss<<fs.rdbuf();
    fs.seekg(0,ios::end);
    size=fs.tellg();
    conLen=to_string(size);
    fs.close();
    this->msgs->insert(this->msgs->end(), a+"Content-Length: "+conLen+"\r\n"+conType);
    string sendMessage=ss.str();
    //string sendMessage=a+"Content-Length: "+conLen+"\r\n"+conType+ss.str();
    int scale=0;
    //按1024一段发送,发送多段
    while (scale<size){
        string temp=sendMessage.substr(scale,1024);
        this->msgs->insert(this->msgs->end(), temp);
        scale+=1024;
    }
    this->msgs->insert(this->msgs->end(),string(sendMessage.substr(scale-1024)));
}
 */

string fileHandler::getSendMsg(){
    return this->sendMsg;
}