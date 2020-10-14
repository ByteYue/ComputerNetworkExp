#include<iostream>
#include<algorithm>
#include"server.h"
#include"config.h"
//#include"fileProcess.h"
//using FileProcess::fileHandler;
Server::Server()
{
	this->recvBuf = new char[Config::BUFFERLENGTH]; //初始化接受缓冲区
	memset(this->recvBuf,'\0',Config::BUFFERLENGTH);
	this->rcvedMessages = new list<string>();
	this->sessions = new list<SOCKET>();
	this->closedSessions = new list<SOCKET>();
	this->clientAddrMaps = new unordered_map<SOCKET,string>();
}
Server::~Server()
{
	//释放接受缓冲区
	if(this->recvBuf != nullptr){
		delete[] this->recvBuf;
		this->recvBuf = nullptr;
	}
	//关闭server socket
	if(this->srvSocket != NULL){
		//closesocket(this->srvSocket);
		close(this->srvSocket);
		this->srvSocket = NULL;
	}
	//关闭所有会话socket并释放会话队列
	if(this->sessions != nullptr) {
		for(auto itor = this->sessions->begin();itor!= this->sessions->end();itor++)
			close(*itor); //关闭会话
		delete this->sessions;  //释放队列
		this->sessions = nullptr;
	}
	//释放失效会话队列
	if(this->closedSessions != nullptr){
		for(auto itor = this->closedSessions->begin();itor!= this->closedSessions->end();itor++)
			close(*itor); //关闭会话
		delete this->closedSessions;//释放队列
		this->closedSessions = nullptr;
	}
	//释放接受消息队列
	if(this->rcvedMessages != nullptr){
		this->rcvedMessages->clear(); //清除消息队列中的消息
		delete this->rcvedMessages;	// 释放消息队列
		this->rcvedMessages = nullptr;
	}
	//释放客户端地址列表
	if(this->clientAddrMaps != nullptr){
		this->clientAddrMaps->clear();
		delete this->clientAddrMaps;
		this->clientAddrMaps = nullptr;
	}
	//WSACleanup(); //清理winsock 运行环境
    
}

//初始化Server，包括创建sockect，绑定到IP和PORT
int Server::ServerStartup(){
	//创建 TCP socket
	this->srvSocket = socket(AF_INET,SOCK_STREAM,0);
	if(this->srvSocket == INVALID_SOCKET){
		cout << "Server socket create error !\n";
		return -1;
	}
	cout<< "Server socket create ok!\n";
	//设置服务器IP地址和端口号
	this->srvAddr.sin_family = AF_INET;
	this->srvAddr.sin_port =  htons(Config::PORT);
	this->srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);//会自动找到服务器合适的IP地址
//	this->srvAddr.sin_addr.S_un.S_addr = inet_addr(Config::SERVERADDRESS.c_str()); //这是另外一种设置IP地址的方法
	//绑定 socket to Server's IP and port
	if((bind(this->srvSocket,(struct sockaddr*)&(this->srvAddr),sizeof(this->srvAddr)))==-1){
        std::cout<<"Server Bind Error"<<std::endl;
        close(this->srvSocket);
        return -1;
    }
    std::cout<<"Server Socket Bind Established"<<std::endl;
	return 0;
}
//开始监听,等待客户的连接请求
int Server::ListenStartup(){
	int rtn = listen(this->srvSocket,Config::MAXCONNECTION);
	if(rtn == SOCKET_ERROR){
		cout << "Server socket listen error!\n";
		close(this->srvSocket);
		return -1;
	}
	cout<< "Server socket listen Established!\n";
	return 0;
}
//将收到的客户端消息保存到消息队列
void Server::AddRecvMessage(const string &str){
	if(!str.empty())
		this->rcvedMessages->insert(this->rcvedMessages->end(),str);
}
//将新的会话SOCKET加入队列
void Server::AddSession(SOCKET session){
	if(session != INVALID_SOCKET){
		this->sessions->insert(this->sessions->end(),session);
	}
}

