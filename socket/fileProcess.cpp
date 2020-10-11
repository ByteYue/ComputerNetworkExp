#include"fileProcess.h"
//test function
void fileHandler::test(){
    cout<<this->requestType<<endl;
    cout<<this->filename<<endl;
    this->fileToString();
    cout<<this->sendMsg<<endl;
}
//construct an istringstream constance receiveMsg
fileHandler::fileHandler(string &msg){
    this->receiveMsg=istringstream(msg);
}

fileHandler::~fileHandler(){

}
//get the request type and request filename
void fileHandler::getFilename(){
    this->receiveMsg >> this->requestType >> this->filename;
    
    this->filename=this->filename.substr(1);
}
//get the respond message
int fileHandler::fileToString(){
    this->getFilename();
    string a="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
    string conLen;
    string conType="Content-Type: text/html\r\n";
    FILE* fp;
    long size;
    fp=fopen(this->filename.c_str(),"rb");
    if(fp==nullptr)
        return-1;
    fseek(fp, 0, SEEK_END);
    size=ftell(fp);//get the length
    conLen=to_string(size);
    fclose(fp);
    fstream fs(this->filename.c_str());
    stringstream ss;
    ss<<fs.rdbuf();
    this->sendMsg=a+"Content-Length: "+conLen+"\r\n"+conType+"\r\n"+ss.str();
    return this->sendMsg.length();
}

string fileHandler::getSendMsg(){
    return this->sendMsg;
}