#ifndef _HTTP_HEADER_H_
#define _HTTP_HEADER_H_

#include <vector>
#include <string>
// #include <functional>
#include "HttpCommomHeaders.h"
#include "server/util/string_piece/string_piece.h"

/*
    HttpHeaders store http headers inside, 
    it have 
        1, header codes vector, 
        2, header names pointers
            if it's commom header, it points to the entry of commom header table
            if it's not the commom headers, dynamic memory is used
        3, header values vector

    how to find the the pos with name or code?
        first get code if you have name,
        then using memchr to find the code in codes_.data(), cause code only have 8 bit
        and the complexity is said to be n/16
 */

namespace http
{
class HttpHeaders
{
    template <typename T>
    using vector_t = std::vector<T>;
    using code_t = HttpHeaderCode;
    using mutable_string_piece = string_piece::mutable_string_piece;
    using const_string_piece = string_piece::const_string_piece;
    // using func_type = std::function<void(const vector_t<code_t>&, const vector_t<const char*>&, const vector_t<std::string>&, size_t&, size_t)>;
public:
    HttpHeaders();
    HttpHeaders(const HttpHeaders& other);
    HttpHeaders(HttpHeaders&& other);
    ~HttpHeaders();
    HttpHeaders& operator=(const HttpHeaders& other);
    HttpHeaders& operator=(HttpHeaders&& other);

public:
    size_t size() const {return codes_.size() - codes_deleted_;}
    void add(const_string_piece headerName, const_string_piece headerValue);
    void add(const std::string& headerName, const std::string& headerValue);
    void add(const std::string& headerName, std::string&& headerValue);
    void add(HttpHeaderCode code, std::string&& headerValue);

    bool removeWithStdString(const std::string& headerName);
    bool remove(const_string_piece headerName);
    bool remove(HttpHeaderCode code);


private:
    void disposeHeaderNames();
    void clearAll();
    template <typename Func>
    //there could be duplicated headers in codes, so iterate over all of them
    void iterateOverCodes(HttpHeaderCode code, Func&& func)
    {
        const code_t* ptr = codes_.data();
        while(ptr)
        {
            ptr = (code_t*)::memchr(ptr, static_cast<int>(code), codes_.size() - (ptr - codes_.data()));
            if(ptr == nullptr) break;
            const size_t pos = ptr - codes_.data();
            func(codes_, headerNames_, headerValues_, codes_deleted_, pos);
            ptr++;
        }
    }

    template <typename Func>
    void iterateOverOtherHeaderNames(const_string_piece headerName, Func&& func)
    {
        iterateOverCodes(HttpHeaderCode::HTTP_HEADER_OTHER, 
            [&](
                std::vector<HttpHeaderCode>& codes,
                std::vector<const char*>& headerNames,
                std::vector<std::string>& headerValues,
                size_t& codes_deleted, 
                size_t pos)
            {
                if(headerName.caseInsensitiveEqual(headerNames[pos]))
                {
                    func(codes, headerNames, headerValues, codes_deleted, pos);
                }
            }
        );

    }
private:
    static const size_t INIT_VECTOR_RESERVE_SIZE; 

private:
    vector_t<HttpHeaderCode>            codes_;
    vector_t<const char*>               headerNames_;
    vector_t<std::string>               headerValues_;
    size_t                              codes_deleted_;

#ifdef TESTING
public:
    const vector_t<HttpHeaderCode>& getCodes() const {return codes_;}
    const vector_t<const char*>& getHeaderNames() const {return headerNames_;}
    const vector_t<std::string>& getHeaderValues() const {return headerValues_;}
    size_t getCodesDeleted() const {return codes_deleted_;}

    static size_t getInitVectorReserveSize() {return INIT_VECTOR_RESERVE_SIZE;}
#endif //TESTING
};

#define CHECK_SIZE(size) \
    if ((size) == 0)     \
        return;

char *copyFrom(string_piece::const_string_piece str);
char *copyFrom(const std::string &str);
}
#endif // _HTTP_HEADER_H_