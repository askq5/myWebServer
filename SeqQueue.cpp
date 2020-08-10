/*************************************************************************
	> File Name: SeqQueue.cpp
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Mon 10 Aug 2020 09:55:16 PM CST
 ************************************************************************/
#include "SeqQueue.h"

template <class T>
bool SeqQueue<T>::enQueue(const T& x) {
    if(isFull())
    {    
        cout << "full" << endl;
        return false;
    }
    elementsArray_[tail_] = x;
    if(isEmpty())
        head_ = tail_;
    tail_ = (tail_+1)%length_;
    if(tail_ == head_)
        tail_ = length_;
    return true;
}

template <class T>
bool SeqQueue<T>::deQueue(){
    if(isEmpty())
    {
        cout << "empty" << endl;
        return false;
    }
    if(isFull())
        tail_ = head_;
    head_ = (head_+1)%length_;
    if(tail_ == head_)
        head_ = -1;
    return true;
}

template<class T>
bool SeqQueue<T>::getFront(T& x)const{
    if(isEmpty())
        return false;
    x =  elementsArray_[head_];
    return true;
}

template <class T>
ostream& operator << (ostream& os,SeqQueue<T>& Q) {
    // 输出栈中元素的重载操作 <<
    os << "front =" << Q.front <<", rear="<<Q.rear<<endl;
    for (int i = Q.head_; i!= Q.tail_; i = (i+1) % Q.length_)
    os << i << ":"<< Q.elements[i] << endl;
    return os;
};
