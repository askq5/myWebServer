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
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
	bool started_;
 	int numThreads_;
    int next_;
    long thread_count;    /* # connections handled */

	void start();

    EventLoop* getNextLoop();
};




#endif
