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
#include <algorithm>
#include <iterator>
template <class T>
class Iiterator;

template <class T>
class arrayList : public LinearList<T>{
public:
    typedef Iiterator<T> iterator;
    
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
//    void output(std::ostream &out)const;
    int capacity()const {return arrayLength;}


    iterator begin() {return iterator(element);}
    iterator end() {return iterator(element + listSize);}

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
   checkIndex(theIndex);
    std::copy(element + theIndex + 1, element + listSize, element + theIndex);
    element[--listSize].~T();
}

template <class T>
void arrayList<T>::insert(int theIndex, const T& theElement){
    //检查是否是无效index
    if(theIndex < 0 || theIndex > listSize){
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw s.str();
    }
    //valid index
    //check the array for full, for full, expand the size of array
    if(this->listSize == arrayLength){
        changeLength1D(element, arrayLength, 2 * arrayLength);
        arrayLength *= 2;
    }
    //move elements after theIndex one step after
    std::copy_backward(element + theIndex, element + listSize, element + listSize + 1);

    element[theIndex] = theElement;
    listSize ++;
}

/*
template <class T>
void arrayList<T>::output(std::ostream& out)const{
    std::copy(element, element + listSize, std::ostream_iterator<T>(out, " "));
}

template <class T>
std::ostream& operator<<(std::ostream& out, const arrayList<T>& x){
    x.output(out);
    return out;
}
*/
template <class T>
class Iiterator{
public:
    typedef std::bidirectional_iterator_tag         iterator_category;
    typedef T                                       value_type;
    typedef std:: ptrdiff_t                         difference_type;
    typedef T*                                      pointer;
    typedef T&                                      reference;
    

    Iiterator(T* theposition = 0){position = theposition;}

    T& operator*()const {return *position;}
    T* operator->(){return &*position;}
protected:
    T* position;

};

#endif
