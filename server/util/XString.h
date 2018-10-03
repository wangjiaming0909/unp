#ifndef _STRING_H_NOT_STD_STRING
#define _STRING_H_NOT_STD_STRING
#include <string>
#include <ext/vstring.h>
#include <ext/vstring_fwd.h>
#include <string.h>
#include <iostream>

typedef __gnu_cxx::__sso_string sso_string;

#ifdef _GLIBCXX_IOSTREAM
#define CONSOLE_LOG(message) std::cout << message << std::endl;
#else
#define CONSOLE_LOG(message) ;
#endif

namespace util{
class string{
public:
    string(){
        m_capacity = 64 + 64/2;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        m_length = 0;
    }
    string(size_t size){
        m_ptr = new char[size + size/2];
        memset(m_ptr, 0, size);
        m_length = 0;
        m_capacity = size + size/2;
    }
    string(const char *ptr) {
        this->m_length = strlen(ptr);//TODO: thread safty
        m_capacity = m_length + m_length/2;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        memcpy(m_ptr, ptr, m_length);
    }
    string(const char *ptr, size_t size) {
        this->m_length = size;
        m_capacity = size + size/2;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        ::memcpy(m_ptr, ptr, size);
    }
    string(const unsigned char* str){
        string(reinterpret_cast<const char*>(str));
    }
    string(const string& s){
        this->m_capacity = s.m_capacity;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        this->m_length = s.m_length;
        //deep copy
        memcpy(m_ptr, s.m_ptr, s.m_length);
    }
    string(const std::string& s) {
        const char* ptr = s.c_str();
        m_length = s.length();
        m_capacity = m_length + m_length/2;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        memcpy(m_ptr, ptr, m_length);
    }
    string(const sso_string& s){
        const char* ptr = s.c_str();
        m_length = s.length();
        m_capacity = m_length + m_length/2;
        m_ptr = new char[m_capacity];
        memset(m_ptr, 0, m_capacity);
        memcpy(m_ptr, ptr, m_length);
    }
    ~string(){
        //TODO: thread safty
        if(!empty())
            delete [] m_ptr;
        m_ptr = nullptr;
    }

private:
    void reAllocate(size_t size){
        if(size == 0){
            m_length = 0;
            m_capacity = 64 + 64/2;//empty string's capacity should be this value
            m_ptr = new char[64];
        }
        else{
            if(empty()){//m_length == 0
                delete []m_ptr;
                m_ptr = new char[size + size/2];
                memset(m_ptr, 0, size + size/2);
                this->m_capacity = size + size/2;
            }else{
                string tmp = *this;//save the content of this
                delete []m_ptr;
                m_ptr = new char[size + size/2];
                memset(m_ptr, 0, size + size/2);
                this->m_capacity = size + size/2;
                memcpy(m_ptr, tmp.m_ptr, tmp.m_length);//only copy the previous value into this
                m_length = tmp.m_length;
            }
        }
    }

public:
    //the old memory is not my concern, it will managed by outside
    string& append(const string& str){
        size_t size = this->size() + str.size();
        //after append size is big than capacity
        if(size > this->m_capacity){//reallocate
            size_t tmpSize = m_length;
            reAllocate(size);//allocate size + size / 2 bytes
            memcpy(m_ptr + tmpSize, str.m_ptr, str.m_length);
            m_length += str.m_length;
        }else{//no need to reallocate
            memcpy(m_ptr+m_length, str.ptr(), str.size());
            m_length = size;
        }
       return *this;
    }
    string& append(const char*ptr){
        string tmp = ptr;
        return append(tmp);
    }
    string& append(const char* ptr, size_t len){
        string tmp(ptr, len);
        return append(tmp);
    }
    string& append(const std::string& str){
        string tmp = str;
        return append(tmp);
    }

    const char* ptr() const {return m_ptr;}
    size_t size() const {return m_length;}
    bool empty() const {
        return (m_length == 0);
    }
    const char* begin() const {return m_ptr;}
    const char* end() const {return m_ptr + m_length - 1;}
    size_t capacity() const{return this->m_capacity;}
    void clear() {
        memset(m_ptr, 0, m_length);
        m_length = 0;
    }
    char operator[](int i) const {return m_ptr[i];}
    bool operator==(const string& s) const {
        return ((m_length == s.m_length) && (strcmp(m_ptr, s.m_ptr) == 0));
    }
//    void set(const char* ptr) {
//        m_ptr = ptr;
//        m_length = static_cast<int>(strlen(m_ptr));
//    }
//    void set(const char* ptr, int len){
//        m_ptr = ptr; m_length = len;
//    }
    bool operator!= (const string& s)const{return !(*this == s);}
    sso_string as_sso_string() const{
        return sso_string(m_ptr, static_cast<size_t>(m_length));
    }
    std::string as_std_string() const {
        return std::string(m_ptr, static_cast<size_t>(m_length));
    }
#define STRING_BINARY_PREDICATE(cmp,auxcmp)                                         \
    bool operator cmp (const string& x) const {                                \
        int r = memcmp(m_ptr, x.m_ptr, m_length < x.m_length ? m_length : x.m_length);    \
        return ((r auxcmp 0) || ((r == 0) && (m_length cmp x.m_length)));             \
    }
    STRING_BINARY_PREDICATE(<,  <)
    STRING_BINARY_PREDICATE(<=, <)
    STRING_BINARY_PREDICATE(>=, >)
    STRING_BINARY_PREDICATE(>,  >)
#undef STRING_BINARY_PREDICATE

private:
    char*    	 	m_ptr;
    size_t          m_length = 0;
    size_t 			m_capacity = 0;
};

//std::ostream& operator<<(std::ostream& o, const string& str){
//    return o << str.as_sso_string();
//}

}
#endif // _STRING_H_NOT_STD_STRING