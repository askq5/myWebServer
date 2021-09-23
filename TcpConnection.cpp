/*************************************************************************
	> File Name: TcpConnection.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:19 PM CST
 ************************************************************************/
#include <functional>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include "TcpConnection.h"
#include "Utils.h"
#include "TcpServer.h"

#define MAXSIZE 4096
#define BUFSIZE 1024

TcpConnection::TcpConnection(int fd, EventLoop * eventLoop, TcpServer * ptr)
{
    tcpServer_ = ptr;
	channel_ = new Channel(fd,OP_ADD);
	channel_->setEvents(EPOLLIN | EPOLLET);
    
	channel_->setReadHandler(std::bind(&TcpConnection::handleRead,this));
	channel_->setWriteHandler(std::bind(&TcpConnection::handleWrite,this));
	
	eventLoop_ = eventLoop;
    eventLoop_->channelMap_[fd] = channel_;
	//name_ ?
    
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
    eventLoop_->channelMap_[channel_->getFd()] = nullptr;
	channel_->setOp(OP_DEL);
    eventLoop_->channelOpEvent(channel_);
    // if (tcpServer_->getConnectionClosedCallBack() != nullptr) {
    //     tcpServer_->getConnectionClosedCallBack()(nullptr,nullptr);
    // }
    return 0;
}

int TcpConnection::handleRead() {
    ssize_t readLen = 0, len = 1;
    char buff[MAXSIZE];
    while(len)
    {
        len = read(channel_->getFd(),buff+readLen, BUFSIZE);
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
        //先判断再用
        readLen += len;
    }

    if(readLen == 0)
    {
        //client调用了shutdown | close
        //总共读到0个字符？
        handleConnectionClosed();
        return -1;
    }
    inBuffer_.clear();
    inBuffer_ += std::string(buff, buff + readLen);
    //appendRequest(this);
    pthread_mutex_lock(&tcpServer_->workLocker_);
    if(tcpServer_->workQueue_.size() >= tcpServer_->maxRequests_)
    {
        pthread_mutex_unlock(&tcpServer_->workLocker_);
        //太多连接了
        return -1;
    }
    tcpServer_->workQueue_.push(this);
    pthread_mutex_unlock(&tcpServer_->workLocker_);
    sem_post(&tcpServer_->sem_);
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
    
    size_t writedLen = 0, len = 0, bufLen = outBuffer_.size();
    char * ptr = const_cast<char *>(outBuffer_.c_str());
    while(writedLen < bufLen)
    {
        len = write(channel_->getFd(), ptr+writedLen,bufLen-writedLen);
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