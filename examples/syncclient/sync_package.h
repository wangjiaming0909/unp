#pragma once
#include <stdint.h>
#include <memory>

namespace filesync
{


enum class PackageType : uint8_t
{
  Client = 1, Server = 2
};

enum class Command : uint8_t
{
  ClientHello = 1, 
  ServerHello = 2, 
  DepositeFile = 3,
  ResportState = 4,
};

struct Header
{
  PackageType type;
  Command command;
  uint32_t headerContentLen;
  const void* headContent;
};

struct DepositeFileHeader
{
  uint32_t fileNameLen;
  void* fileName;
  uint64_t fileLen;
  uint64_t curSeqStart;
  uint64_t curSeqEnd;
};

enum class DepositeState : uint8_t
{
  Succeed, Failed, Paused
};

struct SyncPackage 
{
  Header header;
  uint64_t contentLen;
  const void* content;
};

using SyncPackagePtr = std::shared_ptr<SyncPackage>;

SyncPackagePtr getClientHelloPackage(const char* helloContent);
SyncPackagePtr getServerHelloPackage(const char* helloContent);
SyncPackagePtr getDepositeFilePackage(const char* fileName, uint64_t fileLen, uint64_t from, uint64_t to, void* data);
SyncPackagePtr getReportStatePackage(DepositeState state);

}
