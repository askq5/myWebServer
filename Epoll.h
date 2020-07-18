/*************************************************************************
	> File Name: Epoll.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/

#ifndef _EPOLL_H
#define _EPOLL_H

#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

#include "Channel.h"

#define MAXEVENTS 128

typedef int (*connection_completed_call_back)(struct tcp_connection *tcpConnection);

typedef int (*message_call_back)(struct buffer *buffer, struct tcp_connection *tcpConnection);

typedef int (*write_completed_call_back)(struct tcp_connection *tcpConnection);

typedef int (*connection_closed_call_back)(struct tcp_connection *tcpConnection);

#define OP_DEL    0x01
/**  */
#define OP_ADD        0x02
/**  */
#define OP_MOD    0x04
/** */


class Epoll
{
private:
	/* data */
	int epollAdd(int fd, epoll_event epollEvent);

	int epollDel(int fd, epoll_event epollEvent);

	int epollMod(int fd, epoll_event epollEvent);
public:
	Epoll(/* args */);
	~Epoll();

	//static const int MAXFDS = 100000;
	//Channel fd2chan_[MAXFDS]
    //int event_count_;
    int nfds_;
    int epollFd_;
    struct epoll_event * events_;

	int epollCtl(Channel * channel);
	//int epoll_add(struct event_loop *, Channel & channel1);

	int epollDispatch();

};




#endif
