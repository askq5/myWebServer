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
	//条件等待 主线程在startloop中等待知道子线程初始化完自己的enentloop
	//之后子线程进入循环， 主线程继续创建新的子线程到指定数目
	pthread_mutex_t mutex_;
    pthread_cond_t cond_;
public:
	EventLoopThread(int i);
	~EventLoopThread();

	EventLoop * eventLoop_;
    pthread_t threadTid_;        /* thread ID */
    std::string threadName_;
	//ThreadFunc threadFunc_;
	
	EventLoop* startLoop();
	// threadFunc();

	friend void * threadFunc(void *);
};




#endif
