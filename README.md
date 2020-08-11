# myWebServer

主线程如何等待多个子线程初始化   锁加信号量
主线程如何通知处于分发中的子线程有新的时间加入删除修改   sockerpair      round_robin
httpServer处理完后得到的outBuffer_怎么告诉分发子线程以让其将buffer发出去    线程入口函数调用httpserver的处理函数后中处理


循环read时候记忆总共读了多少字符，其中先判断这次读到的字符是不是小于0，不小于0再累加
switch（变量chenck_state）
getcwd（），在用vscode调试时结果  与   直接在linux上运行时   结果不一样？
    咋办   在用vscode调试时 在 task.json  和 launch.json  中的 改变cwd