/*************************************************************************
	> File Name: TcpConnection.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:18 PM CST
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H
#include <string>

#include "EventLoop.h"
#include "Channel.h"
#include "TcpServer.h"
//继承channel
class TcpConnection
{
private:
	/* data */
    std::string threadName_;
   
	TcpServer * tcpServer_;

	
	int handleConnectionClosed();

	int handleRead();

	//发送缓冲区可以往外写
	//把channel对应的output_buffer不断往外发送
	int handleWrite();
	//int sendData(void *data, int size);
public:
	TcpConnection(int fd, EventLoop * eventloop, TcpServer * ptr);
	~TcpConnection();
    Channel * channel_;
	EventLoop * eventLoop_;

    std::string outBuffer_;  //发送缓冲区
 	std::string inBuffer_;   //接收缓冲区
	std::string getThreadName() { return threadName_; }
	void setTcpServer(TcpServer * tcpServer) { tcpServer_ = tcpServer; }
	//应用层调用入口
	//int SendBuffer(string buffer);

	void shutDown();
};





#endif
