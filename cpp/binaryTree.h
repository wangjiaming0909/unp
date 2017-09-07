/*************************************************************************
	> File Name: binaryTree.h
	> Author: 
	> Mail: 
	> Created Time: Thu 31 Aug 2017 04:36:22 PM CST
 ************************************************************************/

#ifndef _BINARYTREE_H
#define _BINARYTREE_H

template <class T>
class BinaryTree{
public:
    virtual ~BinaryTree(){}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual void preOrder(void (*)(T*)) = 0;
    virtual void inOrder(void (*)(T*)) = 0;
    virtual void postOrder(void (*)(T*)) = 0;
    virtual void levelOrder(void (*)(T*)) = 0;
};


#endif
