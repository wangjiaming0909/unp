#ifndef _UNP_DEMULTIPLEX_TABLE_H_
#define _UNP_DEMULTIPLEX_TABLE_H_

#include "event_handler.h"
#include <limits>

namespace reactor
{
class event_handler;

class demultiplex_table{
public:
    using Event_Type = event_handler::Event_Type;
    struct demultiplex_element{
        event_handler *event_handler_;
        Event_Type event_type_;
    };
    demultiplex_element table_[sizeof(unsigned long int)];
private:
    
};
} // reactor



#endif // _UNP_DEMULTIPLEX_TABLE_H_