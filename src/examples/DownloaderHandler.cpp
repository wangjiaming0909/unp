#include "examples/DownloaderHandler.h"
#include "http/HttpCodec.h"
#include "http/gperf/HttpHeaderCode.h"

namespace examples
{

int DownloaderHandler::handle_input(int handle)
{
  if(sock_connection_handler::handle_input(handle) < 0) {
    LOG(WARNING) << "error when sock_connection_handler::handle_input";
    return -1;
  }

  if(input_buffer_.buffer_length() == 0) {
    LOG(INFO) << "didn't get any data...";
    if(isShouldClose_)
      return -1;
    return 0;
  }

  while(input_buffer_.buffer_length() > 0) {
    auto firstChain = input_buffer_.begin().chain();
    auto data = firstChain.get_start_buffer();
    auto chainLen = firstChain.size();
    if(data != nullptr && chainLen != 0) {
      std::string s{static_cast<char*>(data), chainLen};
      LOG(INFO) << "received len: " << s.size();
      //LOG(INFO) << "----------------received: " << s;
    }
    string_piece::const_string_piece sp{static_cast<const char*>(data), chainLen};
    size_t bytesRead = codec_.onIngress(sp);
    if(codec_.hasError()) {
      LOG(WARNING) << "Http parser parse error";
      codec_.pause(1);
      return -1;
    }
    assert(bytesRead == chainLen);
    input_buffer_.drain(chainLen);
    // LOG(INFO) << "Consumed " << chainLen << " bytes...";
  }
  if(isShouldClose_)
    return -1;
  return 0;
}

int DownloaderHandler::open()
{
  int ret = 0;
  if(mesSetupCallback_) {
    ret = mesSetupCallback_(request_);
  }
  if(ret != 0) return -1;

  auto messageStr = request_.buildRequestMessage();
  LOG(INFO) << *messageStr;
  auto bytesWritten = write(messageStr->c_str(), messageStr->size(), true);
  if (bytesWritten <= 0) return -1;

  return enable_reading();
}

int DownloaderHandler::onStatus(const char* /*buf*/, size_t /*len*/)
{
  // if(codec_.status() == 302)
  // {
  //     LOG(WARNING) << "status is: " << codec_.status();
  //     auto* location = codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_LOCATION);
  //     if(location == nullptr)
  //     {
  //         LOG(WARNING) << "server returned 302, but no location header...";
  //         return -1;
  //     }

  //     return -1;
  // }else if(codec_.status() == 200)
  // {
  //     if(codec_.message().hasHeader(http::HttpHeaderCode::HTTP_HEADER_CONTENT_LENGTH))
  //     {
  //         fileSize_ = codec_.contentLength();
  //     }else if(*(codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING)) == "chunked")
  //     {
  //         isChunked_ = true;
  //     }
  // }
  return 0;
}

int DownloaderHandler::onBody(const char* buf, size_t size)
{
  if(usingRangeDownload_ && !codec_.message().hasHeader(http::HttpHeaderCode::HTTP_HEADER_CONTENT_RANGE)) {
    isShouldClose_ = true;
    return -1;
  }
  bytesDownloaded_ += size;
  if(fileWriterPtr_) {
    fileWriterPtr_->write(buf, size);
    fileWriterPtr_->flush();
    return 0;
  }
  return 0;
}

int DownloaderHandler::onHeadersComplete(size_t len)
{
  (void)len;
  if(whenToClose_ == http::Http1xCodec::CodecState::ON_HEADERS_COMPLETE) {
    isShouldClose_ = true;
  }
  LOG(INFO) << "on Headers complete...";
  return 0;
}

int DownloaderHandler::onMessageComplete()
{
  if(whenToClose_ == http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE) {
    isShouldClose_ = true;
  }
  LOG(INFO) << "Message completed...";
  return 0;
}

int DownloaderHandler::onChunkHeader(size_t len)
{
  LOG(INFO) << "on chunk header len: " << len;
  return 0;
}

int DownloaderHandler::onChunkComplete()
{
  LOG(INFO) << "on chunk complete...";
  return 0;
}
}
