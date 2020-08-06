# myWebServer

主线程如何等待多个子线程初始化   锁加信号量
主线程如何通知处于分发中的子线程有新的时间加入删除修改   sockerpair      round_robin
httpServer处理完后得到的outBuffer_怎么告诉分发子线程以让其将buffer发出去    线程入口函数调用httpserver的处理函数后中处理