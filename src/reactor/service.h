#pragma once
#include "boost/intrusive/list_hook.hpp"
namespace reactor
{
using IntrusiveListBaseHook = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
struct ServiceT : public IntrusiveListBaseHook
{
  virtual ~ServiceT() = default;
  virtual int open() = 0;
  virtual int close() = 0;
};

}
