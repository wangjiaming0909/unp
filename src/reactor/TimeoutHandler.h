#ifndef _UNP_REACTOR_TIMEOUT_HANDLER_H_
#define _UNP_REACTOR_TIMEOUT_HANDLER_H_

#include "reactor/EventHandler.h"
#include "boost/intrusive/list_hook.hpp"
#include "boost/intrusive/list_hook.hpp"

namespace reactor
{
class TimeoutHandler;
using boost_list_base_hook_t = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
class TimeoutHandler : public EventHandler, public boost_list_base_hook_t, public boost::noncopyable
{
public:
    TimeoutHandler() = default;
    TimeoutHandler(Reactor& reactor);
    virtual ~TimeoutHandler();
private:
    
};

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_