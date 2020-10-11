#pragma once
#include <string>
#include <netinet/in.h>

using namespace std;

class Config{
public:
    static const int MAXCONNECTION; //maximum connection numbers
    static const int BUFFERLENGTH;  //maximun buffer length
    static const string SERVERADDRESS;  // server address
    static const int PORT;          //server port
    static const u_long BLOCKMODE;   //socket block MODE
    static const string a;
private:
    Config();
    ~Config();
};
