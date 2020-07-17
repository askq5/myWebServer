/*************************************************************************
	> File Name: TcpConnection.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:19 PM CST
 ************************************************************************/
#include <functional>

#include "TcpConnection.h"
#include "Utils.h"
#include "sys/socket.h"

TcpConnection::TcpConnection(int fd, EventLoop * eventLoop)
{
	channel_ = new Channel(fd,EVENT_READ);
	channel_->setEvents(EPOLLIN | EPOLLET);
	channel_->setReadHandler(std::bind(&TcpConnection::handleRead,this));
	channel_->setWriteHandler(std::bind(&TcpConnection::handleWrite,this));
	
	eventLoop_ = eventLoop;
	//name_ ?
	
	eventLoop_->doChannelEvent(channel_);
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
    eventLoop_->doChannelEvent(channel_);
    if (tcpServer_->getConnectionClosedCallBack() != nullptr) {
        tcpServer_->getConnectionClosedCallBack()(nullptr);
    }
}

int TcpConnection::handleRead() {

    if (read(inBuffer_, channel_->getFd()) > 0) {
        //应用程序真正读取Buffer里的数据
        if (tcpServer_->getMessageCallBack() != nullptr) {
            tcpServer_->getMessageCallBack()(&inBuffer_);
        }
    } else {
        handleConnectionClosed();
    }
}

//发送缓冲区可以往外写
//把channel对应的output_buffer不断往外发送
int TcpConnection::handleWrite() {
    
    assertInSameThread(eventLoop_);

    ssize_t nwrited = write(channel_->getFd(), outBuffer_,
                            outBuffer_.size());
    if (nwrited > 0) {
        //已读nwrited字节
        outBuffer_ += nwrited;
        //如果数据完全发送出去，就不需要继续了
        if (outBuffer_.size() == 0) {
            channel_->writeEventDisable();
        }
        //回调writeCompletedCallBack
        if (tcpServer_->getWriteCompletedCallBack() != NULL) {
            tcpServer_->getWriteCompletedCallBack()(nullptr);
        }
    } else {
        //yolanda_msgx("handle_write for tcp connection %s", tcpConnection->name);
    }

}