#pragma once

#ifndef SERVER_H
#define SERVER_H

#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<cstring>
using namespace std;
typedef int SOCKET;

class Server
{
private:
    SOCKET serverSocket;//welcome socket
    SOCKET session;//connection socket, message transfers between connection socket and client socket
    char* recvBuf;
    //fd_set
    sockaddr_in serverAddr;
public:
    Server(/* args */);
    ~Server();
    int serverStart();//creat and bind the welcome socket
    int serverListen();//listen to the client
    int acceptRequestFromClient();//get the client socket
    int loop();
    void receieveMessage();//receive messsage from the client
    void sendMessage();//send msg to session socket
    int fileProcess(string &s);//return the length of msg
    void closeSession();//close the connection socket
};
#endif // !SERVER_H

