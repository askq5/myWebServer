/*************************************************************************
	> File Name: TcpConnection.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:19 PM CST
 ************************************************************************/
#include <functional>
#include <unistd.h>
#include <errno.h>

#include "TcpConnection.h"
#include "Utils.h"
#include "sys/socket.h"

#define BUFSIZE 1024

TcpConnection::TcpConnection(int fd, EventLoop * eventLoop, TcpServer * ptr)
{
    tcpServer_ = ptr;
	channel_ = new Channel(fd,EVENT_READ);
	channel_->setEvents(EPOLLIN | EPOLLET);
    channel_->setOp(OP_ADD);
	channel_->setReadHandler(std::bind(&TcpConnection::handleRead,this));
	channel_->setWriteHandler(std::bind(&TcpConnection::handleWrite,this));
	
	eventLoop_ = eventLoop;
	//name_ ?
	eventLoop_->channelOpEvent(channel_);
    
}

TcpConnection::~TcpConnection()
{
	free(channel_);
	
}


void TcpConnection::shutDown() {
    if (shutdown(channel_->getFd(), SHUT_WR) < 0) {
        //yolanda_msgx("tcp_connection_shutdown failed, socket == %d", tcpConnection->channel->fd);
    }
}

int TcpConnection::handleConnectionClosed() {
	channel_->setOp(OP_DEL);
    eventLoop_->channelOpEvent(channel_);
    if (tcpServer_->getConnectionClosedCallBack() != nullptr) {
        tcpServer_->getConnectionClosedCallBack()(nullptr);
    }
}

int TcpConnection::handleRead() {
    ssize_t readLen = 0, len = 1;
    while(len)
    {
        len = read(channel_->getFd(),&inBuffer_+readLen, BUFSIZE);
        readLen += len;
        if(len < 0)
        {
            if(errno == EAGAIN || errno == EINTR)
                break;
            else
            {
                //读端出错，重新读？重新连接
                 handleConnectionClosed();
                 return -1;
            }
        }
    }
    if(readLen == 0)
    {
        //client调用了shutdown | close
        //总共读到0个字符？
        handleConnectionClosed();
        return -1;
    }

    tcpServer_->workQueue.push(this);
    return 0;
    // if(tcpServer_->getMessageCallBack() != nullptr) 
    // {
    //     //channel_->setOp(OP_MOD);
    //     //eventLoop_->channelOpEvent(channel_);
    //     tcpServer_->getMessageCallBack()(this);
    //     return 0;
    // }    
           
    
}

//fd发送缓冲区可以往外写
//把channel对应的output_buffer不断往外发送
int TcpConnection::handleWrite() {
    
    assertInSameThread(eventLoop_);
    ssize_t writedLen = 0, len = 0, bufLen = outBuffer_.size();
    while(writedLen < bufLen)
    {
        len = write(channel_->getFd(), &outBuffer_+writedLen,bufLen-writedLen);
        writedLen+=len;
        if(len < 0)
        {
            if(errno == EAGAIN || errno == EINTR)
                break;
            else
            {
                //写端出错，重新写？重新连接
                 handleConnectionClosed();
                 return -1;
            }
        }
    }
    //if(writeLen == bufLen)
    //?
    channel_->writeEventDisable();
    //MODE成监控fd的读事件
    channel_->setEvents(EPOLLIN | EPOLLET);
    channel_->setOp(OP_MOD);
    eventLoop_->channelOpEvent(channel_);
    //回调writeCompletedCallBack ?
    // if (tcpServer_->getWriteCompletedCallBack() != NULL) 
    //     tcpServer_->getWriteCompletedCallBack()(nullptr);
    //yolanda_msgx("handle_write for tcp connection %s", tcpConnection->name);
    return 0;

}