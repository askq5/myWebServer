/*************************************************************************
	> File Name: EventLoop.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Wed 15 Jul 2020 05:15:56 PM CST
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <unordered_map>
#include <map>
#include <string>
#include <queue>
#include <functional>

#include "Channel.h"
#include "Epoll.h"

class EventLoop
{
private:
	/* data */
    //1:quit 0:continue
    int quit_;
    //等待事件发生
    Epoll * epoller_;
    //状态 正在处理待处理队列
    int isHandlePending_;
    //待处理的循环队列  链表？
    //会有主reactor 和 某个从reactor 竞争
    std::queue<Channel *> pendingChannel_;
    //竞争解决
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    //新连接来临时通信
    int wakeupFd_;
    //和event_loop绑定在一起的线程名字 main thread 和 sub threads
    std::string threadName_;
    //和event_loop绑定在一起的线程id
    pthread_t ownerThreadId_;

    int handlePendingChannel();
    //主reactor调用唤醒某一从reactor
    void wakeup();
    //绑定在根据wakefd生成的channel上
    int handleWakeup();

    int createEventfd();

public:
	EventLoop(std::string threadName);
	~EventLoop();
    //fd 指示 channel 的存在
    std::map<int,Channel*> channelMap_;
    
    std::string getThreadName() { return threadName_; }
	pthread_t getTid() { return ownerThreadId_;}
	int run();

	// int addChannelEvent(Channel * channel1);

    // int removeChannelEvent(Channel * channel1);

    // int updateChannelEvent(Channel * channel1);

    int channelOpEvent(Channel * channel);

  
    // int handlePendingAdd(int fd, struct channel *channel);

    // int handlePendingRemove(int fd, struct channel *channel);

    // int handlePendingUpdate(int fd, struct channel *channel);

// dispather派发完事件之后，调用该方法通知event_loop执行对应事件的相关callback方法
// res: EVENT_READ | EVENT_READ等

};




#endif
