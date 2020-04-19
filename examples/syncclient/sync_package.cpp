#include "sync_package.h"
#include <cstdlib>
#include <memory>
#include <cstring>
#include <cstdio>

namespace filesync
{

SyncPackagePtr getClientHelloPackage(const char* helloContent)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Client;
  header.command = Command::ClientHello;
  header.headerContentLen = 0;
  header.headerContent = nullptr;
  p->contentLen = strlen(helloContent);
  p->content = ::calloc(strlen(helloContent), 1);
  std::memcpy(p->content, helloContent, p->contentLen);
  p->data_need_free = true;
  return p;
}

SyncPackagePtr getServerHelloPackage(const char* helloContent)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Server;
  header.command = Command::ServerHello;
  header.headerContentLen = 0;
  header.headerContent = nullptr;
  p->contentLen = strlen(helloContent);
  p->content = ::calloc(strlen(helloContent), 1);
  std::memcpy(p->content, helloContent, p->contentLen);
  p->data_need_free = true;
  return p;
}

SyncPackagePtr getDepositeFilePackage(const char* fileName, uint64_t fileLen, uint64_t from, uint64_t to, void* data, bool data_can_persist)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;
  header.type = PackageType::Client;
  header.command = Command::DepositeFile;
  DepositeFileHeader* headerContent = (DepositeFileHeader*)::calloc(sizeof(DepositeFileHeader), 1);
  headerContent->fileName = fileName;
  headerContent->fileNameLen = strlen(fileName);
  headerContent->fileLen = fileLen;
  headerContent->curSeqStart = from;
  headerContent->curSeqEnd = to;
  header.headerContentLen = sizeof(DepositeFileHeader);
  header.headerContent = headerContent;
  p->contentLen = to - from + 1;
  if(data_can_persist) {
    p->content = data;
  } else {
    p->content = ::calloc(p->contentLen, 1);
    std::memcpy(p->content, data, p->contentLen);
  }
  p->data_need_free = !data_can_persist;
  return p;
}

SyncPackagePtr getReportStatePackage(DepositeState state)
{
  SyncPackagePtr p = std::make_shared<SyncPackage>();
  auto& header = p->header;

  return p;
}

}
