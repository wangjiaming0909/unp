#ifndef FILEUTIL_TEST_H
#define FILEUTIL_TEST_H
#include "../server/config/ServerConfig.h"
#include "./tests_macros.h"
#include <typeinfo>

namespace TEST {

class FILEUTIL_TEST
{
public:
    FILEUTIL_TEST();
    static void test(){
        FILEUTIL_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.read_to_string();
    }

public:
    void read_to_string();
private:
    config::FileUtil* 	m_file_util_ptr;
};

}
#endif // FILEUTIL_TEST_H
