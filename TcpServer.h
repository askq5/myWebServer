/*************************************************************************
	> File Name: TcpServer.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:18 PM CST
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include <functional>
#include <string>

#include "Channel.h"
#include "EventLoopThreadPool.h"


//template <typename T>
class TcpServer
{
private:
	/* data */
	
	typedef std::function<int (void *)> TcpServerCallBack;
    TcpServerCallBack connectionCompletedCallBack_;
    TcpServerCallBack messageCallBack_;
    TcpServerCallBack writeCompletedCallBack_;
    TcpServerCallBack connectionClosedCallBack_;
    

	void make_nonblocking(int fd);
	int listenNoblock(int port);
	int handleConnectioneEstablished();
	//设置callback数据
	//void setData(void * data);
public:
	TcpServer(int port, int threadNum);
	~TcpServer();

	
	void setConnectionCompleted(TcpServerCallBack && connectionCompletedCallBack)
	{ connectionCompletedCallBack_ = connectionCompletedCallBack; }
	void setMessage(TcpServerCallBack && messageCallBack)
	{ messageCallBack_ = messageCallBack; }
	void setWriteCompleted(TcpServerCallBack && writeCompletedCallBack)
	{ writeCompletedCallBack_ = writeCompletedCallBack; }
	void setConnectionClosed(TcpServerCallBack && connectionClosedCallBack)
	{ connectionClosedCallBack_ = connectionClosedCallBack; }
	TcpServerCallBack getConnetionCompleted()
	{ return connectionClosedCallBack_; }
	TcpServerCallBack getMessageCallBack()
	{ return messageCallBack_; }
	TcpServerCallBack getWriteCompletedCallBack()
	{ return writeCompletedCallBack_; }
	TcpServerCallBack getConnectionClosedCallBack()
	{ return connectionClosedCallBack_; }
	int connectionCompleted(void * ptr) { return connectionClosedCallBack_(ptr); }
	int message(void * ptr) { return messageCallBack_(ptr); }
	int writeCompleted(void * ptr) { return writeCompletedCallBack_(ptr); }
	int connectionClosed(void * ptr) { return connectionClosedCallBack_(ptr); }

	int port_;
	Channel *acceptor_;
    int threadNum_;
    EventLoopThreadPool *eventLoopThreadPool_;

	void start();

};




#endif
