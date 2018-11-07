#include "addr.h"

using namespace net;

const addr addr::addr_any(-1, -1);

addr::addr(int type, int size)
	: addr_type_(type), addr_size_(size){ }

addr::~addr(){}

inline int addr::get_size() const{ return this->addr_size_; }
inline void addr::set_size(int size){ this->addr_size_ = size; }
inline int addr::get_type() const{ return this->addr_type_; }
inline void addr::set_type(int type){ this->addr_type_ = type; }