//将失效的会话SOCKET加入队列
void Server::AddClosedSession(SOCKET session){
	if(session != INVALID_SOCKET){
		this->closedSessions->insert(this->closedSessions->end(),session);
	}
}
//将失效的SOCKET从会话SOCKET队列删除
void Server::RemoveClosedSession(SOCKET closedSession){
	if(closedSession != INVALID_SOCKET){
		auto itor=find(this->sessions->begin(),this->sessions->end(),closedSession);
		if(itor != this->sessions->end()) 
			this->sessions->erase(itor);
	}
}
//将失效的SOCKET从会话SOCKET队列删除
void Server::RemoveClosedSession(){
	for(auto itor = this->closedSessions->begin();itor != this->closedSessions->end();itor++){
		this->RemoveClosedSession(*itor);
	}
}
//从SOCKET s接受消息
void Server::recvMessage(SOCKET socket){
	int receivedBytes = recv(socket,this->recvBuf,Config::BUFFERLENGTH,0);
	if(receivedBytes == SOCKET_ERROR || receivedBytes == 0){//接受数据错误，把产生错误的会话socekt加入sessionsClosed队列
		this->AddClosedSession(socket);
		string s(this->GetClientAddress(this->clientAddrMaps,socket)+"disconnect");
		//没用的消息不用加进去
		//this->AddRecvMessage(s);
		cout << s <<endl;
	}
	else{
		recvBuf[receivedBytes] = '\0';
		//string s("来自" +  this->GetClientAddress(this->clientAddrMaps,socket) + "的游客说:\n" + recvBuf + "\n");
		string s(recvBuf);
		this->AddRecvMessage(s); //将收到的消息加入到消息队列
		cout << s<<endl;
		memset(this->recvBuf,'\0',Config::BUFFERLENGTH);//清除接受缓冲区
	}
}

void Server::sendHtml(fileHandler &fh, SOCKET socket) {
    if(!fh.htmlTransfer()){//没有找到
        this->notFound();
    }
    else
        this->sendOneMsg(socket,fh.sendMsg, fh.sendMsg.length());
}

void Server::sendImage(fileHandler &fh, SOCKET socket, string &extName) {
    fstream fs(fh.filename.c_str(), ios::in|ios::binary);
    if(!fs.is_open())//找不到filename
        this->notFound();
    else{
        string conType = fileHandler::respondFileType.at(extName);//search the correspond value of key"extName"
        string head="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
        string conLen;
        //TODO SB错误,content-type这一行下面要再接一个空行后才是数据段
        //string conType="Content-Type: image/jpeg\r\n\r\n";
        char buffer[SCALE];
        long size;
        long cur,last;
        int rtn;
        memset(buffer,'\0',SCALE*sizeof(char));
        fs.seekg(0,ios::end);
        size=fs.tellg();
        conLen=to_string(size);
        fs.seekg(0, ios::beg);
        head=head+"Content-Length: "+conLen+"\r\n"+conType;
        this->sendOneMsg(socket,head,head.size());
        while (!fs.eof()){
            last=fs.tellg();
            fs.read(buffer,SCALE);
            cur=fs.tellg();
            send(socket,buffer,cur-last,0);
            memset(buffer,'\0',SCALE*sizeof(char));
        }
    }
}

void Server::notFound() {//error code 404

}

void Server::notSupported() {//error code 400

}

