#include "sync_package.h"
#include <memory>
#include <cstring>

namespace filesync
{

SyncPackagePtr getClientHelloPackage(const char* helloContent)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Client;
  header.command = Command::ClientHello;
  header.headerContentLen = 0;
  header.headContent = nullptr;
  p->contentLen = strlen(helloContent);
  p->content = helloContent;
  return p;
}

SyncPackagePtr getServerHelloPackage(const char* helloContent)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Server;
  header.command = Command::ServerHello;
  header.headerContentLen = 0;
  header.headContent = nullptr;
  p->contentLen = strlen(helloContent);
  p->content = helloContent;
  return p;
}

SyncPackagePtr getDepositeFilePackage(const char* fileName, uint64_t fileLen, uint64_t from, uint64_t to, void* data)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Client;
  header.command = Command::DepositeFile;
  //header.headContent = 
  return p;
}

SyncPackagePtr getReportStatePackage(DepositeState state)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;

  return p;
}

}
