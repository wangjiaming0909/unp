#include <iostream>
#include "acctabc.h"
using std::cout;
using std::ios_base;
using std::endl;
using std::string;

//Abstract Base class
AcctABC::AcctABC(const string &s, long an, double bal){
	fullName = s;
	acctNum = an;
	balance = bal;
}

void AcctABC::Deposit(double amt){
	if(amt < 0)
		cout << "Negative depost not allowed;" << "depost is canceled." << endl;
	else
		balance += amt;
}

//protected methods for formatting
void AcctABC::Withdraw(double amt){
	balance -= amt;
}

AcctABC::Formatting AcctABC::SetFormat()const{
	Formatting f;
	f.flags = cout.setf(ios_base::fixed, ios_base::floatfield);
	f.pr = cout.precision(2);
	return f;
}

void AcctABC::Restore(Formatting &f)const{
	cout.setf(f.flags, ios_base::floatfield);
	cout.precision(f.pr);
}

//Brass methods
void Brass::Withdraw(double amt){
	if(amt < 0)
		cout << "withdraw1 amount must be positive;" << "withdraw canceled." << endl;
	else if(amt <= Balance())
		AcctABC::Withdraw(amt);
	else
		cout << "Withdraw amount of $" << amt << " exceeds your balance." << endl << "withdraw canceled." << endl;
}

void Brass::ViewAcct()const{
	Formatting f = SetFormat();
	cout << "Brass Client: " << FullName() << endl;
	cout << "Account number: " << AcctNum() << endl;
	cout << "Balance: $" << Balance() << endl;
	Restore(f);
}

//BrassPlus Methods
BrassPlus::BrassPlus(const string &s, long an, double bal, double ml, double r) : AcctABC(s, an, bal){
	maxLoan = ml;
	owesBank = 0.0;
	rate = r;
}

BrassPlus::BrassPlus(const Brass &ba, double ml, double r) : AcctABC(ba){
	maxLoan = ml;
	owesBank = 0.0;
	rate = r;
}

void BrassPlus::ViewAcct() const {
	Formatting f = SetFormat();

	cout << "BrassPlus Client: "<< FullName() << endl;
	cout << "Account Number: " << AcctNum() << endl;
	cout << "Balance: $ " << Balance() << endl;
	cout << "Maximum loan: $" << maxLoan << endl;
	cout << "Owed to bank: " << owesBank << endl;
	cout.precision(3);
	cout << "Loan Rate: " << 100 * rate << "%" << endl;
	Restore(f);
}

void BrassPlus::Withdraw(double amt){
	Formatting f = SetFormat();
	double bal = Balance();
	if(amt <= bal)
		AcctABC::Withdraw(amt);
	else if(amt <= bal + maxLoan + owesBank){
		double advance = amt - bal;
		owesBank += advance * (1.0 + rate);
		cout << "Bank advance: $ " << advance  << endl;
		cout << "Finance charge: $ " << advance * rate << endl;
		Deposit(advance);
		AcctABC::Withdraw(amt);
	}else
		cout << "Credit limit exceeded.  Transaction canceled." << endl;
	Restore(f);
}
