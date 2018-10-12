#include "thread.h"

struct ThreadInfo{
    const util::string thread_name;
    ThreadInfo(const util::string& name) : thread_name(name){}
};

thread::Thread::Thread()
{

}
