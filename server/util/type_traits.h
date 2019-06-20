#ifndef _TYPES_TRAITS_H___
#define _TYPES_TRAITS_H___

namespace recipes
{
template <class T>
struct IsCharPointer {};

template <>
struct IsCharPointer<char*> {
  typedef int type;
};

template <>
struct IsCharPointer<const char*> {
  typedef int const_type;
  typedef int type;
};

template <typename T>
struct RemoveLowConst { };

template <typename T>
struct RemoveLowConst<T*>
{
    using type = T*;
};

template <typename T>
struct RemoveLowConst<const T*>
{
    using type = T *;
};

template <typename T>
using RemoveLowConst_t = typename RemoveLowConst<T>::type;

template <typename T>
struct AddLowConst { };

template <typename T>
struct AddLowConst<T*>
{
    using type = const T*;
};

template <typename T>
struct AddLowConst<const T*>
{
    using type = const T*;
};

template <typename T>
using AddLowConst_t = typename AddLowConst<T>::type;



}//recipes


#endif // _TYPES_TRAITS_H___