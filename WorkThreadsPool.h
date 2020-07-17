/*************************************************************************
	> File Name: WorkThreadsPool.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 07:58:31 PM CST
 ************************************************************************/

#ifndef _WORKTHREADsPOOl_H
#define _WORKTHREADsPOOl_H

#include <pthread.h>

//#include "TcpServer.h"
#include "TcpConnection.h"
#include "Thread.h"

class WorkThreadsPool
{
private:
	typedef std::function<void *()> ThreadFunc;
	ThreadFunc threadFunc_;
	/* data */
public:
	WorkThreadsPool(int threadsNumber,ThreadFunc threadFunc);
	~WorkThreadsPool();
	int start();
	//void * worker(void *);
	int threadsNum_;
	pthread_t * workThreads_;

	//TcpServer * tcpServer_;
};

WorkThreadsPool::WorkThreadsPool(int threadsNum,ThreadFunc threadFunc)
				:threadsNum_(threadsNum),
				threadFunc_(threadFunc)


{
	workThreads_ = new pthread_t[threadsNum_];

}

WorkThreadsPool::~WorkThreadsPool()
{
	delete [] workThreads_;
}

int WorkThreadsPool::start()
{
	for(int i = 0; i  < threadsNum_; i++)
	{
		Thread thread(threadFunc_,"worker");
		*(workThreads_+i) = thread.tid();
	}

}


#endif
