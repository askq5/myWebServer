/*************************************************************************
	> File Name: Channel.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:55 PM CST
 ************************************************************************/

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <functional>



#define EVENT_TIMEOUT    0x01
/** Wait for a socket or FD to become readable */
#define EVENT_READ        0x02
/** Wait for a socket or FD to become writeable */
#define EVENT_WRITE    0x04
/** Wait for a POSIX signal to be raised*/
#define EVENT_SIGNAL    0x08

class Channel {
private:
	typedef std::function<int()> CallBack;
	int fd_;
	int fdState_;
	int op_;
	__uint32_t fdEvent_;

	CallBack  readHandler_;
  	CallBack  writeHandler_;
 	CallBack  errorHandler_;
 	CallBack  connHandler_;
public:
	Channel(int fd, int eventState);
	~Channel();

    int getOp() { return op_; }
	void setOp(int op) { op_ = op; }
	int getFd() { return fd_; }
  	void setFd(int fd){ fd_ = fd; }
	void setEvents(__uint32_t ev) { fdEvent_ = ev; }
	__uint32_t &getEvents() { return fdEvent_; }
	void setReadHandler(CallBack && readHandler)
	{ readHandler_ = readHandler; }
  	void setWriteHandler(CallBack && writeHandler) 
	{ writeHandler_ = writeHandler; }
  	void setErrorHandler(CallBack &errorHandler) 
	{ errorHandler_ = errorHandler; }
  	void setConnHandler(CallBack &connHandler) 
	{ connHandler_ = connHandler; }

	void handleRead();
  	void handleWrite();
  	void handleError(int fd, int err_num, std::string short_msg);
  	void handleConn();


	int isWriteEventEnabled();

	void writeEventEnable();

	void writeEventDisable();

private:
};


#endif
