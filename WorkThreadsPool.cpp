/*************************************************************************
	> File Name: WorkThreadsPoll.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 07:58:31 PM CST
 ************************************************************************/
#include "WorkThreadsPool.h"

WorkThreadsPool::WorkThreadsPool(int threadsNum,ThreadFunc threadFunc)
				:threadsNum_(threadsNum),
				threadFunc_(threadFunc)


{
	workThreads_ = new std::thread::id[threadsNum_];

}

WorkThreadsPool::~WorkThreadsPool()
{
	delete [] workThreads_;
}

int WorkThreadsPool::start()
{
	for(int i = 0; i  < threadsNum_; i++)
	{
		//Thread * thread = new Thread(threadFunc_,"worker");
        //thread->start();
		std::thread * t = new std::thread(threadFunc_);
        *(workThreads_+i) = t->get_id();
	}
    return 0;
}


