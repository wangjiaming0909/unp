#include "gtest/gtest.h"
#include "http/HttpMethod.h"

using namespace http;

TEST(httpmethod, stringToMethod)
{
    auto ret = stringToMethod("put");
    ASSERT_EQ(ret.get(), HTTPMethod::PUT);

    ret = stringToMethod("methodnotexisted");
    ASSERT_EQ(ret, boost::none);
}