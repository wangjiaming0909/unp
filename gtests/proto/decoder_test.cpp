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

TEST(proto, all)
{
  using namespace downloadmessage;
  using namespace reactor;

  Mess_WL m1{};
  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  std::string url = "https://github.com/wangjiaming0909";
  m1.set_url(url);

  auto sizeLong = m1.ByteSizeLong();
  LOG(INFO) << "sizeLong: " << sizeLong;
  m1.set_len(1);
  sizeLong = m1.ByteSizeLong();
  LOG(INFO) << "sizeLong: " << sizeLong;

  char* d = (char*)::calloc(1024, 1);

  m1.SerializeToArray(d, sizeLong);
  
  Mess_WL m2{};
  auto ret = m2.ParseFromArray(d, sizeLong);
  ASSERT_TRUE(ret);

  m2.Clear();
  ret = m2.ParseFromArray(d, sizeLong-1);
  ASSERT_FALSE(ret);

  m2.Clear();
  ret = m2.ParsePartialFromArray(d, sizeLong-1);
  ASSERT_FALSE(ret);

  Mess_WL m3{};
  LOG(INFO) << "non initialized size: " << m3.ByteSizeLong();
  ret = m3.ParsePartialFromArray(d+sizeLong-1, 1);
  LOG(INFO) << "initialized size: " << m3.ByteSizeLong();
  ASSERT_FALSE(ret);

  m2.MergeFrom(m3);

  printMess(m1);
  printMess(m2);
  printMess(m3);
}

TEST(decoder, bufer)
{
  using namespace reactor;
  using namespace downloadmessage;
  std::string url = "http://github.com/wangjiaming0909";
  Decoder<Mess_WL, int32_t> decoder;
  Mess_WL m1{};
  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  auto len = m1.ByteSizeLong();
  m1.set_url(url);
  m1.set_len(1);
  len = m1.ByteSizeLong();

  buffer buf{};
  buf.append(static_cast<int32_t>(len));

  char* d = (char*)::calloc(len,1);
  m1.SerializeToArray(d, len);
  auto ret = buf.append(d, len);
  ASSERT_EQ(ret, len);

  ret = decoder.decode(buf);
  ASSERT_EQ(ret, len + sizeof(int32_t));

  auto mes = decoder.getMess();

  printMess(*mes);
  printMess(m1);

  buffer buf2{};
  buf.append(static_cast<int32_t>(len));

  decoder.reset();
  ret = decoder.decode(buf);
  ASSERT_EQ(ret, sizeof(int32_t));

  buf.append(d, len - 1);

  ret = decoder.decode(buf);
  ASSERT_EQ(ret, 0);

  buf.append(d + len - 1, 1);
  ret = decoder.decode(buf);
  ASSERT_EQ(ret, len);

  mes = decoder.getMess();
  printMess(*mes);
  free(d);
}
