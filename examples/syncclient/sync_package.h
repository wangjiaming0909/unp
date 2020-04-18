#pragma once
#include <stdint.h>

namespace filesync
{


enum class PackageType : uint8_t
{

};

enum class Command : uint8_t
{
  ClientHello = 1, 
  ServerHello = 2, 
  DepositeFile = 3,
  ResportState = 4,
};

struct SyncPackage 
{
  PackageType type;
  Command command;
  uint64_t contentLen;
  void* content;
};

}
