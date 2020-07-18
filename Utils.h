/*************************************************************************
	> File Name: utils.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 05:09:35 PM CST
 ************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <cstdlib>
#include <string>

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
void handle_for_sigpipe();
int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);

#include "EventLoop.h"

void assertInSameThread(EventLoop *eventLoop);

//1： same thread: 0： not the same thread
int isInSameThread(EventLoop *eventLoop);

#endif
