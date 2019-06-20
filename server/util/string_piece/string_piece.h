#ifndef _STRING_PIECE_H_
#define _STRING_PIECE_H_

#include <string>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <ostream>
#include "server/util/substring_search/KMP.h"
#include "server/util/type_traits.h"

namespace string_piece
{

struct AsciiCaseSensitiveEqual
{
    bool operator()(char lhs, char rhs) const
    {
        return lhs == rhs;
    }
};

struct AsciiCaseInsensitiveEqual
{
    bool operator()(char lhs, char rhs) const
    {
        char k = lhs ^ rhs;
        if (k == 0) {
        return true;
        }
        if (k != 32) {
        return false;
        }
        k = lhs | rhs;
        return (k >= 'a' && k <= 'z');
    }
};

template <typename Iter>
class Range
{
public:
    using it_traits = std::iterator_traits<Iter>;
    using value_type = typename it_traits::value_type;
    using pointer = typename it_traits::pointer;
    using difference_type = typename it_traits::difference_type;
    using reference = typename it_traits::reference;

    using iterator = Iter;
    using const_iterator = std::add_const_t<iterator>;
    using size_type = std::size_t;
public:
    Range() : begin_(), end_(){}
    Range(iterator begin, iterator end) : begin_(begin), end_(end){}
    Range(iterator begin, size_type size) : begin_(begin), end_(begin + size){}

    //应该只在Iter时包含const时才能调用此构造函数
    template <typename T = Iter, typename recipes::IsCharPointer<T>::const_type = 0>
    Range(const std::string &str) : begin_(str.data()), end_(str.data() + str.size()) {}
    Range(iterator begin) : begin_(begin), end_(begin_ + ::strlen(begin_)){}

    Range(const Range&) = default;
    Range(Range&&) = default;
    Range &operator=(const Range &range) = default;

    bool operator==(const Range& range) const
    {
        if(size() != range.size()) return false;
        return strncmp(cbegin(), range.cbegin(), size()) == 0;
    }

public:
    iterator begin() {return begin_;}
    const_iterator cbegin() const {return begin_;}
    iterator end() {return end_;}
    const_iterator cend() const {return end_;}

    size_type find(value_type value)
    {
        auto it = std::find(begin(), end(), value);
        return it == end_ ? std::string::npos : (it - begin_);
    }

    template <typename T = Iter>
    int find(const Range<T>& str)
    {
        return substring_search::kmp_search(cbegin(), size(), str.cbegin(), str.size());
    }

    Range<iterator> sub_string(size_type first, size_type len = std::string::npos)
    {
        if(first > size())
        {
            throw std::out_of_range("index out of range");
        }
        return Range(begin_ + first, std::min(len, size() - first));
    }

    bool caseInsensitiveEqual(recipes::AddLowConst_t<Iter> str)
    {
        Range<recipes::AddLowConst_t<Iter>> str_range{str};
        if(str_range.size() != size()) return false;
        return std::equal(cbegin(), cend(), str_range.cbegin(), AsciiCaseInsensitiveEqual());
    }

    constexpr size_type size() const 
    {
        return size_type(end_ - begin_);
    }

    //allocate new memory for copying, remember to free it
    recipes::RemoveLowConst_t<Iter> copy()
    {
        using address_t = recipes::RemoveLowConst_t<Iter>;
        if (size() == 0)
            return 0;

        address_t data = static_cast<address_t>(::calloc(size() + 1, 1));
        ::memcpy(data, cbegin(), size());
        return data;
    }

    template <typename T>
    friend std::ostream &operator<<(std::ostream &os, Range<T> str);

private:
    iterator begin_;
    iterator end_;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, Range<T> str)
{
    return os << str.begin_;
}

using mutable_string_piece = Range<char*>;
using const_string_piece = Range<const char*>;

template <typename T, typename U>
struct CaseInSensitiveEqual
{
    bool operator()(const T& lhs, const U& rhs)
    {
        if(lhs.size() != rhs.size()) return false;
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
    }
    bool operator()(const U& lhs, const T& rhs)
    {
        if(lhs.size() != rhs.size()) return false;
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
    }
};

template <typename T>
struct CaseInSensitiveEqual<T, T>
{
    bool operator()(const T& lhs, const T& rhs)
    {
        if(lhs.size() != rhs.size()) return false;
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
    }
};


template <typename T>
struct CaseInSensitiveEqual<T, const char*>
{
    bool operator()(const T& lhs, const char* rhs)
    {
        auto rhs_T = T(rhs);
        return CaseInSensitiveEqual<T, T>()(lhs, rhs_T);
    }
    bool operator()(const char* lhs, const T& rhs)
    {
        auto lhs_T = T(lhs);
        return CaseInSensitiveEqual<T, T>()(lhs_T, rhs);
    }
};

template <typename T>
struct CaseInSensitiveEqual<const char*, T>
{
    bool operator()(const T& lhs, const char* rhs)
    {
        auto rhs_T = T(rhs);
        return CaseInSensitiveEqual<T, T>()(lhs, rhs_T);
    }
    bool operator()(const char* lhs, const T& rhs)
    {
        auto lhs_T = T(lhs);
        return CaseInSensitiveEqual<T, T>()(lhs_T, rhs);
    }
};

template <typename T>
struct CaseInSensitiveEqual<T, char*>
{
    bool operator()(const T& lhs, const char* rhs)
    {
        auto rhs_T = T(rhs);
        return CaseInSensitiveEqual<T, T>()(lhs, rhs_T);
    }
    bool operator()(const char* lhs, const T& rhs)
    {
        auto lhs_T = T(lhs);
        return CaseInSensitiveEqual<T, T>()(lhs_T, rhs);
    }
};

template <typename T>
struct CaseInSensitiveEqual<char*, T>
{
    bool operator()(const T& lhs, const char* rhs)
    {
        auto rhs_T = T(rhs);
        return CaseInSensitiveEqual<T, T>()(lhs, rhs_T);
    }
    bool operator()(const char* lhs, const T& rhs)
    {
        auto lhs_T = T(lhs);
        return CaseInSensitiveEqual<T, T>()(lhs_T, rhs);
    }
};

// template <typename T, typename U>
// bool CaseInSensitiveEqual(const T&lhs, const U&rhs)
// {
//     if(lhs.size() != rhs.size()) return false;
//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
// }

// template <typename T> //std::string or range
// inline bool CaseInSensitiveEqual(const T& lhs, const T&rhs)
// {
//     if(lhs.size() != rhs.size()) return false;
//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
// }

// template <typename T> //std::string or range
// inline bool CaseSensitiveEqual(const T& lhs, const T& rhs)
// {
//     if(lhs.size() != rhs.size()) return false;
//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseSensitiveEqual());
// }

// template <typename Iter>
// inline bool CaseInsensitiveEqual(const Range<Iter>& lhs, const Range<Iter>& rhs)
// {
//     if(lhs.size() != rhs.size()) return false;

//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
// }

// inline bool CaseInsensitiveEqual(const std::string& lhs, const std::string& rhs)
// {
//     if(lhs.size() != rhs.size()) return false;

//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseInsensitiveEqual());
// }

// template <typename Iter>
// inline bool CaseSensitiveEqual(const Range<Iter>& lhs, const Range<Iter>& rhs)
// {
//     if(lhs.size() != rhs.size()) return false;

//     return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), AsciiCaseSensitiveEqual());
// }

// inline bool CaseSensitiveEqual(const std::string& lhs, const std::string& rhs)
// {
//     if(lhs.size() != rhs.size()) return false;

//     return std::equal(lhs.cbegin(), )
// }


}
#endif //_STRING_PIECE_H_

