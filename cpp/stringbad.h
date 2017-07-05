#include <iostream>
#include <cstring>
#ifndef STRINGBAD_H
#define STRINGBAD_H

class StringBad{
    char *str;
    int len;
    static int num_strings;
    static const int CINLIM = 80;//cin input limit
public:
    StringBad(const char *s);
    StringBad(const StringBad &);
    /*
     * 为社么要定义=操作符？
     * 为了：给一个对象用等号赋值，注意不是初始化，初始化会先调用复制构造，则此函数非必需
     * 等号赋值时，为了避免浅拷贝，需要定义这个版本的operator=
     * */
    StringBad& operator=(const StringBad &);
    /*
     * 为什么要定义=的重载版本呢？
     * 当调用： StringBad ss;ss = "abcd";时
     * 可以避免调用的三步： 构造临时对象，调用operator=， 析构临时对象
     * 中生成的临时对象，提高效率
     * */
    StringBad& operator=(const char *s);
    StringBad();
    ~StringBad();
    int length() const{return len;};
    friend bool operator<(const StringBad &, const StringBad &);
    friend bool operator>(const StringBad &, const StringBad &);
    //为什么要使用友元？
    /*
     * 不使用友元：bool operator==(const StringBad &st);
     * 当调用if(st1 == st2)...这样可以
     * 但是当调用 if("C++" == st2)时将会报错
     * 使用了友元： 编译器会将"C++"自动调用构造函数StringBad(const char *s);
     * 生成一个对象，再调用 operator== 运算符
     * */
    friend bool operator==(const StringBad &, const StringBad &);
/*	为什么返回一个引用：
	因为可以编写这样的代码：StringBad means("mighs");
    means[0] = 'r';	//可以修改means对象
	当定义个const 对象时，此const对象只能调用const成员函数
	如：cout << mean[1];将导致错误，因为operator<<运算符没有定义成const函数
*/
    char& operator[](int i);
    //因此需要定义operator[]的一个重载版本，专门给const对象使用
    const char& operator[](int i)const;//这就重载了？
    static int HowMany(){return num_strings;};

    friend std::istream& operator>>(std::istream &is, StringBad &st);
    friend std::ostream& operator<<(std::ostream &os, const StringBad &st);
};


#endif
