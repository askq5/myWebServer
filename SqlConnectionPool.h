/*************************************************************************
	> File Name: SqlConnectionPool.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Mon 10 Aug 2020 10:01:36 PM CST
 ************************************************************************/

#ifndef _SQLCONNECTIONPOOL_H
#define _SQLCONNECTIONPOOL_H

#include <pthread.h>
#include <semaphore.h>
#include <mysql/mysql.h>
#include <string>
#include "SeqQueue.h"

using std::string;

class SqlConnectionPool
{
public:
    string url_;
    string port_;
    string user_;
    string passWord_;
    string databaseName_;
    int closeLog_;
private:
    /* data */
    //锁
    pthread_mutex_t mutex_;
    //信号量
    sem_t sem_;
    //连接池
    SeqQueue<MYSQL *>  connPool_;
public:
    MYSQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MYSQL *conn); //释放连接
	int GetFreeConn();					 //获取当前可用连接数
	void DestroyPool();					 //销毁所有连接

	//单例模式
	static SqlConnectionPool *GetInstance();

	void init(string User, string PassWord, string DBName, int MaxConn, 
                int close_log, string url, int Port); 

private:
    SqlConnectionPool(/* args */);
    ~SqlConnectionPool();

};


class ConnectionRAII{

public:
	ConnectionRAII(MYSQL **con, SqlConnectionPool *connPool);
	~ConnectionRAII();
	
private:
	MYSQL *conRAII;
	SqlConnectionPool *poolRAII;
};

#endif
