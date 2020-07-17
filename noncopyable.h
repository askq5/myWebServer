/*************************************************************************
	> File Name: noncopyable.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Fri 17 Jul 2020 10:33:45 PM CST
 ************************************************************************/

#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class noncopyable {
 protected:
  noncopyable() {}
  ~noncopyable() {}

 private:
  noncopyable(const noncopyable&);
  const noncopyable& operator=(const noncopyable&);
};

#endif
