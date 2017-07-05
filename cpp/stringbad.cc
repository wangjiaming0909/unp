#include "stringbad.h"


using std::cout;

int StringBad::num_strings = 0;

StringBad::StringBad(const char *s){
    len = strlen(s);
    str = new char[len + 1];
    std::strcpy(str, s);
    num_strings++;
    cout << num_strings << ": \"" << str << "\"object created" << std::endl;
}

StringBad::StringBad(){
//    len = 4;
//    str = new char[4];
//    std::strcpy(str, "C++");
//    num_strings++;
//    cout << num_strings << ":\"" << str << "\"default object created\n";
	len = 0;
	str = new char[1];//不用str = new char， 为了使得在与析构函数兼容
	str[0] = '\0';
}

StringBad::StringBad(const StringBad &s){
    len = s.len;
    str = new char[len + 1];
    std::strcpy(str, s.str);
    num_strings++;
    cout << num_strings <<": \"" << str << "copy constructor" << std::endl;
}

StringBad::~StringBad(){
    cout << "\"" << str << "\"object deleted, ";
    --num_strings;
    cout << this->str << "--" << num_strings << " lefted" << std::endl;
    delete [] str;
}

StringBad& StringBad::operator=(const StringBad &s){
	if(this == &s)
		return *this;//因为返回时引用，所以不调用复制构造函数
	delete [] this->str;//删除原本内存重新分配
	len = s.len;
	str = new char[len + 1];
	std::strcpy(str, s.str);
	return *this;
}

StringBad& StringBad::operator=(const char *s){
	delete [] this->str;
	this->len = std::strlen(s) + 1;
	this->str = new char[this->len];
	std::strcpy(this->str, s);
	return *this;
}

std::ostream& operator<<(std::ostream &os, const StringBad &st){
    os << st.str;
    return os;
}

bool operator<(const StringBad &st, const StringBad &st2){
	if(std::strcmp(st.str, st2.str) < 0)
		return true;
	else
		return false;
}

bool operator>(const StringBad &st, const StringBad &st2){
	return (std::strcmp(st.str, st2.str) < 0);
}

bool operator==(const StringBad &st, const StringBad &st2){
	return (std::strcmp(st.str, st2.str) == 0);
}

std::istream& operator>>(std::istream &is, StringBad &st){
	char temp[StringBad::CINLIM];
	is.get(temp, StringBad::CINLIM);
	if(is)
		st = temp;
	while(is && is.get() != '\n')
		continue;
	return is;
}
char& StringBad::operator[](int i){
	return this->str[i];
}
const char& StringBad::operator[](int i)const{
	return this->str[i];
}

