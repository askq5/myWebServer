/*************************************************************************
	> File Name: Thread.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 10:33:45 PM CST
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>

#include "NonCopyable.h"

class Thread : NonCopyable
{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc &, const std::string &name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }

private:
    void setDefaultName();
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
};

#endif
