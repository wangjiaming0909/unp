#include "KMP.h"

namespace substring_search
{
std::shared_ptr<std::vector<int>>
build_search_table(const char* pattern, size_t len)
{
    assert(pattern != nullptr);
    std::shared_ptr<std::vector<int>> search_table
    = std::make_shared<std::vector<int>>();

    search_table->push_back(-1);

    bool isLastEqual = true;
    for (size_t i = 1; i < len; i++)
    {
        int back = search_table->back();
        if(isLastEqual)
            search_table->push_back(++back);
        else
            search_table->push_back(0);
        isLastEqual = pattern[i] == pattern[search_table->operator[](i)];
    }

    return search_table;
}

int kmp_search(const char* text, const char* pattern)
{
    if(text == nullptr || pattern == nullptr) return -1;
    size_t text_len = strlen(text);
    size_t pattern_len = strlen(pattern);
    if(text_len == 0 || pattern_len == 0) return -1;

    auto search_table = build_search_table(pattern, pattern_len);

    int pattern_len_int = pattern_len;
    int j = 0;
    for (size_t i = 0; i < text_len; i++)
    {
        char t = text[i];
        char p = pattern[j];
        if(t == p)
        {
            if(j == pattern_len_int - 1)
                return i - pattern_len_int + 1;
            j++;
            continue;
        }

        //回退J
        while(j != -1 && t != pattern[j])
        {
            j = search_table->operator[](j);
        }
        if(j == -1)
        {
            j = 0;
        }else
        {
            j++;
        }
    }
    return -1;
}

int kmp_search(const char *text, size_t text_len, const char *pattern, size_t pattern_len)
{
    if(text == nullptr || pattern == nullptr) return -1;
    if(text_len == 0 || pattern_len == 0) return -1;

    auto search_table = build_search_table(pattern, pattern_len);

    int pattern_len_int = pattern_len;
    int j = 0;
    for (size_t i = 0; i < text_len; i++)
    {
        char t = text[i];
        char p = pattern[j];
        if(t == p)
        {
            if(j == pattern_len_int - 1)
                return i - pattern_len_int + 1;
            j++;
            continue;
        }

        //回退J
        while(j != -1 && t != pattern[j])
        {
            j = search_table->operator[](j);
        }
        if(j == -1)
        {
            j = 0;
        }else
        {
            j++;
        }
    }
    return -1;

}

}