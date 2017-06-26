#include <iostream>
using namespace std;

int sum_arr(int arr[], int n){
	cout << "123" <<endl;
	cout <<"sizeof arr[]:" <<  sizeof(arr) << endl << n << endl;//8
//	cout << arr[0] << endl << arr[1] << endl << arr [2] << endl;
//	cout << arr << endl << arr + 1 << endl << arr + 2 << endl;
	return 0;
}

int main(){
	int  buf[32] = {2};
	*(buf + 1) = 12;
	*(buf + 2)= 13;
	cout  << &buf << endl;//0x7ffd77d21630
	//sum_arr(buf, 1);
	auto p2 = sum_arr;
	p2(buf, 1);
	cout << "sizeof buf[1]:" << sizeof(buf) << endl;
    cout << "hello world" << endl;
    return 0;
}
