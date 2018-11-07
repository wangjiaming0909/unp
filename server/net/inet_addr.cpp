/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   inet_addr.cpp
 * Author: JWang284
 * 
 * Created on November 7, 2018, 9:14 PM
 */

#include <cstring>
#include <cerrno>

#include "inet_addr.h"

using namespace net;
inet_addr::inet_addr() : addr(AF_INET, sizeof(in4_)){ reset_addr(); }

//inet_addr::inet_addr(const inet_addr& orig) { }

inet_addr::inet_addr(host_byte_order_port port,
		const char* host_name,
		int address_family) 
	: addr(AF_INET, sizeof(in4_)){
	
}

int inet_addr::set(host_byte_order_port port, const in_addr inet_addr){
	reset_addr();
	set_port_number(port);
	return set_addr(&inet_addr, sizeof(inet_addr));
}

int inet_addr::set_addr(const in_addr* addr, int len){
	if(len != 4){
		errno = EINVAL;
		return -1;
	}
	uint32_t net_byte_order_addr = htonl(addr->s_addr);
	in4_.sin_family = AF_INET;
	in4_.sin_addr.s_addr = net_byte_order_addr;
	return 0;
}

void inet_addr::set_port_number(host_byte_order_port port){
	port = htons(port);
	in4_.sin_port = port;
}

int inet_addr::addr_to_string(char* buffer, size_t size){

}

int inet_addr::string_to_addr(const char* addr_port){

}

inline void inet_addr::reset_addr(){
	memset(&in4_, 0, sizeof(in4_));
	in4_.sin_family = AF_INET;
}

inet_addr::~inet_addr() {
}

