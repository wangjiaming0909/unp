#ifndef _NET_ADDR_H_
#define _NET_ADDR_H_

namespace net {
class addr{
public:
    addr(int type = -1, int size = -1);
    virtual ~addr(void);
    int get_size() const;
    void set_size(int size);
    int get_type()const;
    void set_type(int type);

    static const addr addr_any;
    bool operator==(const addr& sap) const{
        return (sap.addr_size_ == this->addr_size_ && sap.addr_type_ == this->addr_type_); }
    bool operator!=(const addr& sap) const{
        return !this->operator==(sap);
    }
    // virtual void* get_addr() const;
    // virtual void set_addr(const void*, int len);

private:
    // AF_UNIX, AF_INET, AF_SPIPE
    int addr_type_;
    //numbers of bytes in the address
    int addr_size_;
};

}


#endif //_NET_ADDR_H_
