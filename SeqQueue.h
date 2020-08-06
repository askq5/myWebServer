#include <iostream>

using namespace std;

#define MAX_SIZE 50
template <class T>
class   Queue{
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