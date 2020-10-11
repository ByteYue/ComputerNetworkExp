#pragma once

#ifndef SERVER_H
#define SERVER_H


#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<string>
#include<cstring>
#include<list>
#include<cmath>
#include<unordered_map>

using namespace std;
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
class Server
{
private:
    SOCKET srvSocket;  //server socket
    char* recvBuf;  //receive buffer
    fd_set rfds;    //检查socket是否有数据到来的文件描述符,用于socket非阻塞模式下等待网络事件通知(有数据到爱)
    fd_set wfds;    //用于检查socket是否可以发送的文件描述符,用于socket非阻塞模式下等待网络时间通知(可以发送数据)
    sockaddr_in srvAddr;    //server IP address
    list<SOCKET>* sessions; //当前会话socket队列
    list<SOCKET>* closedSessions;   //all invalid socket queue
    list<string>* rcvedMessages;    //received clients messages
    int numOfSocketSignaled;    //readable,writable,requested socket nums
    unordered_map<SOCKET, string>* clientAddrMaps;    //client host address map
public:
	Server();
	~Server();
	//int WinsockStartup(); no need in Linux		//初始化Winsock
	int ServerStartup();		//初始化Server，包括创建SOCKET，绑定到IP和PORT
	int ListenStartup();		//开始监听客户端请求
	int Loop();					//循环执行"等待客户端请求"->“向其他客户转发信息”->"等待客户端消息"
	void AddRecvMessage(string str);								//将收到的客户端消息保存到消息队列
	void AddSession(SOCKET session);								//将新的会话socket加入队列
	void AddClosedSession(SOCKET session);							//将失效的会话socket加入队列
	void RemoveClosedSession(SOCKET closedSession);					//将失效的SOCKET从会话socket队列删除
	void RemoveClosedSession();										//将失效的SOCKET从会话socket队列删除
	void ForwardMessage();											//向其他客户转发信息
	void recvMessage(SOCKET s);										//从SOCKET s接受消息
	void sendMessage(SOCKET s,string msg);							//向SOCKET s发送消息
	string  GetClientAddress(SOCKET s);								//得到客户端IP地址
	string  GetClientAddress(unordered_map<SOCKET,string> *maps,SOCKET s);	//得到客户端IP地址
	void  ReceieveMessageFromClients();								//接受客户端发来的信息
	int AcceptRequestionFromClient();								//等待客户端连接请求
};
#endif // !SERVER_H
