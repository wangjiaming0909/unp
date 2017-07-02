#include <iostream>

//多项式求值
template <class T>
T polyEval(T *coeff, int n, const T &x){
    T y = 1, value = coeff[0];
    for(int i = 1; i <= n;i++){
        y *= x;
        value +=  y*coeff[i];
    }
    return value;
}

//名次计算
template <class T>
void rank(T *a, int n, int *r){
    for(int i = 0; i < n; i++){
        r[i] = 0;
    }
    for(int i = 0; i < n; i++){//大循环是从1 - n
        for(int j = 0; j < i; j++){//小循环是从1 - i，保证每两个元素之间都进行过比较
            if(a[j] <= a[i])
                r[i]++;//每大于一个元素，都在其位置上的名次加1
            else
                r[j]++;
        }
    }
}

//打印数组
template <class T>
void printarray(T *arr, int n){
    for(int i = 0; i < n; i++)
        std::cout << arr[i] << ",";
    std::cout << std::endl;
}

int main(){
    int x = 2;
    int buf[4] = {5,2,4,5};
    int a[9] = {3, 2,6,5,9,4,7,1,8};
//    int ret = polyEval(buf, 3, x);
//    std::cout << ret << std::endl;
    
    int ret[9];
    rank(a, 9, ret);
    printarray(ret, 9);

    return 0;
}
