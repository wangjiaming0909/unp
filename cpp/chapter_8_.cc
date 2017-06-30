#include <iostream>
#include <string>
using namespace std;

static int a = 1;
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
int main(){

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

	return 0;
}



