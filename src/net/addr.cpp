#include "addr.h"

using namespace net;

const addr addr::addr_any(-1, -1);

addr::addr(int type, int size)
	: addr_type_(type), addr_size_(size){ }

addr::~addr(){}

int addr::get_size() const{ return this->addr_size_; }
void addr::set_size(int size){ this->addr_size_ = size; }
int addr::get_type() const{ return this->addr_type_; }
void addr::set_type(int type){ this->addr_type_ = type; }





