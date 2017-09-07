/*************************************************************************
	> File Name: linearlist.h
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Aug 2017 10:08:36 AM CST
 ************************************************************************/

#ifndef _LINEARLIST_H
#define _LINEARLIST_H
#include <iostream>
template <class T>
class LinearList{
public:
    virtual ~LinearList(){};
    virtual bool empty()const = 0;
    virtual int size() const = 0;
    virtual T& get(int theIndex)const = 0;
    virtual int indexOf(const T& theElement)const = 0;
    virtual void erase(int theIndex) = 0;
    virtual void insert(int theIndex, const T& theElement) = 0;
    //virtual void output(std::ostream &out)const = 0;
};

//改变数组的长度
template <class T>
void changeLength1D(T*&a, int oldLength, int newLength){
    if(newLength < 0)
        throw "new length must be >= 0";
    T* temp = new T[newLength];
    int number = std::min(oldLength, newLength);
    std::copy(a, a + number, temp);
    delete [] a;
    a = temp;
}

#endif
