/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   inet_addr.h
 * Author: JWang284
 *
 * Created on November 7, 2018, 9:14 PM
 */

#ifndef INET_ADDR_H
#define INET_ADDR_H
#include "addr.h"
#include <netinet/in.h>

namespace net{
	
class inet_addr : public addr{
public:
	using host_byte_order_port = u_short;
    inet_addr();
//    inet_addr(const inet_addr& orig);
//	inet_addr(const sockaddr_in *addr, int len);
	inet_addr(host_byte_order_port port,
		const char* host_name,
		int address_family = AF_INET);
	int set(host_byte_order_port port,
		const char* host_name,
		int address_family = AF_UNSPEC);
	int set(host_byte_order_port port, const in_addr inet_addr);
	int set_addr(const in_addr* addr, int len);
	void set_port_number(host_byte_order_port port);
//	int set_address(const void* addr, int len);
	int addr_to_string(char* buffer, size_t size);
	int string_to_addr(const char* addr_port);

    virtual ~inet_addr();

private:
	void reset_addr(void);
private:
	sockaddr_in in4_;
};
}

#endif /* INET_ADDR_H */

