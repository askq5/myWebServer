/*************************************************************************
	> File Name: EventLoopThreadPool.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:18:02 PM CST
 ************************************************************************/

#ifndef _EVENTLOOPTHREADPOOL_H
#define _EVENTLOOPTHREADPOOL_H

#include <vector>

#include "EventLoop.h"
#include "EventLoopThread.h"

class EventLoopThreadPool
{
private:
	/* data */
public:
	EventLoopThreadPool(int numThreads);
	~EventLoopThreadPool();
	
	EventLoop * baseEventLoop_;
	std::vector<EventLoop *> loops_;
	std::vector<EventLoopThread *> threads_;
	//?
	bool started_;
 	int threadsNum_;
	//主vector用来分配任务用的
    int next_;

	void start();
    EventLoop* getNextLoop();
};




#endif
