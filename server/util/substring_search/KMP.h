#ifndef _RECIPES_KMP_H_
#define _RECIPES_KMP_H_
#include <cstdint>
#include <vector>
#include <memory>
#include <cassert>
#include <cstring>
#include <iostream>

namespace substring_search
{
std::shared_ptr<std::vector<int>>
build_search_table(const char *pattern, size_t len);

int kmp_search(const char *text, const char *pattern);

int kmp_search(const char *text, size_t text_len, const char *pattern, size_t pattern_len);

}
#endif




