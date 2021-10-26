/*************************************************************************
	> File Name: WorkThreadsPool.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 07:58:31 PM CST
 ************************************************************************/

#ifndef _WORKTHREADsPOOl_H
#define _WORKTHREADsPOOl_H
#include <thread>
#include <pthread.h>

//#include "TcpServer.h"
#include "TcpConnection.h"
#include "Thread.h"

class WorkThreadsPool
{
private:
	typedef std::function<void *()> ThreadFunc;
    int threadsNum_;
	ThreadFunc threadFunc_;
	/* data */
public:
	WorkThreadsPool(int threadsNumber,ThreadFunc threadFunc);
	~WorkThreadsPool();
	int start();
	//void * worker(void *);
	//pthread_t * workThreads_;
    std::thread::id * workThreads_;
};



#endif
