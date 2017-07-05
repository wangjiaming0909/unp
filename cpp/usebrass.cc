#include "acctabc.h"
#include <string>
#include <iostream>
const int CLIENTS = 4;
using namespace std;

int main(){
	AcctABC*		p_client[CLIENTS];
	string				temp;
	long				tempnum;
	double			tempbal;
	char 				kind;

	for(int i = 0; i < CLIENTS; i++){
		cout << "Enter client's name: ";
		getline(cin, temp);
		cin >> tempnum;
		cout  << "Enter opening balance: $";
		cin >> tempbal;
		cout << "Enter 1 for Brass Account or 2 for BrassPlus Account: ";
		while(cin >> kind && (kind != '1' && kind != '2'))
			cout << "Enter either 1 or 2: ";
		if(kind == '1')
			p_client[i] = new Brass(temp, tempnum, tempbal);
		else{
			double tmax, trate;
			cout << "Enter the overdraft limit; $ ";
			cin >> tmax;
			cout << "Enter the interest rate as a decimal fraction: ";
			cin >> trate;
			p_client[i] = new BrassPlus(temp, tempnum, tempbal, tmax, trate);
		}
		while(cin.get() != '\n')
			continue;
	}
	cout << endl;
	for(int i = 0; i < CLIENTS; i++){
		p_client[i]->ViewAcct();
		cout << endl;
	}

	for(int i = 0; i < CLIENTS; i++)
		delete p_client[i];

	cout << endl;
	return 0;
}
