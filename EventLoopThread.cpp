/*************************************************************************
	> File Name: EventLoopThread.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/
#include <pthread.h>

#include "EventLoopThread.h"


EventLoopThread::EventLoopThread(int i)
{
	pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&cond_, NULL);
    eventLoop_ = NULL;
    threadCount_ = 0;
    threadTid_ = 0;

    char *buf = new char[16];
    sprintf(buf, "Thread-%d\0", i + 1);
    threadName_ = buf;
    //threadFunc_ = std::bind(threadFunc,this);
}

EventLoopThread::~EventLoopThread()
{
}


void * threadFunc(void * ptr)
{

    EventLoopThread * myPtr = (EventLoopThread *)ptr;
    pthread_mutex_lock(&myPtr->mutex_);
    //初始化 eventloop
    myPtr->eventLoop_ = new EventLoop(myPtr->threadName_);  
    //通知停止等待
    pthread_cond_signal(&myPtr->cond_);
    pthread_mutex_unlock(&myPtr->mutex_);
    
    //循环调用eventloop.diapatcher;
    myPtr->eventLoop_->run();
    // while(1)
    // {
    //     myPtr->eventLoop_->eventDispatcher_->epoll_dispatch();

    //     //处理pendingChannel?

    // }
    return nullptr;
}

EventLoop* EventLoopThread::startLoop()
{
    pthread_mutex_lock(&mutex_);
    pthread_create(&threadTid_,nullptr,threadFunc,this); 
    pthread_cond_wait(&cond_, &mutex_);
    pthread_mutex_unlock(&mutex_);


}


