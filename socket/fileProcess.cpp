#include"fileProcess.h"
#include <algorithm>

//using FileProcess::fileHandler;
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
bool fileHandler::htmlTransfer(){
    fstream fs(this->filename.c_str(), ios::in|ios::binary);
    if(!fs.is_open())
        return false;
    //this->getFilename();
    else{
        string a="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
        string conLen;
        string conType="Content-Type: text/html\r\n";
        long size;

        std::stringstream ss;
        ss<<fs.rdbuf();
        fs.seekg(0,ios::end);
        size=fs.tellg();
        conLen=std::to_string(size);
        fs.close();
        this->sendMsg=a+"Content-Length: "+conLen+"\r\n"+conType+"\r\n"+ss.str();
        return true;
    }
}

const unordered_map<string ,string >fileHandler::respondFileType={
        {".html","Content-Type: text/html\r\n\r\n"},
        {".xml", "Content-Type: application/xml\r\n\r\n"},
        {".jpg","Content-Type: image/jpeg\r\n\r\n"},
        {".png","Content-Type: image/png\r\n\r\n"}
};

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