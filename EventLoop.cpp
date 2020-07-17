/*************************************************************************
	> File Name: EventLoop.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <functional>
#include <unistd.h>

#include "EventLoop.h"
#include "Utils.h"

int createEventfd() {
  int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    
  }
  return evtfd;
}

EventLoop::EventLoop(std::string threadName)

{
	if (!threadName.empty()) {
        threadName_ = threadName;
    } else {
        threadName_ = "main thread";
    }

    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&cond_, NULL);

    quit_ = 0;//keep going
	epoller_ = new Epoll();
    
    ownerThreadId_ = pthread_self();
	//add the wakefd_ to event
    wakeupFd_ = createEventfd();
    
    Channel * pWakeupChannel = new Channel(wakeupFd_, OP_ADD);
	pWakeupChannel->setEvents(EPOLLIN | EPOLLET);
	//pWakeChannel->setOp(OP_ADD);
	//pWakeChannel->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
	pWakeupChannel->setReadHandler(std::bind(&EventLoop::handleWakeup,this));
	channelMap_[wakeupFd_] = pWakeupChannel;

	//注册到epoller_里的epollFd

	epoller_->epollCtl(pWakeupChannel);
    
}

EventLoop::~EventLoop()
{
	delete epoller_;
	delete channelMap_;
}

int EventLoop::run(/*struct event_loop *eventLoop*/)
{
	while(!quit_)
	{
		int evenntsCount = epoller_->epollDispatch();
		//发任务
		for(int i = 0; i < evenntsCount; i++)
		{
			int fd = epoller_->events_[i].data.fd;
			struct epoll_event fdEvents = epoller_->events_[i];

			if(fdEvents.events & EPOLLERR || fdEvents.events & EPOLLHUP)
			{
				perror("event error\n");
				close(fdEvents.data.fd);
				continue;
			}

			if(fdEvents.events & EPOLLIN)
			{

				channelMap_[fd]->handleRead();
			}

			if(fdEvents.events & EPOLLOUT)
			{

				channelMap_[fd]->handleWrite();
			}

		}

		handlePendingChannel();
	}

}

void EventLoop::wakeup()
{	
	uint64_t one = 1;
  	ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof(one));
  	if (n != sizeof one) 
	{
    //LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  	}

}

int EventLoop::handleWakeup() {
    char one;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        //LOG_ERR("handleWakeup  failed");
    }
    //yolanda_msgx("wakeup, %s", eventLoop->thread_name);
	return 0;
}

int EventLoop::doChannelEvent(Channel * channel)
{
	//在这里判断线程id（phtread_self）是否与eventloop绑定的一样
	if(pthread_self() != ownerThreadId_)
	{
		pthread_mutex_lock(&mutex_);
		isHandlePending_ = 0;
		pendingChannel_.push(channel);
		pthread_mutex_unlock(&mutex_);
		wakeup();
	}
	else
	{
		epoller_->epollCtl(channel);
		//handlePendingChannel();
	}
}

int EventLoop::handlePendingChannel()
{
	pthread_mutex_lock(&mutex_);
	isHandlePending_ = 1;
	while(!pendingChannel_.empty())
	{
		Channel * channel = pendingChannel_.front();
		pendingChannel_.pop();	
		epoller_->epollCtl(channel);
	}
	pthread_mutex_unlock(&mutex_);
	return 0;
}