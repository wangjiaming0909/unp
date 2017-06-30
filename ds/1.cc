#include <iostream>
using namespace std;

template <typename T>
void bubble(T *t, int n){
	for(int i = 0; i < n; i++){
		for(int j = i; j < n; j ++){
			if(t[j] < t[i]){
				T tmp = t[j];
				t[j] = t[i];
				t[i] = tmp;
			}
		}
	}
}

template <class T>
void swap1(T &a, T &b){
	T tmp ;
	tmp = a;
	a = b;
	b = tmp;
}

//及时终止的选择排序
template <class T>
void selectionsort(T *a, int n){
	bool sorted = false;
	for(int size = n; !sorted && (size > 1); size--){
		int indexofmax = 0;
		sorted = true;

		//寻找最大的元素
		for(int i = 1; i < size; i ++)
			if(a[indexofmax] <= a[i]) indexofmax = i;
			else sorted = false;//如果这个for循环循环一遍,没有到这一句,说明已经排序完成了

		swap(a[indexofmax], a[size - 1]);
	}
}
int main(){
	int buf[14] = {94,12,53,23,23,6,2,623,68,9,3,2,1243,23};
//	bubble(buf, 14);
	selectionsort(buf, 14);
	for(int i = 0; i < 14; i++){
		cout << buf[i] << ",";
	}
	cout << "asd" << endl;
	return 0;
}
