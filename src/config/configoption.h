#ifndef CONFIGOPTION_H
#define CONFIGOPTION_H

#include <string>
namespace config{

template <typename T>
class ConfigOption
{
public:
    ConfigOption(const std::string& name, const T& value);
    T getValue(const std::string& name);
private:
    std::string    m_name;
    T               m_value;
};
}
#endif // CONFIGOPTION_H
