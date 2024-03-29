/*************************************************************************
	> File Name: webServer.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 11:27:21 AM CST
 ************************************************************************/
#include <functional>
#include <iostream>
#include <string>

#include "TcpServer.h"
#include "HttpServer.h"

using std::string;

int main(int c, char **v) {
    
    std::cout << "start" << std::endl;
    string user = "root";
    string passwd = "askq";
    string databasename = "web_server";

    //初始化数据库连接池
    HttpServer::sqlConnPool_->init(user, passwd, databasename, 8);
    HttpServer httpServer;
    
    //初始tcp_server，可以指定线程数目，这里线程是1，说明是一个acceptor线程，1个I/O 从reactor线程
    //tcp_server自己带一个event_loop
    TcpServer *tcpServer = new TcpServer(2020,1,2);
	tcpServer->setMessage(std::bind(&HttpServer::handleMessage,httpServer,
                            std::placeholders::_1,std::placeholders::_2));
    tcpServer->start();

    return 0;
}
