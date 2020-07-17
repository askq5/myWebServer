/*************************************************************************
	> File Name: EpollLoopThreadPool.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:18:02 PM CST
 ************************************************************************/

#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(int numThreads)
:started_(false), numThreads_(numThreads), next_(0)
{
  baseEventLoop_  = new EventLoop(nullptr);

  if (numThreads_ <= 0)
  {
      
    
  }
  

}

void EventLoopThreadPool::start() {
    //主reactor 上要初始化监听套接字  并且要add acceptor（channel）
    /*
    socket();
    setsocketopt();
    bind();
    listen();
    baseLoop_->
    */

    started_ = true;
    for (int i = 0; i < numThreads_; ++i) 
    {
    EventLoopThread * t = new EventLoopThread(i);
    threads_.push_back(t);
    
    //新建从reactor 咋和 eventloopthread 绑定
    //一旦返回就代表eventlopthread中的eventloop已经初始化完毕 进入epoll_wait
    t->startLoop();
    loops_.push_back(t->eventLoop_);
    
    }

    //在主reactor上注册监听套接字
    //开启主reactor
}

EventLoop *EventLoopThreadPool::getNextLoop() {
  //baseLoop_->assertInLoopThread();
  //assert(started_);
  if(baseEventLoop_->getTid() != pthread_self())
  {
    //
  }
  EventLoop *loop = nullptr;
  if (!loops_.empty()) {
    loop = loops_[next_];
    next_ = (next_ + 1) % numThreads_;
  }
  return loop;
}