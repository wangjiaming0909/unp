#ifndef CONFIGOPTION_H
#define CONFIGOPTION_H

#include "util/XString.h"
namespace config{

template <typename T>
class ConfigOption
{
public:
    ConfigOption(const util::string& name, const T& value);
    T getValue(const util::string& name);
private:
    util::string    m_name;
    T               m_value;
};
}
#endif // CONFIGOPTION_H
