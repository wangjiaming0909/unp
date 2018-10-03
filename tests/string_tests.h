#ifndef _XSTRING_TESTS_H_
#define _XSTRING_TESTS_H_

#include "tests_macros.h"
#include <typeinfo>

namespace TEST{

class XSTRING_TEST {
public:
    static void test(){
        XSTRING_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.string_empty_string();
        test.string_initialize_from_char_star();
        test.string_copy_constructor();
        test.string_initialize_with_size();
        test.string_initialize_with_char_and_size();
        test.string_initialize_from_unsigned_char_star();
        test.string_initialize_from_std_string();
        test.string_append_char_star_need_to_reallocate_memory();
        test.string_append_char_star_no_need_to_reallocate_memory();
        test.string_append_std_string();
        test.string_append_char_star_len();
        test.string_begin_and_end();
        test.string_operator_braket();
        test.string_operator_equal();
    }
public:
    void string_empty_string();
    void string_initialize_from_char_star();
    void string_copy_constructor();
    void string_initialize_with_size();
    void string_initialize_with_char_and_size();
    void string_initialize_from_unsigned_char_star();
    void string_initialize_from_std_string();
    void string_append_char_star_need_to_reallocate_memory();
    void string_append_char_star_no_need_to_reallocate_memory();
    void string_append_std_string();
    void string_append_char_star_len();
    void string_begin_and_end();
    void string_operator_braket();
    void string_operator_equal();
};
}
#endif // _XSTRING_TESTS_H_
