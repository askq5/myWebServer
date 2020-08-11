/*************************************************************************
	> File Name: SqlConnectionPool.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Mon 10 Aug 2020 10:02:10 PM CST
 ************************************************************************/

#include <mysql/mysql.h>
#include <string>
#include <pthread.h>
#include <iostream>

#include "SqlConnectionPool.h"


SqlConnectionPool::SqlConnectionPool()
{
	
}

SqlConnectionPool *SqlConnectionPool::GetInstance()
{
	static SqlConnectionPool connPool;
	return &connPool;
}

//构造初始化
void SqlConnectionPool::init(string User, string PassWord, string DBName, int MaxConn, int close_log,string url,int Port)
{
	url_ = url;
	port_ = Port;
	user_ = User;
	passWord_ = PassWord;
	databaseName_ = DBName;
	closeLog_ = close_log;
    pthread_mutex_init(&mutex_,NULL);

	for (int i = 0; i < MaxConn; i++)
	{
		MYSQL *con = NULL;
		con = mysql_init(con);

		if (con == NULL)
		{
			//LOG_ERROR("MySQL Error");
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
			//LOG_ERROR("MySQL Error");
			exit(1);
		}
		connPool_.enQueue(con);
	}
    if (sem_init(&sem_, 0, MaxConn) != 0)
    {
        //throw std::exception();
    }
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *SqlConnectionPool::GetConnection()
{
	MYSQL *con = NULL;

	if (connPool_.isEmpty())
		return NULL;

	sem_wait(&sem_);
	pthread_mutex_lock(&mutex_);

	connPool_.getFront(con);
	connPool_.deQueue();
	pthread_mutex_unlock(&mutex_);
	return con;
}

//释放当前使用的连接
bool SqlConnectionPool::ReleaseConnection(MYSQL *con)
{
	if (NULL == con)
		return false;

	pthread_mutex_lock(&mutex_);
	connPool_.enQueue(con);
	pthread_mutex_unlock(&mutex_);

	sem_post(&sem_);
	return true;
}

//销毁数据库连接池
void SqlConnectionPool::DestroyPool()
{

	pthread_mutex_lock(&mutex_);
	while(!connPool_.isEmpty())
	{
		MYSQL *con;
        connPool_.getFront(con);
		mysql_close(con);
		connPool_.deQueue();
	}

	pthread_mutex_unlock(&mutex_);
    pthread_mutex_destroy(&mutex_);
    sem_destroy(&sem_);
}

//当前空闲的连接数
int SqlConnectionPool::GetFreeConn()
{
	return connPool_.length_ - connPool_.getSize();
}

SqlConnectionPool::~SqlConnectionPool()
{
	DestroyPool();
}

ConnectionRAII::ConnectionRAII(MYSQL **SQL, SqlConnectionPool *connPool){
	*SQL = connPool->GetConnection();
	
	conRAII = *SQL;
	poolRAII = connPool;
}

ConnectionRAII::~ConnectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}