#include <iostream>
using namespace std;

template <typename T>
void bubble1(T *t, int n){
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

//一次冒泡过程
template <class T>
void bubble(T *a, int n){
    for(int i = 0; i < n - 1; i++ )
    if(a[i] > a[i+1]) 
        swap(a[i], a[i+1]);
}

template <class T>
void bubblesort1(T *a, int n){
    for(int i = n; i > 1; i--)
    bubble(a, i);//一次冒泡之后，最大的元素到了最后位置，再次调用bubble时，参数为(T *a, i),即冒泡前面的元素
}


//能及时终止的冒泡排序
//	int buf[14] = {94,12,53,23,23,6,2,623,68,9,3,2,1243,23};
template <class T>
void bubblesort(T *a, int n){
    bool swapped = true;
    for(int i = n; i > 1 && swapped == true; i--){
		swapped = false;
        for(int j = 0; j< i - 1; j++){
        	if(a[j] > a[j+1]){
        		swap(a[j], a[j+1]);
        	    for(int l = 0; l < n; l++){
        			cout << a[l] << ",";
        		}
        	    cout << endl;
        		swapped = true;
        	}
        }
    }
}

//寻找最大元素
template <class T>
int indexofmax(T *a, int n){
    if(n <= 0)
        return -1;
    int indexofmax = 0;
    for(int i = 1; i < n; i++)
        if(a[indexofmax] < a[i])
            indexofmax = i;
    return indexofmax;
}

//不能及时终止的选择排序
template <class T>
void selectionsort1(T *a, int n){
    for(int size = n; size > 1; size--){
        int j = indexofmax(a, size);
        swap(a[j], a[size - 1]);//找出最大的然后和最后一个交换
    }
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

//插入
template <class T>
void insert(T *a, int n, const T&x){
	int i;
	for(i = n-1; i >= 0 && x < a[i]; i--)
		a[i+1] = a[i];
	a[i+1] = x;

	for(int l = 0; l < 14; l++){
		cout << a[l] << ",";
	}
    cout << endl;
}

//插入排序
template <class T>
void insertsort(T *a, int n){
	for(int i = 1; i< n; i++){
		T t = a[i];
		insert(a, i, t);

	}
}

int main01(){
	int buf[14] = {94,12,53,23,23,6,2,623,68,9,3,2,1243,23};
//	bubble(buf, 14);
//	selectionsort(buf, 14);
//    bubblesort(buf, 14);
	insertsort(buf, 14);
    for(int i = 0; i < 14; i++){
		cout << buf[i] << ",";
	}
	return 0;
}
