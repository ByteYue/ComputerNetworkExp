#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <string>
#include <map>

using namespace std;
typedef int SOCKET;

class Server
{
private:
    int srvSocket;  //server socket
    char* recvBuf;  //recieve buffer
    fd_set rfds;    //检查socket是否有数据到来的文件描述符,用于socket非阻塞模式下等待网络事件通知(有数据到爱)
    fd_set wfds;    //用于检查socket是否可以发送的文件描述符,用于socket非阻塞模式下等待网络时间通知(可以发送数据)
    sockaddr_in srvAddr;    //server IP address
    list<SOCKET>* sessions; //当前会话socket队列
    list<SOCKET>* closedSessions;   //all invalid socket queue
    list<string>* rcvedMessages;    //recieved clients messages
    int numOfSocketSignaled;    //readable,writable,requested socket nums
    map<SOCKET, string>* clientAddrMaps;    //client host address map
protected:
	void AddRecvMessage(string str);								//将收到的客户端消息保存到消息队列
	void AddSession(SOCKET session);								//将新的会话socket加入队列
	void AddClosedSession(SOCKET session);							//将失效的会话socket加入队列
	void RemoveClosedSession(SOCKET closedSession);					//将失效的SOCKET从会话socket队列删除
	void RemoveClosedSession();										//将失效的SOCKET从会话socket队列删除
	void ForwardMessage();											//向其他客户转发信息
	void recvMessage(SOCKET s);										//从SOCKET s接受消息
	void sendMessage(SOCKET s,string msg);							//向SOCKET s发送消息
	string  GetClientAddress(SOCKET s);								//得到客户端IP地址
	string  GetClientAddress(map<SOCKET,string> *maps,SOCKET s);	//得到客户端IP地址
	void  ReceieveMessageFromClients();								//接受客户端发来的信息
	int AcceptRequestionFromClient();								//等待客户端连接请求
public:
	Server(void);
	~Server(void);
	//int WinsockStartup(); no need in Linux		//初始化Winsock
	int ServerStartup();		//初始化Server，包括创建SOCKET，绑定到IP和PORT
	int ListenStartup();		//开始监听客户端请求
	int Loop();					//循环执行"等待客户端请求"->“向其他客户转发信息”->"等待客户端消息"
};

