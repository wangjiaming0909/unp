#ifndef _CURRENT_THREAD_H_
#define _CURRENT_THREAD_H_

#include "../util/XString.h"
namespace thread{

template <typename THREAD_IMP, typename THREAD_ID_TYPE>
class current_thread{
public:
    
private:
    THREAD_ID_TYPE              m_thread_id;
    const util::string*         m_thread_name;
};
}

#endif