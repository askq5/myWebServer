/*************************************************************************
	> File Name: noncopyable.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 10:33:45 PM CST
 ************************************************************************/

#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H
//把拷贝构造函数 和 拷贝复制函数私有化
//多重继承可能会被优化
//c++11 delete关键字 删除掉默认的拷贝构造，拷贝复制
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}

private:
    noncopyable(const noncopyable &);
    const noncopyable &operator=(const noncopyable &);
};

#endif
