/*
 * 2.cc
 *
 *  Created on: Jun 28, 2017
 *      Author: www
 */

#include <iostream>
#include <string>
#include <fstream>

struct free_throws{
	std::string name;
	int made;
	int attempts;
	float percent;
	std::ofstream fout;
};

void display(const free_throws &ft);
void set_pc(free_throws &ft);
free_throws& accumulate(free_throws &target, const free_throws &source);

int main02(){
	free_throws one = {"Ifelsa Branch", 13, 14};
	free_throws two = {"Andro knott", 7, 9};
	free_throws three = {"minnie Nax", 10, 16};
	free_throws four = {"Whihy Looper", 5, 9};
	free_throws five = {"Throwgoods", 6, 14};
	free_throws term = {"Long long", 0, 0};

	free_throws dup;

	set_pc(one);
	display(one);
	accumulate(term, one);
	display(term);
	display(accumulate(term, two));
	accumulate(accumulate(term, three), four);
	display(term);
//	dup = accumulate(term, five);
	std::cout << "Displaying term: \n";
	display(term);
//	accumulate(dup, five) = four;
	display(four);
	display(dup);
}

void display(const free_throws &ft){
	using namespace std;
	cout << "Name: " << ft.name << endl;
	cout << "  Made: " << ft.made << '\t';
	cout << "Atteempts: " << ft.attempts << '\t';
	cout << "Percent: " << ft.percent << endl;
}

void set_pc(free_throws &ft){
	if(ft.attempts != 0)
		ft.percent = 100.0f * float(ft.made) / float(ft.attempts);
	else
		ft.percent = 0;
}

free_throws& accumulate(free_throws &target, const free_throws &source){
	target.attempts += source.attempts;
	target.made += source.made;
	set_pc(target);
	return target;
}
