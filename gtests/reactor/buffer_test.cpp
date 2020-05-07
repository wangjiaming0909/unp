#include <gtest/gtest.h>
#include "reactor/buffer.h"
#include "proto/mess_wl.pb.h"
#include "reactor/poll_demultiplex_table.h"
#include "reactor/reactor_implementation.h"

using namespace reactor;
using namespace downloadmessage;

namespace buffer_test
{
void printMess(const downloadmessage::Mess_WL& m1)
{
  LOG(INFO) << "id: " << m1.id();
  LOG(INFO) << "len: " <<  m1.len();
  LOG(INFO) << "command: " << m1.command();
  LOG(INFO) << "url: " << m1.url();
}
}

TEST(buffer, buffer_proto)
{
  std::string url = "https";
  buffer buf;
  Mess_WL m1{};

  m1.set_id(1);
  m1.set_command(Mess_WL_DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
  m1.set_url(url);
  m1.set_len(1);
  auto byteSizeLong = m1.ByteSizeLong();

  buf.append(byteSizeLong);
  char* d = (char*)::calloc(byteSizeLong,1);
  m1.SerializeToArray(d, byteSizeLong);
  buf.append(d, byteSizeLong);

  int32_t len = 0;
  auto read = buf.read_T(len, sizeof(byteSizeLong));
  LOG(INFO) << "len read: " << len;
  ASSERT_EQ(read, sizeof(byteSizeLong));

  buf.drain(read);

  auto* new_d = buf.pullup(len);
  ASSERT_TRUE(new_d != nullptr);

  Mess_WL m2{};
  m2.ParseFromArray(new_d, len);
  buffer_test::printMess(m1);
  buffer_test::printMess(m2);
}
