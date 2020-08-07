/*************************************************************************
	> File Name: Epoll.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/
#include <sys/epoll.h>
#include <unistd.h>
#include "Epoll.h"

Epoll::Epoll(/* args */)
{
	//event_count = 0;
	nfds_ = 0;
	//realloc_copy = 0;
	epollFd_ = epoll_create1(0);
	if(epollFd_ == -1)
	{
		/* error */
	}
    events_ = new epoll_event[MAXEVENTS];
    
}

Epoll::~Epoll()
{
}

int Epoll::epollCtl(Channel * channel)
{
	struct epoll_event epollEvent;
	epollEvent.events =  channel->getEvents();
	epollEvent.data.fd = channel->getFd();
	if(channel->getOp() == OP_ADD)
	{
		epollAdd(channel->getFd(), epollEvent);
	}
	else if(channel->getOp() == OP_DEL)
	{
		epollDel(channel->getFd(), epollEvent);
	}
	else if(channel->getOp() == OP_MOD)
	{
		epollMod(channel->getFd(), epollEvent);
	}
	return 0;
}

int Epoll::epollAdd(int fd, epoll_event epollEvent)
{
	//fd 从哪来
	//event ？

	if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &epollEvent) < 0) 
    	perror("epoll_add error");
    
	return 0;
}

int Epoll::epollDel(int fd, epoll_event epollEvent)
{

	if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &epollEvent))
		perror("epoll_del error");

	return 0;
}

int Epoll::epollMod(int fd, epoll_event epollEvent)
{

	if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &epollEvent))
		perror("epoll_mod error");


	return 0;
}

int Epoll::epollDispatch()
{

	int eventsCount = epoll_wait(epollFd_, events_,MAXEVENTS, 500);
    if(eventsCount < 0)
    {
        perror("epoll_wait()");
    }
	/*
	for(int i = 0; i < event_count; i++)
	{
		if(events_[i].events & EPOLLERR || events_[i].events & EPOLLHUP)
		{
			perror("epoll error\n");
			close(events_[i].data.fd);
			continue;
		}

		if(events_[i].events & EPOLLIN)
		{

			//events_[i]
		}

		if(events_[i].events & EPOLLOUT)
		{


		}

	}
	*/
	
	return eventsCount;
}