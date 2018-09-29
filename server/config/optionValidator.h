


#ifndef _H_OPTIONVALIDATOR_
#define _H_OPTIONVALIDATOR_

#include <utility>
#include <string>

namespace config{

class OptionValidator{
public:
    typedef std::pair<std::string, std::string> option;
    static option validateAndReturn(const option& inputOption);
};

}
#endif