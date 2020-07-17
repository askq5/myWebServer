/*************************************************************************
	> File Name: Channnel.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:55 PM CST
 ************************************************************************/

#include "Channel.h"

Channel::Channel(int fd, int op)
				:fd_(fd),
				op_(op)
{	

}

Channel::~Channel()
{

}

int Channel::isWriteEventEnabled()
{
	return this->fdState_ & EVENT_WRITE;
}

void Channel::writeEventEnable()
{
	//data数据类型要不要设置成 EventLoop
	this->fdState_ = this->fdState_ | EVENT_WRITE;
	//event_loop_update_channel_event(eventLoop, channel->fd, channel);
}

void Channel::writeEventDisable()
{
	//data数据类型要不要设置成 EventLoop
	this->fdState_ = this->fdState_ & ~EVENT_WRITE;
	//event_loop_update_channel_event(eventLoop, channel->fd, channel);
}

void Channel::handleRead() {
  if (readHandler_) {
    readHandler_();
  }
}

void Channel::handleWrite() {
  if (writeHandler_) {
    writeHandler_();
  }
}

void Channel::handleConn() {
  if (connHandler_) {
    connHandler_();
  }
}


