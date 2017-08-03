/*************************************************************************
	> File Name: arraylist.h
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Aug 2017 10:23:14 AM CST
 ************************************************************************/

#ifndef _ARRAYLIST_H
#define _ARRAYLIST_H
#include "linearlist.h"
#include <sstream>
template <class T>
class arrayList : public LinearList<T>{
public:
    arrayList(int initialCapacity = 10);
    arrayList(const arrayList<T> &);
    ~arrayList() {delete [] element;}
    
    bool empty() const {return listSize == 0;}
    int size() const {return listSize;}
    T& get(int theIndex)const;
    int indexOf(const T& theElement)const;
    void erase(int theIndex);
    void insert(int theIndex, const T& theElement);
    void output(std::ostream &out)const;
    int capacity()const {return arrayLength;}
   
protected:
    void checkIndex(int theIndex) const;
    T* element;
    int arrayLength;
    int listSize;

};

template <class T>
arrayList<T>::arrayList(int initialCapacity){
    if(initialCapacity < 1){
        std::ostringstream s;
        s << "initial Capacity = " << initialCapacity << " must be > 0";
        throw s.str();
    }
    arrayLength = initialCapacity;
    element = new T[arrayLength];
    listSize = 0;
}

template <class T>
arrayList<T>::arrayList(const arrayList<T>& thelist){
    arrayLength = thelist.arrayLength;
    listSize = thelist.listSize;
    element = new T[arrayLength];
    std::copy(thelist.element, thelist.element + listSize, element);
}

template <class T>
void arrayList<T>::checkIndex(int theIndex) const{
    if(theIndex < 0 || theIndex >= this->listSize){
        std::ostringstream s;
        s << "index = " << theIndex << "size = " << listSize;
        throw s.str();
    }    
}

template <class T>
T& arrayList<T>::get(int theIndex)const{
    checkIndex(theIndex);
    return element[theIndex];
}

template <class T>
int arrayList<T>::indexOf(const T&theElement)const {
    int theIndex = std::find(this->element, this->element + this->listSize, theElement) - element;
    if(theIndex == listSize)
        return -1;
    else
        return theIndex;
}

template <class T>
void arrayList<T>::erase(int theIndex){
    
}

template <class T>
void arrayList<T>::insert(int theIndex, const T& theElement){

}

template <class T>
void arrayList<T>::output(std::ostream& out)const{
    
}

template <class T>
std::ostream& operator<<(std::ostream& out, const arrayList<T>& x){
    x.output(out);
    return out;
}
#endif
