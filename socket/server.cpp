#include<iostream>
#include<fstream>
#include<sstream>
#include"server.h"
#include"config.h"
Server::Server(){//initialize the receive buffer
    this->recvBuf=new char[Config::BUFFERLENGTH];
    memset(this->recvBuf,'\0', Config::BUFFERLENGTH);
}

Server::~Server(){
    if(this->recvBuf!=nullptr){
        delete this->recvBuf;
        this->recvBuf=nullptr;
    }
    if(this->serverSocket!=NULL){
        close(this->serverSocket);
        this->serverSocket=NULL;
    }
    if(this->session!=NULL){
        closeSession();
    }
}

int Server::serverStart(){
    //creat TCP socket
    this->serverSocket=socket(AF_INET, SOCK_STREAM,0);
    if(serverSocket==-1){
        std::cout<<"ERROR:socket"<<std::endl;
        return 0;
    }
    std::cout<<"Server socket created"<<std::endl;
    this->serverAddr.sin_family=AF_INET;//IPV4
    this->serverAddr.sin_port=htons(Config::port);
    this->serverAddr.sin_addr.s_addr=INADDR_ANY;//TCP
    //bind serverSocket to Server'IP and port
    if((bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))==-1){
        std::cout<<"Server Bind Error"<<std::endl;
        close(this->serverSocket);
        return -1;
    }
    std::cout<<"Server Socket Bind Established"<<std::endl;
    return 0;
}

int Server::serverListen(){//listen to the client
    if((listen(this->serverSocket,Config::CONNECTIONNUMS))==-1){
        std::cout<<"Server Socket Listen Error"<<std::endl;
        close(this->serverSocket);
        return -1;
    }
    std::cout<<"Server Socket Listen Established"<<std::endl;
    return 0;
}

int Server::acceptRequestFromClient(){//get the client socket
    sockaddr_in clientAddr;
    socklen_t addressLen=sizeof(clientAddr);
    char clientIP[INET_ADDRSTRLEN]="";
    this->session = accept(this->serverSocket,(sockaddr*)&clientAddr,&addressLen);
    if(this->session==-1){
        std::cout<<"Server Accept Connetion Failed"<<std::endl;
        return -1;
    }
    inet_ntop(AF_INET,&clientAddr.sin_addr,clientIP,INET_ADDRSTRLEN);
    std::cout<<clientIP<<":"<<ntohs(clientAddr.sin_port)<<std::endl;
    return 0;
}

int Server::loop(){
    while(true){
        if((this->acceptRequestFromClient())!=0)
            return -1;
        
        /*
        while(true){
            this->receieveMessage();
        }
        */
        this->closeSession();
    }
    return 0;
}

int Server::fileProcess(string& s){//process the file to HTTP respond message
    string a="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
    string conLen;
    string conType="Content-Type: text/html\r\n";
    FILE* fp;
    long size;
    fp=fopen("index.html","rb");
    if(fp==nullptr)
        return;
    fseek(fp, 0, SEEK_END);
    size=ftell(fp);//get the length
    conLen=to_string(size);
    fclose(fp);
    fstream fs("index.html");
    stringstream ss;
    ss<<fs.rdbuf();
    s=a+"Content-Length: "+conLen+"\r\n"+conType+"\r\n"+ss.str();
    return s.length();
}

void Server::sendMessage(){
    string s;
    int length=fileProcess(s);
    send(this->session, s.c_str(), length, 0);
}

void Server::receieveMessage(){
    int receivedBytes=recv(this->session,this->recvBuf,Config::BUFFERLENGTH,0);
    if(receivedBytes==-1 || receivedBytes==0){
        close(this->session);
        std::cout<<"Out"<<std::endl;
    }
    else{
        this->recvBuf[receivedBytes]='\0';
        cout<<this->recvBuf<<std::endl;
        memset(this->recvBuf,'\0',Config::BUFFERLENGTH);
    }
}

void Server::closeSession(){
    close(this->session);
}