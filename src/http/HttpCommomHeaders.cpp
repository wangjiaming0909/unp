#include <cassert>
#include <cctype>
#include "HttpCommomHeaders.h"
#include "http/gperf/HttpCommomHeadersHash.h"

namespace http{

HttpHeaderCode HttpCommomHeaders::getHeaderCode(const char* headerName, size_t len)
{
  const HttpHeader* header = HttpCommomHeaderInternal::isValidHttpHeader(headerName, len);
  if (header == nullptr) {
    std::string header_name = headerName;
    header_name[0] = std::toupper(header_name[0]);
    header = HttpCommomHeaderInternal::isValidHttpHeader(header_name.c_str(), len);
  }
  return header == nullptr ? HttpHeaderCode::HTTP_HEADER_OTHER : header->headerCode;
}

HttpHeaderCode HttpCommomHeaders::getHeaderCode(const std::string& headerName)
{
    return getHeaderCode(headerName.data(), headerName.length());
}

const std::vector<std::string*>& HttpCommomHeaders::getHeaderTable()
{
    static const std::vector<std::string*>* headerTable = &initHeaderTable();
    return *headerTable;
}

const std::vector<std::string*>& HttpCommomHeaders::initHeaderTable()
{
    const uint8_t offset = 2;
    std::vector<std::string*>* headerTable = new std::vector<std::string*>(TOTAL_KEYWORDS + offset);

    headerTable->operator[](0) = new std::string();
    headerTable->operator[](1) = new std::string();

    for(size_t i = 0; i < TOTAL_KEYWORDS; i++)
    {
        HttpHeaderCode code = wordlist[i].headerCode;
        size_t codeIndex = static_cast<size_t>(code);
        assert(codeIndex < TOTAL_KEYWORDS + offset);
        headerTable->operator[](codeIndex) = new std::string(wordlist[i].headerName);
    }
    return *headerTable;
}

const char* HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode code)
{
    size_t codeIndex = static_cast<size_t>(code);
    return getHeaderTable()[codeIndex]->c_str();
}


}//http
