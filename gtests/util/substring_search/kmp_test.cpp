#include "gtest/gtest.h"
#include "util/substring_search/KMP.h"
#include "util/timer.h"
#include <random>

namespace kmp_test
{
void print_search_table(const std::vector<int>& search_table)
{
    for(auto i : search_table)
    {
        std::cout << i;
    }
    std::cout << std::endl;
}

using build_search_table_callback = std::shared_ptr<std::vector<int>> (*)(const char*, size_t);

void validate(build_search_table_callback callback, const char* pattern, const std::vector<int>& expectedTable)
{
    size_t len = strlen(pattern);
    auto table = callback(pattern, len);
    // std::cout << "-";
    // std::cout << pattern << std::endl;
    // print_search_table(*table);

    ASSERT_TRUE(*table == expectedTable);
}

void test_build_search_table(build_search_table_callback build_search_table_call)
{
    std::vector<int> expectedTable = {-1, 0, 0, 1, 2, 3, 4, 5};
    validate(build_search_table_call, "ABABABAB", expectedTable);

    expectedTable.clear();
    expectedTable = {-1, 0, 0, 0, 0, 0, 0};
    validate(build_search_table_call, "ABCDEFG", expectedTable);

    expectedTable.clear();
    expectedTable = {-1, 0, 0, 0, 0, 1, 2, 3, 4, 0};
    validate(build_search_table_call, "chinchinla", expectedTable);

    expectedTable.clear();
    expectedTable = {-1, 0, 0, 1, 0, 0, 0, 1, 2, 3, 4};
    validate(build_search_table_call, "abaabbabaab", expectedTable);

    expectedTable.clear();
    expectedTable = {-1, 0, 0, 1, 2};
    validate(build_search_table_call, "ababb", expectedTable);
}

void bench_mark_test(const char* pattern)
{
    size_t len = strlen(pattern);
    {
        utils::timer _{"no while"};
        auto table = substring_search::build_search_table(pattern, len);
        // print_search_table(*table);
    }
    {
        utils::timer _{"with while"};
        auto table = substring_search::build_search_table(pattern, len);
        // print_search_table(*table);
    }
}

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(33, 34);

template <unsigned int N>
struct SizableClass_WithChar{
    SizableClass_WithChar()
    {
        for(uint32_t i = 0; i < N; i++)
        {
            buffer_[i] = dis(gen);
        }
    }
    char buffer_[N];
};

TEST(kmp, test_kmp)
{
    {
        utils::timer _{"faster version of build search table"};
        for(int i = 0; i < 10; i++)
        {
            test_build_search_table(substring_search::build_search_table);
        }
    }

    {
        utils::timer _{"slower version of build search table"};
        for(int i = 0; i < 10; i++)
        {
            test_build_search_table(substring_search::build_search_table);
        }
    }


    const char* pattern = "ababshsbababaskjdbabababbabbabbabkljababbabaldjlwbababbabbabaklhsdbabbababbakhsdwbabbabababbasbdabbab";
    bench_mark_test(pattern);

    SizableClass_WithChar<4096> random_data{};
    bench_mark_test(random_data.buffer_);
}

TEST(kmp, test_kmp_search)
{
    const char* text = "ababshsbababaskjdbabababbabbabbabkljababbabaldjlwbababbabbabaklhsdbabbababbakhsdwbabbabababbasbdabbab";
    const char* pattern = "ababb";
    size_t index = substring_search::kmp_search(text, pattern);
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 20);

    text = "aabraacadabraacaadabra";
    pattern = "aacaa";
    index = substring_search::kmp_search(text, pattern);
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 12);

    text = "http://wangjiaming0909/unp";
    pattern = "http";
    index = substring_search::kmp_search(text, pattern);
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 0);

    pattern = "://";
    index = substring_search::kmp_search(text, pattern);
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 4);


}

TEST(kmp, test_kmp_search_without_0_end)
{
    const char* text = "ababshsbababaskjdbabababbabbabbabkljababbabaldjlwbababbabbabaklhsdbabbababbakhsdwbabbabababbasbdabbab";
    const char* pattern = "ababb";
    size_t index = substring_search::kmp_search(text, strlen(text), pattern, strlen(pattern));
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 20);

    text = "aabraacadabraacaadabra";
    pattern = "aacaa";
    index = substring_search::kmp_search(text, strlen(text), pattern, strlen(pattern));
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 12);

    text = "http://wangjiaming0909/unp";
    pattern = "http";
    index = substring_search::kmp_search(text, strlen(text), pattern, strlen(pattern));
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 0);

    pattern = "://";
    index = substring_search::kmp_search(text, strlen(text), pattern, strlen(pattern));
    std::cout << "index: " << index << std::endl;
    ASSERT_TRUE(index == 4);
}

}
