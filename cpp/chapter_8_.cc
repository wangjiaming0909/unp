#include <iostream>
#include <string>
using namespace std;

static int a = 1;
//将abc2定义的文件编译成.o文件, 并链接到a.out中,
//abc2定义为外部链接的静态存储连续性变量, 因此, 可被链接到执行程序中
extern long abc2;
static long abc1 = 13;
//char ch;

template <typename T>
void iquote(T  t){
	cout << "$" << t << "$" << endl;
}

template <typename T>
T average(T a, T b) {
	return (a + b) / 2;
}

//template <typename T>
//T mangle(T t){
//
//}

inline void printex(const char *p, int n){
	if (n != 0){
		for(int i = 0; i < a; i++){
			cout << p << endl;
		}
		a ++;
	}
}


static void printf(char *format, ...){
	cout << format << "123"<< endl;
}

int maincha(){

	int a = 1;
	double b = 2.0000;
	string c = "3.000000";
	iquote(a);
	iquote(b);
	iquote(c);
	//cout.setf(ios::fixed);
	//cout.precision(2);


	cout << average(3.0, 6.0) << endl;
	printex("asdad", 1);
	printex("qweq", 1);
	printex("zxc", 1);

	cout << abc2 << endl;
	cout <<abc1 << endl;
	printf("asd");

	//初始化动态内存的数组,结构
	struct where{
		int x; int y; double z;
	};
	where *one = new where{1,2,3.1};
	int * ar = new int[4]{2,3,4,1};
	return 0;
}



