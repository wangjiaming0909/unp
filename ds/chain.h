/*************************************************************************
	> File Name: chain.h
	> Author: 
	> Mail: 
	> Created Time: Thu 03 Aug 2017 05:10:32 PM CST
 ************************************************************************/

#ifndef _CHAIN_H
#define _CHAIN_H

#include "linearlist.h"

template <class T>
struct chainNode{
    T element;
    chainNode<T> *next;

    chainNode(){}
    chainNode(const T&element){this->element = element;}
}


#endif
