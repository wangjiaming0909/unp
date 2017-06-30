#include <iostream>
#include <climits>
using namespace std;

int sum_arr(int arr[], int n){
	cout << "123" <<endl;
	// cout <<"sizeof arr[]:" <<  sizeof(arr) << endl << n << endl;//8
//	cout << arr[0] << endl << arr[1] << endl << arr [2] << endl;
//	cout << arr << endl << arr + 1 << endl << arr + 2 << endl;
	return 0;
}

//演示枚举类型
int spectrum(){
		enum spectrum {
		red,
		yellow,
		blue
	};
	spectrum band;
	return 0;
}

int arraypoint(){
	//int (*) [16];
	int buf[16] = {1,2,3,4};
	typedef int (*funcb)(int*, int);//函数指针类型
	typedef int funcb2(int *, int);//函数类型
	funcb2 *f2 = sum_arr;//函数类型定义了一个指针
	f2(buf, 16);
	funcb f = sum_arr;//函数指针类型定义了一个指针
	f(buf, 16);
	int (*p)[16] = &buf;//是一个数组类型的指针
	typedef int (*arrp)[16];//数组指针类型
	arrp pp = &buf;
	cout << (*p)[0] <<endl;
	return 0;
}

int swapref(int &a, int &b){
//	int tmp;

//	tmp = a;a = b; b = tmp;
	return a + b;
}

int main01(){
	// int  buf[32] = {2};

	// *(buf + 1) = 12;
	// *(buf + 2)= 13;
	// cout  << &buf << endl;//0x7ffd77d21630
	// //sum_arr(buf, 1);
	// auto p2 = sum_arr;
	// p2(buf, 1);
	// cout << "sizeof buf[1]:" << sizeof(buf) << endl;
    // cout << "hello world" << endl;
//	arraypoint();
//	char *p = "asd";
//	"asd " = "qwe";
//	cout << p << endl;
//	生成临时变量的条件:
//	1, 实参的类型正确,但是不能修改
//	2, 实参的类型不正确, 但可以转换为正确的类型
//	long a = 2, b = 3;//报错, 类型不匹配, 生成临时变量?
//	貌似不生成临时变量, 实参类型不匹配, 直接报错
	int a = 2, b = 3;
	cout << swapref(a, b) << endl;
//	swapref(1,2);//报错, 实参类型正确, 但不是左值, 直接报错

//	struct s{
//		int i;
//		long j;
//	};
//
//	s s1 = {1,2};
//	char *p = "asd";
//	*p = 'b';//段错误
//	cout << p << endl;
	const double dd = 1.23;
	const double &d = 1.2 + 2.4;//右值表达式也可以
	const double &d1 = a;//可修改的左值, 类型不匹配也可以
	const double &d3 = 1;//不可修改的右值可以
	const double &dd1 = dd;//const 左值也可以
	int &d2 = a;
//	int &d4 = 1;//不可以修改的右值不可以
//	int &d5 = 1 + 2;//不可以

	int &&d6 = 1+2;//右值
	cout << d << endl;

//	wcha
	wchar_t ch2 = L'q';
	cout << "wchat_t的大小为:" << sizeof(ch2) << endl;
	char16_t ch1 = u'q';
	char32_t ch3 = U'w';
	int abc(13);
//	int abcd{14.0};
	double abcd2{1};
	cout << "char16_t size: " << sizeof(ch1) << endl;
	cout << "char32_t size: " << sizeof(ch3) << endl;
	return 0;
}
