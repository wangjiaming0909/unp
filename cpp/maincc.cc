#include <iostream>
using namespace std;
#include "stringbad.h"

void callme1(StringBad &);
void callme2(StringBad );

int maincc(){
//	char tmp[16] = "abcd";
	StringBad ss;
	/*
	 * 这条语句分三步执行：
	 * 1， 调用StringBad::StringBad(const char *s)构造函数生成临时对象
	 * 2， 调用StringBad& StringBad::operator=(const StringBad &);//注意const
	 * 3， 调用析构函数StringBad::~StringBad()释放临时对象
	 * */
	ss = "cdeg";
	StringBad *sp = new StringBad(ss);
//    cout << "Starting an inner block.\n";
    StringBad headline1("headline1");
    StringBad headline2("headline2");
    StringBad sports("sports");

//    cout << "headline1: " << headline1 << endl;
//    cout << "headline2: " << headline2 << endl;
//    cout << "sports: " << sports << endl;
//    callme1(headline1);
//    cout << "headline1: " << headline1 << endl;
    callme2(headline2);
    cout << "headline2: " << headline2 << endl;
    cout << "Initialize one object to another: \n";
    StringBad sailor = sports;
    cout << "sailor: " << sports << endl;
    cout << "Assign one object to another: \n";
    StringBad knot;
    //赋值运算符需要重载
    knot = headline1;//析构headline1时发生错误
    cout << "knot: " << knot << endl;
    cout << "Exiting the block.\n";
    return 0;
}

void callme1(StringBad &rsb){
    cout << "String passed by ref: \n";
    cout << "   \"" << rsb << "\"\n";
}

void callme2(StringBad sb){
    cout << "String passed by value:" << endl;;
    cout << "   \"" << sb << "\"" << endl;;
}
