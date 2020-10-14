//#pragma once
#include <string>
#include <netinet/in.h>
#include <unordered_map>

using namespace std;

class Config{
public:
    static const int MAXCONNECTION; //maximum connection numbers
    static const int BUFFERLENGTH;  //maximun buffer length
    static const string SERVERADDRESS;  // server address
    static const int PORT;          //server port
    static const u_long BLOCKMODE;   //socket block MODE
    static const string a;
    //static const unordered_map<string ,string > respondFileType;
private:
    Config();
    ~Config();
};
