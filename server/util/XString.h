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
    string(): m_ptr(nullptr), m_length(0){}
    string(const char*ptr) : m_ptr(ptr), m_length(static_cast<int>(strlen(m_ptr))){}
    string(const unsigned char* str) : 
        m_ptr(reinterpret_cast<const char*>(str)),
        m_length(static_cast<int>(strlen(m_ptr))){}
    // string(const string& s);
    string(const std::string& s) : m_ptr(s.data()), m_length(static_cast<int>(s.size())){}
    string(const sso_string& s) : m_ptr(s.data()), m_length(static_cast<int>(s.size())){}
    string(const char* offset, int len) : m_ptr(offset), m_length(len){}
    ~string(){
        //TODO: thread safty
        if(this->m_if_self_allocated){
            if(m_ptr != nullptr)
            delete m_ptr;
            m_ptr = nullptr;
        }
    }

public:
    //the old memory is not my concern, it will managed by outside
    string& append(string& str){
        //TODO thread safty
        m_if_self_allocated = true;
        int size = this->size() + str.size();
        char *ptr = new char[static_cast<size_t>(size)];//this memory will managed by myself
        memcpy(ptr, m_ptr, static_cast<size_t>(m_length));
        memcpy(ptr + m_length, str.ptr(), static_cast<size_t>(str.m_length));
        this->m_ptr = ptr;
        this->m_length = size;
        return *this;
    }
    string& append(const char*ptr){
        string tmp = ptr;
        return append(tmp);
    }
    string& append(const char* ptr, int len){
        string tmp(ptr, len);
        return append(tmp);
    }
    string& append(const std::string& str){
        string tmp = str;
        return append(tmp);
    }
    const char* ptr() const {return m_ptr;}
    int size() const {return m_length;}
    bool empty() const {return m_length == 0;}
    const char* begin() const {return m_ptr;}
    const char* end() const {return m_ptr + m_length;}
    void clear() {m_ptr = nullptr; m_length = 0;}
    char operator[](int i) const {return m_ptr[i];}
    bool operator==(const string& s) const {
        return ((m_length == s.m_length) && (strcmp(m_ptr, s.m_ptr) == 0));
    }
    void set(const char* ptr) {
        m_ptr = ptr;
        m_length = static_cast<int>(strlen(m_ptr));
    }
    void set(const char* ptr, int len){
        m_ptr = ptr; m_length = len;
    }
    bool operator!= (const string& s)const{return !(*this == s);}
    sso_string as_sso_string() const{
        return sso_string(m_ptr, m_length);
    }
    std::string as_std_string() const {
        return std::string(m_ptr, m_length);
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
    const char*     m_ptr;
    int             m_length;
    bool 			m_if_self_allocated = false;
};

//std::ostream& operator<<(std::ostream& o, const string& str){
//    return o << str.as_sso_string();
//}

}
#endif // _STRING_H_NOT_STD_STRING
