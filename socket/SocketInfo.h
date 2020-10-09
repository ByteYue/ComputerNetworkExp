#pragma once
#include <list>

using namespace std;
typedef int SOCKET;
class SocketInfo
{
public:
    SocketInfo(SOCKET socket); 
    ~SocketInfo();

private:
    SOCKET socket;
    list<char *> recvStrings;
    list<char *> sendStrings;
};