//向SOCKET s发送消息
void Server::sendMessage(SOCKET socket,string &msg){
	fileHandler fh(msg);
	string extName=fh.getFileExtensionName();
	if(fileHandler::respondFileType.find(extName)==fileHandler::respondFileType.end())//不支持的后缀名
        this->notSupported();
	else if(extName==".html"){
	    //int length=fh.htmlTransfer();
	    //string msgS=fh.getSendMsg();
	    //sendOneMsg(socket,msgS,length);
        this->sendHtml(fh, socket);
	}
	else if(extName==".jpg" || extName=="png" || extName=="gif"){
	    this->sendImage(fh,socket, extName);
        /*
        string conType = fileHandler::respondFileType.at(extName);
        string head="HTTP/1.1 200 ok\r\nConnection: keep-alive\r\n";
        string conLen;
		//TODO SB错误,content-type这一行下面要再接一个空行后才是数据段
        //string conType="Content-Type: image/jpeg\r\n\r\n";
        char buffer[SCALE];
        long size;
        long cur,last;
        int rtn;
        memset(buffer,'\0',SCALE*sizeof(char));
	    fstream fs(fh.filename.c_str(), ios::in|ios::binary);
        fs.seekg(0,ios::end);
        size=fs.tellg();
        conLen=to_string(size);
        fs.seekg(0, ios::beg);
        head=head+"Content-Length: "+conLen+"\r\n"+conType;
        this->sendOneMsg(socket,head,head.size());
        while (!fs.eof()){
            last=fs.tellg();
            fs.read(buffer,SCALE);
            cur=fs.tellg();
            send(socket,buffer,cur-last,0);
            memset(buffer,'\0',SCALE*sizeof(char));
        }
         */
        //memset(buffer,'\0',SCALE);
	}
}

//send one paragraph message
void Server::sendOneMsg(SOCKET socket, string &fh, int length) {
    int rtn = send(socket,fh.c_str(),length,0);
    //int rtn=1;
    if(rtn == SOCKET_ERROR){//发送数据错误，把产生错误的会话socekt加入sessionsClosed队列
//		cout << "Send to client failed!" << endl;
//		cout << "A client leaves..." << endl;
        //string s("来自" + this->GetClientAddress(this->clientAddrMaps,socket) + "的游客离开了聊天室,我们深深地凝望着他(她)的背影...\n");
        string s(this->GetClientAddress(this->clientAddrMaps,socket)+"disconnect");
        //this->AddRecvMessage(s);
        this->AddClosedSession(socket);
        cout << s<<endl;
    }
}

