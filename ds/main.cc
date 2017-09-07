#include <iostream>
#include "arraylist.h"
#include <vector>
using namespace std;

int main(){
    arrayList<vector<int> > al(4);
    vector<int> v1 = {1,2,3,4};
    vector<int> v2 = {5,6,7,8};
    al.insert(0, v1);
    al.insert(0, v2);


    auto it = al.begin();
    auto it2 = it->begin();
    cout << *it2 << endl;

    return 0;
}
