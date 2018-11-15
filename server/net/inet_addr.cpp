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
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>
#include <boost/scoped_array.hpp>
#include <exception>

#include "inet_addr.h"

net::inet_addr::inet_addr() : addr(AF_INET, sizeof(in4_)){ reset_addr(); }

//host_name: 127.0.0.1 not ubuntu, www
net::inet_addr::inet_addr(host_byte_order_port port,
		 const char* host_name, int address_family) 
		 : addr(AF_INET, sizeof(in4_)){
	if(address_family != AF_INET)
		throw std::invalid_argument("address_family must be AF_INET...");
	reset_addr();
	if(set_port_number(port) != 0)
		throw std::invalid_argument("invalid argument: port...");
	if(set_address(host_name) != 0)
		throw std::invalid_argument("invalid argument: host_name...");
}

net::inet_addr::inet_addr(const inet_addr& rl) : addr(AF_INET, sizeof(in4_)), in4_(rl.in4_){ }

net::inet_addr::~inet_addr() { }

net::inet_addr& net::inet_addr::operator=(const inet_addr& rl){
	if(this == &rl) return *this;
	in4_ = rl.in4_;
	return *this;
}

int net::inet_addr::set(host_byte_order_port port, const in_addr* inet_addr){
	reset_addr();
	set_port_number(port);
	return set_address(inet_addr, sizeof(inet_addr));
}

int net::inet_addr::set_address(const in_addr* addr, int len){
	if(len != 4){
		errno = EINVAL;
		return -1;
	}
	uint32_t net_byte_order_addr = htonl(addr->s_addr);
	in4_.sin_family = AF_INET;
	in4_.sin_addr.s_addr = net_byte_order_addr;
	return 0;
}

int net::inet_addr::set_address(const char* address){
	int ret = inet_pton(AF_INET, address, &in4_.sin_addr);
	if(ret != 1) return -1;
	return 0;
}

int net::inet_addr::set_addr(const char* addr_port){
	reset_addr();
	//any parse error, it will return
	return net::string_to_addr(addr_port, &in4_, AF_INET);
}

sockaddr_in net::inet_addr::get_sockaddr_in() const{
	return in4_;
}

boost::shared_ptr<sockaddr_in> net::inet_addr::get_sockaddr_in_ptr()const{
	sockaddr_in* addr = new sockaddr_in();
	memcpy(addr, &in4_, sizeof in4_);
	boost::shared_ptr<sockaddr_in> ret_addr{addr};
	return ret_addr;
}

int net::inet_addr::set_port_number(host_byte_order_port port){
	port = htons(port);
	if(port > INT16_MAX){
		in4_.sin_port = htons(0);
		return -1;
	}
	in4_.sin_port = port;
	return 0;
}

inline void net::inet_addr::reset_addr(){
	memset(&in4_, 0, sizeof(in4_));
	in4_.sin_family = AF_INET;
}

boost::shared_ptr<util::string> net::inet_addr::get_address_string() const{
	boost::scoped_array<char> addr_str{new char[INET_ADDRSTRLEN]};
	memset(addr_str.get(), 0, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(in4_.sin_addr), addr_str.get(), sizeof(sockaddr_in));
	return boost::shared_ptr<util::string>(new util::string(addr_str.get()));
}

int net::addr_to_string(char* buffer, size_t len, const sockaddr_in* ip4_addr, int addr_family){
	boost::scoped_array<char> addr_str{new char[INET_ADDRSTRLEN]};
	memset(addr_str.get(), 0, INET_ADDRSTRLEN);
	const char* ret = inet_ntop(addr_family, &(ip4_addr->sin_addr), addr_str.get(), sizeof(sockaddr_in));
	if(ret == nullptr) return -1;
	ushort port_short = ntohs(ip4_addr->sin_port);
	snprintf(buffer, len, "%s:%d", addr_str.get(), port_short);
	return 0;
}


boost::shared_ptr<util::string> net::inet_addr::to_string(){
	boost::scoped_array<char> addr_str{new char[16]};
	int ret = addr_to_string(addr_str.get(), 16, &in4_, AF_INET);
	if(ret != 0) return nullptr;
	return boost::shared_ptr<util::string>(new util::string(addr_str.get()));
}

/// addr_port looks like 127.0.0.1:9900
int net::string_to_addr(const char* addr_port, sockaddr_in *ip4_addr, int addr_family ){
	ip4_addr->sin_family = addr_family;
//	char* addr_str = new char[32];
//	std::unique_ptr<char> addr_str{new char[32]);
	boost::scoped_array<char> addr_str{new char[32]};
	bzero(addr_str.get(), 32);
	memcpy(addr_str.get(), addr_port, strlen(addr_port));
	char* position = ::strrchr(addr_str.get(), ':');
	*position = '\0';
	int ret = inet_pton(addr_family, addr_str.get(), &ip4_addr->sin_addr);
	if(ret <= 0) return ret;
	int port_int_host_byte_order = atoi(position+1);
	if(port_int_host_byte_order > INT16_MAX){
		ip4_addr->sin_port = htons(0);
		return -1;
	}
	ip4_addr->sin_port = htons(port_int_host_byte_order);
	return 0;
}

