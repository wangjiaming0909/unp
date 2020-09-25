#ifndef _H_OPTIONVALIDATOR_
#define _H_OPTIONVALIDATOR_

#include <utility>
#include <string>
using std::string;

namespace config{

class OptionValidator{
public:
  using option = std::pair<string, string>;
  static option validateAndReturn(const option& inputOption);
};

}
#endif
