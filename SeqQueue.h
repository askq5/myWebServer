/*************************************************************************
	> File Name: SeqQueue.h
	> Author: nemo
	> Mail: 1445118957@qq.com
	> Created Time: Mon 10 Aug 2020 10:00:23 PM CST
 ************************************************************************/

#ifndef _SEQQUEUE_H
#define _SEQQUEUE_H


#include <iostream>

using namespace std;

#define MAX_SIZE 50

template <class T>
class   Queue
{
public:
    Queue(){};
    ~Queue(){};
    virtual bool enQueue(const T & x) = 0;
    virtual bool deQueue() = 0;
    virtual bool getFront(T & x) const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isFull()  const = 0;
    virtual int  getSize() const = 0;

};


template <class T>
class SeqQueue:public Queue<T>
{
private:
    /* data */
public:
    SeqQueue(int size = 10):head_(-1),tail_(0),length_(size){
        if(size > MAX_SIZE)
            cout << "the seqqueue  have " << MAX_SIZE << " elements at most" <<endl;
        elementsArray_ = new T[size];
    }
    ~SeqQueue(){ delete [] elementsArray_; }
     bool enQueue(const T & x);
     bool deQueue();
     bool getFront(T & x) const;
     bool isEmpty() const { return head_ == -1; }
     bool isFull()  const { return tail_ == length_; }
     int  getSize() const { if(tail_ == length_) return length_;
                    if(head_ == -1) return 0; 
                    return length_-abs(tail_-head_)+1;}
     template <typename T2>
     friend ostream & operator << (ostream & os, SeqQueue<T2> & Q);
protected:
    int head_,tail_;
    T * elementsArray_;
public:
    int length_;
};



#endif
