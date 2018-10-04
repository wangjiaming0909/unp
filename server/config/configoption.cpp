#include "configoption.h"

template <typename T>
config::ConfigOption<T>::ConfigOption(
    const util::string& name, const T& value)
    : m_name(name), m_value(value){}

template <typename T>
T config::ConfigOption<T>::getValue(const util::string& name){
    return m_value;
}
