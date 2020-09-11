# myWebServer

主线程如何等待多个子线程初始化   锁加信号量
主线程如何通知处于分发中的子线程有新的时间加入删除修改   sockerpair      round_robin
httpServer处理完后得到的outBuffer_怎么告诉分发子线程以让其将buffer发出去    线程入口函数调用httpserver的处理函数后中处理


循环read时候记忆总共读了多少字符，其中先判断这次读到的字符是不是小于0，不小于0再累加
switch（变量chenck_state）
getcwd（），在用vscode调试时结果  与   直接在linux上运行时   结果不一样？
    咋办   在用vscode调试时 在 task.json  和 launch.json  中的 改变cwd
    
    
    close_wait()
    1,代码需要判断socket，一旦read返回0，断开连接，read返回负，检查一下errno，如果不是AGAIN，也断开连接。(注:在UNP 7.5节的图7.6中，可以看到使用select能够检测出对方发送了FIN，再根据这条规则就可以处理CLOSE_WAIT的连接)
2.给每一个socket设置一个时间戳last_update，每接收或者是发送成功数据，就用当前时间更新这个时间戳。定期检查所有的时间戳，如果时间戳与当前时间差值超过一定的阈值，就关闭这个socket。
3.使用一个Heart-Beat线程，定期向socket发送指定格式的心跳数据包，如果接收到对方的RST报文，说明对方已经关闭了socket，那么我们也关闭这个socket。
4.设置SO_KEEPALIVE选项，并修改内核参数


    
    
    
    
    
    
    
    
    
    
    
