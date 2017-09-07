#include <iostream>

using namespace std;

class A{
public:
    static constexpr double d = 12.2;
    static const int i = 12;
};

//constexpr double A::d;
const int A::i;
void print(const double &d){
    cout << d << endl;
}
void print(const int &i){
    cout << i << endl;
}

int main(){
    print(1.1);
    print(A::i);
    cout << A::d << endl;
    return 0;
}
