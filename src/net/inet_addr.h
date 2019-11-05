#ifndef INET_ADDR_H
#define INET_ADDR_H
#include "addr.h"
#include <netinet/in.h>
#include "util/XString.h"
#include <memory>
#include <boost/scoped_array.hpp>

namespace net{
int string_to_addr(const char* addr_port, sockaddr_in *ip4_addr, int addr_family = AF_INET);
int addr_to_string(char* buffer, size_t size, const sockaddr_in* ip4_addr , int addr_family = AF_INET);

class inet_addr : public addr{
public:
	using host_byte_order_port = in_port_t;
    inet_addr();
//    inet_addr(const inet_addr& orig);
//	inet_addr(const sockaddr_in *addr, int len);
	inet_addr(host_byte_order_port port,
		const char* host_name,
		int address_family = AF_INET);
	inet_addr(const inet_addr& rl);
	inet_addr(const sockaddr_in& sockAddr);
	inet_addr& operator=(const inet_addr& rl);
	int set(host_byte_order_port port,
		const char* host_name,
		int address_family = AF_UNSPEC);
	int set(host_byte_order_port port, const in_addr* inet_addr);
	int set_address(const in_addr* addr, int len);
	int set_address(const char* address);
	int set_addr(const char* addr_port);
	sockaddr_in get_sockaddr_in() const;
	std::shared_ptr<sockaddr_in> get_sockaddr_in_ptr()const;
	std::shared_ptr<sockaddr> get_sockaddr_ptr() const;
	int set_port_number(host_byte_order_port port);
	ushort get_port_number()const{return ntohs(in4_.sin_port);}
	util::string get_address_string() const;
	void reset_addr(void);
	util::string to_string();
    virtual ~inet_addr();
	bool operator==(const inet_addr& addr) const {
		return memcmp(&in4_, &addr, sizeof in4_);
	}
	bool operator!=(const inet_addr& addr){
		return !this->operator==(addr);
	}

private:
	sockaddr_in in4_;
};
}

#endif /* INET_ADDR_H */
