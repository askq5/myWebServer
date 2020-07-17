/*************************************************************************
	> File Name: TcpServer.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Thu 16 Jul 2020 03:15:18 PM CST
 ************************************************************************/
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <signal.h>


#include "TcpServer.h"
#include "TcpConnection.h"

#define LISTENQ 100

TcpServer::TcpServer(int port, int threadsNum)
{
	acceptor_ =new Channel(listenNoblock(port),OP_ADD);
	eventLoopThreadPool_ = new EventLoopThreadPool(threadsNum);

}

TcpServer::~TcpServer()
{
	delete acceptor_;
	delete eventLoopThreadPool_;
}

void TcpServer::make_nonblocking(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

int TcpServer::listenNoblock(int port) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    make_nonblocking(listenfd);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        //error(1, errno, "bind failed ");
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        //error(1, errno, "listen failed ");
    }

    signal(SIGPIPE, SIG_IGN);

    return listenfd;
}

//初始化已建立连接的回调函数
int TcpServer::handleConnectioneEstablished() {
    
    int listenfd = acceptor_->getFd();
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connected_fd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len);
    make_nonblocking(connected_fd);
    
    EventLoop *eventLoop = eventLoopThreadPool_->getNextLoop();

    // create a new tcp connection 本质是封装了一个channel 及 其它
    //tcpconnection 中根据fd创建connectedchannel 设置connectedchannel的回调函数(其中会调用tcpserver的回调函数？)
	//并将该channel注册到eventloop
    TcpConnection *tcpConnection = new TcpConnection(connected_fd,eventLoop,this);
    //tcpConection->setName  = eventLoop->getThreadName();
	//tcpConnection->setTcpServer(this);

    return 0;
}

void TcpServer::start() 
{

    //开启多个线程
    eventLoopThreadPool_->start();

    //设置acceptor的回调函数 并将acceptor注册到baseeventloop
	acceptor_->setReadHandler(std::bind(&TcpServer::handleConnectioneEstablished,this));
    //acceptor_->setOp();
	eventLoopThreadPool_->baseEventLoop_->channelMap_[acceptor_->getFd()] = acceptor_;
	eventLoopThreadPool_->baseEventLoop_->channelOpEvent(acceptor_);

	//运行baseeventloop 
	eventLoopThreadPool_->baseEventLoop_->run();
    return;
}