//向其他客户转发信息
void Server::ForwardMessage(){
	if(this->numOfSocketSignaled > 0){
		if(!this->rcvedMessages->empty()){//如果消息队列不为空
			for(auto msgItor = this->rcvedMessages->begin();msgItor != this->rcvedMessages->end();msgItor++){//对消息队列中的每一条消息
				for(auto sockItor = this->sessions->begin();sockItor != this->sessions->end();sockItor++){//对会话socket队列中的每个socket
					if( FD_ISSET(*sockItor,&this->wfds)){ 
						this->sendMessage(*sockItor,*msgItor);
					}
				}
			}
		}
		this->rcvedMessages->clear(); //向其他客户转发消息后，清除消息队列
	}
}
//get the client socket&IP then restore 
int Server::AcceptRequestionFromClient(){
	sockaddr_in clientAddr;		//客户端IP地址
	socklen_t nAddrLen = sizeof(clientAddr);
	u_long blockMode = Config::BLOCKMODE;//将session socket设为非阻塞模式以监听客户连接请求
	//检查srvSocket是否收到用户连接请求
	if(this->numOfSocketSignaled > 0){
		if(FD_ISSET(this->srvSocket,&rfds)){  //有客户连接请求到来
			this->numOfSocketSignaled--;
			//产生会话sockets
			SOCKET newSession = accept(this->srvSocket,(sockaddr*)&(clientAddr),&nAddrLen);
			if(newSession == INVALID_SOCKET ){
				cout << "Server accept connection request error!\n";
				return -1;
			}
//			cout << "New client connection request arrived and new session created\n";
			//将新的会话socket设为非阻塞模式，
			if(ioctl(newSession,FIONBIO,&blockMode) == SOCKET_ERROR){
				cout << "ioctlsocket() for new session failed with error!\n";
				return -1;
			}
			//将新的session加入会话队列
			this->AddSession(newSession);
			this->clientAddrMaps->insert(unordered_map<SOCKET,string>::value_type(newSession,this->GetClientAddress(newSession)));//保存地址
			string s("connection from " + this->GetClientAddress(this->clientAddrMaps,newSession)  + "\n");
			//string s(this->GetClientAddress(this->clientAddrMaps,newSession));
			//this->AddRecvMessage(s);
			cout << s<<endl;
		}
	}
	return 0;
}
void Server::ReceieveMessageFromClients(){
	if(this->numOfSocketSignaled > 0){
		//遍历会话列表中的所有socket，检查是否有数据到来
		for(list<SOCKET>::iterator itor = this->sessions->begin();itor!=this->sessions->end();itor++){
			if(FD_ISSET(*itor,&rfds)){  //某会话socket有数据到来
				//接受数据
				this->recvMessage(*itor);
			}
		}//end for
	}
}
//得到客户端IP地址
string  Server::GetClientAddress(SOCKET s){
	string clientAddress; //clientAddress是个空字符串， clientAddress.empty() is true
	sockaddr_in clientAddr;
	int rtn;
	socklen_t nameLen = sizeof(clientAddr);
	rtn = getsockname(s,(sockaddr*)&clientAddr,&nameLen);
	if(rtn != SOCKET_ERROR){
		clientAddress += inet_ntoa(clientAddr.sin_addr);
	}
	return clientAddress; 
}
//得到客户端IP地址
string  Server::GetClientAddress(unordered_map<SOCKET,string> *maps,SOCKET s){
	auto itor = maps->find(s);
	if( itor != maps->end())
		return (*itor).second;
	else{
		return string("");
	}
}
//接受客户端发来的请求和数据并转发
int Server::Loop(){
	u_long blockMode = Config::BLOCKMODE;//将srvSock设为非阻塞模式以监听客户连接请求
	int rtn;
	SOCKET maxSocket=this->srvSocket;
	if( (rtn = ioctl(this->srvSocket,FIONBIO,&blockMode) == SOCKET_ERROR)){ //FIONBIO：允许或禁止套接口s的非阻塞模式。
		cout << "ioctlsocket() failed with error!\n";
		return -1;
	}
	cout<< "ioctlsocket() for server socket ok!Waiting for client connection and data\n";
	while(true){ //等待客户的连接请求
		//首先从会话SOCKET队列中删除掉已经关闭的会话socket
		this->RemoveClosedSession();
		//Prepare the read and write socket sets for network I/O notification.
		FD_ZERO(&this->rfds);
		FD_ZERO(&this->wfds);
		//把srvSocket加入到rfds，等待用户连接请求
		FD_SET(this->srvSocket,&this->rfds);
		//把当前的会话socket加入到rfds,等待用户数据的到来;加到wfds，等待socket可发送数据
		for(auto itor = this->sessions->begin();itor!=this->sessions->end();itor++){
			maxSocket=max(maxSocket, *itor);
			FD_SET(*itor,&rfds);
			FD_SET(*itor,&wfds);
		}
		//等待用户连接请求或用户数据到来或会话socket可发送数据
		if((this->numOfSocketSignaled = select(maxSocket+1,&this->rfds,&this->wfds,NULL,NULL)) == SOCKET_ERROR){ //select函数返回有可读或可写的socket的总数，保存在rtn里.最后一个参数设定等待时间，如为NULL则为阻塞模式
			cout << "select() failed with error!\n";
			return -1;
		}
		//当程序运行到这里，意味着有用户连接请求到来，或有用户数据到来，或有会话socket可以发送数据
		//首先检查是否有客户请求连接到来
		if(this->AcceptRequestionFromClient() != 0) return -1;	
		//然后向客户端发送数据
		this->ForwardMessage();
		//最后接受客户端发来的数据
		this->ReceieveMessageFromClients();
	}	
	return 0;
}
