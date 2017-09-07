#include <iostream>
#include <vector>
using namespace std;

class Example{
public:

    static void print(){
//        vecsize = 10;
        cout << vecsize << endl;
    }
   //constexpr double d = 12.2;
    constexpr static double rate = 6.4;
    static const int vecsize = 10.0;
    static vector<double> vec(double);
    const int a = 10;
};

constexpr double Example::rate;
void print1(const double &d){
    cout << d << endl;
}


int mainmain2(){
    Example e;
   // e.print();
    e.print();
    Example::print();
    print1(e.rate);
    //int x = 12;
    //int &r = x;
    
    //cout << "hello world" << endl;
    return 0;
}

