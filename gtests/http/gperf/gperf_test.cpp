#include <cassert>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <gtest/gtest.h>

#include "server/http/gperf/HttpCommomHeadersHash.h"

using namespace std;

void _2()
{
    (void)http::wordlist;
}

//test run folder should be in unp folder
TEST(gperf, test_all)
{
    const char* fileName = "server/http/gperf/HttpCommomHeaders.txt";
    ifstream ifs{fileName, ifs.in};
    if(!ifs.is_open())
    {
        cout << "can't open file: " << fileName << endl;
        return;
    }

    char* line = static_cast<char*>(::calloc(64, 1));

    int count = 0;
    while(true)
    {
        if(ifs.peek() == EOF) break;

        ifs.getline(line, 64);
        auto ret = http::HttpCommomHeaderInternal::isValidHttpHeader(line, strlen(line));
        ASSERT_TRUE(ret != nullptr);
        ASSERT_EQ(strcmp(line, ret->headerName), 0);
        // cout << ret->headerName << " OK..." << endl;
        memset(line, 0, 64);
        count++;
    }
    // cout << "count: " << count << endl;
}

TEST(gperf, test_some_cases)
{
    const char* a1 = ":Authority";
    auto ret = http::HttpCommomHeaderInternal::isValidHttpHeader(a1, strlen(a1));
    // cout << "header name: " 
    //     << ret->headerName 
    //     << " header code: " 
    //     << static_cast<uint8_t>(ret->headerCode) << endl;
    ASSERT_TRUE(ret != 0);
    ASSERT_TRUE(strcmp(ret->headerName, a1) == 0);
    ASSERT_EQ(ret->headerCode, http::HttpHeaderCode::HTTP_HEADER_COLON_AUTHORITY);

    a1 = "";
    ret = http::HttpCommomHeaderInternal::isValidHttpHeader(a1, strlen(a1));
    ASSERT_TRUE(ret == 0);

    a1 = "Content-Encoding";
    ret = http::HttpCommomHeaderInternal::isValidHttpHeader(a1, strlen(a1));
    ASSERT_TRUE(ret != 0);
    ASSERT_EQ(strcmp(ret->headerName, a1), 0);
    ASSERT_EQ(ret->headerCode, http::HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING);

    a1 = "none";
    ret = http::HttpCommomHeaderInternal::isValidHttpHeader(a1, strlen(a1));
    ASSERT_TRUE(ret == 0);
}