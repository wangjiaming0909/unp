#include <iostream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;
void fcn3(){
    int v1 = 42;
    auto f = [v1]()mutable {return ++ v1;};
    v1 = 0;
    auto j = f();
    cout << j << endl;
   cout << v1 << endl; 
}


int main(){
//    fcn3();
//    cout << "aaaa" << endl;
    istream_iterator<int> in_iter(cin);
    istream_iterator<int> eof;
    
    vector<int> v(in_iter, eof);
    while(in_iter != eof)
        v.push_back(*in_iter++);
   
    ostream_iterator<int> out_iter(cout, " ");
    copy(v.begin(), v.end(), out_iter);
    cout << endl;

    return 0;
}
