#include "thread.h"
namespace thread{

template <typename THREAD_IMP, typename THREAD_ID_TYPE>
struct ThreadInfo{
    const util::string      thread_name;
    THREAD_ID_TYPE          thread_id;          
    ThreadInfo(const util::string& name) : thread_name(name){}
};
}