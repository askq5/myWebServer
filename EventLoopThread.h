/*************************************************************************
	> File Name: EventLoopThread.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/

#ifndef _EVENTLOOPTHREAD_H
#define _EVENTLOOPTHREAD_H

#include <string>

#include "EventLoop.h"

//typedef std::function<void()> ThreadFunc;
class EventLoopThread
{
private:
	/* data */
public:
	EventLoopThread(int i);
	~EventLoopThread();

	EventLoop * eventLoop_;
    pthread_t threadTid_;        /* thread ID */
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    std::string threadName_;
    long threadCount_;    /* # connections handled */
	//ThreadFunc threadFunc_;
	
	EventLoop* startLoop();
	// threadFunc();

	friend void * threadFunc(void *);
};




#endif
