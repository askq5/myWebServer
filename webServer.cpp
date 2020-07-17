/*************************************************************************
	> File Name: webServer.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 11:27:21 AM CST
 ************************************************************************/

#include "TcpServer.h"

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

//连接建立之后的callback
template <typename T>
int onConnectionCompleted(T * ptr) {
    printf("connection completed\n");
    return 0;
}

//数据读到buffer之后的callbacknectiostruct
template <typename T>
int onMessage(T * ptr) {
    printf("get message from tcp connection %s\n", ptr->getThreadName());
    //printf("%s", input->data);

   
    for (int i = 0; i < size; i++) {
        buffer_append_char(ptr->outBuffer_, rot13_char(buffer_read_char(ptr->inBuffer)));
    }
    ptr->eventLoop_
    return 0;
}

//数据通过buffer写完之后的callback
template <typename T>
int onWriteCompleted(T *ptr) {
    printf("write completed\n");
    return 0;
}

//连接关闭之后的callback
template <typename T>
int onConnectionClosed(T *ptr) {
    printf("connection closed\n");
    return 0;
}

int main(int c, char **v) {
    //主线程event_loop
    //struct event_loop *eventLoop = event_loop_init();

    //初始化acceptor
    // acceptor *acceptor = acceptor_init(SERV_PORT);

    //初始tcp_server，可以指定线程数目，这里线程是1，说明是一个acceptor线程，1个I/O线程
    //tcp_server自己带一个event_loop
    TcpServer *tcpServer = new TcpServer(43211,1);
	tcpServer->setMessage(onMessage<void>);
    tcpServer->start();

    // main thread for acceptor
    //event_loop_run(eventLoop);
}
