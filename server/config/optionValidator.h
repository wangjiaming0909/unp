#ifndef _H_OPTIONVALIDATOR_
#define _H_OPTIONVALIDATOR_

#include <utility>
#include "server/util/XString.h"
using namespace util;

namespace config{

class OptionValidator{
public:
    typedef std::pair<string, string> option;
    static option validateAndReturn(const option& inputOption);
};

}
#endif