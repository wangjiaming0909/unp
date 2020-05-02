#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/port.h>
#include <gtest/gtest.h>
#include "proto/decoder.h"
#include "proto/mess_wl.pb.h"
#include "util/easylogging++.h"

void printMess(const downloadmessage::Mess_WL& m1)
{
  LOG(INFO) << "id: " << m1.id();
  LOG(INFO) << "len: " <<  m1.len();
  LOG(INFO) << "command: " << m1.command();
  LOG(INFO) << "url: " << m1.url();
}

TEST(decoder, normal)
{
  using namespace reactor;
  using namespace downloadmessage;
  std::string url = "http://github.com/wangjiaming0909";
  Decoder<Mess_WL, int32_t> decoder;
  Mess_WL m1{};
  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  auto len = m1.ByteSizeLong();
  LOG(INFO) << "ByteSizeLong: " << len;

  m1.set_url(url);
  len = m1.ByteSizeLong();
  LOG(INFO) << "ByteSizeLong: " << len;

  auto size2 = sizeof(decltype(m1.id())) + sizeof(decltype(m1.command()))
    + url.size() + sizeof(decltype(m1.len()));
  LOG(INFO) << "size2: " << size2;
  LOG(INFO) << "ByteSizeLong: " << len;
  m1.set_len(size2);



  char* d = (char*)::calloc(1024, 1);
  m1.SerializeToArray(d, 1024);

  decoder.decode(d, 1024);
  if (decoder.isCompleted())
  {
    auto mesDecoded = decoder.getMess();
    printMess(mesDecoded);
    ASSERT_EQ(decoder.getMess().url(), m1.url());
    ASSERT_EQ(decoder.getMess().command(), m1.command());
    ASSERT_EQ(decoder.getMess().id(), m1.id());
    ASSERT_EQ(decoder.getMess().len(), m1.len());
  }

  free(d);
}

TEST(decoder, multi_time_parse)
{
  using namespace reactor;
  using namespace downloadmessage;
  std::string url = "http://github.com/wangjiaming0909";
  Decoder<Mess_WL, int32_t> decoder;
  Mess_WL m1{};
  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  m1.set_url(url);
  auto len = m1.ByteSizeLong();
  m1.set_len(len);

  char* d = (char*)::calloc(1024, 1);
  m1.SerializeToArray(d, 1024);

  auto lenParsed = decoder.decode(d, 5);
  ASSERT_FALSE(decoder.getMess().IsInitialized());

  lenParsed += decoder.decode(d+lenParsed, 5);
  ASSERT_FALSE(decoder.getMess().IsInitialized());

  decoder.decode(d + lenParsed, 1014);
  ASSERT_TRUE(decoder.getMess().IsInitialized());
  printMess(decoder.getMess());
  ASSERT_EQ(decoder.getMess().url(), m1.url());
  free(d);
}

TEST(decoder, multi_message_parse)
{
  using namespace downloadmessage;
  using namespace reactor;

  std::string url = "http://github.com/wangjiaming0909";
  std::string url2 = url+"123";
  Decoder<Mess_WL, int32_t> decoder;
  Mess_WL m1{};
  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  m1.set_url(url);
  auto len1 = m1.ByteSizeLong();
  m1.set_len(len1);

  char* d = (char*)::calloc(1024, 1);
  m1.SerializeToArray(d, 1024);


  Mess_WL m2{};
  m2.set_id(1);
  m2.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_PAUSE);
  m2.set_url(url2);
  auto len2 = m2.ByteSizeLong();
  m2.set_len(len2);

  m2.SerializeToArray(d + len1, 1024-len1);

  decoder.decode(d, 1024);

  LOG(INFO) << "m1: --------";
  printMess(m1);
  LOG(INFO) << "m2: --------";
  printMess(m2);
  LOG(INFO) << "decoder: --------";
  printMess(decoder.getMess());
}
