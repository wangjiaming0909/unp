#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include "util/easylogging++.h"
#include "util/timer.h"
#include "util/min_heap.h"


void hide_pwd(std::string& conn_str)
{
  std::string tmp = conn_str;
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
  size_t start_pos = tmp.find("PWD");
  if(start_pos == std::string::npos) {
    return;
  }
  size_t semi_colon_pos = tmp.find(';', start_pos);
  if(semi_colon_pos == std::string::npos) {
    conn_str.erase(start_pos);
    return ;
  }
  size_t left_brace_pos = tmp.find('{', start_pos);
  if(left_brace_pos == std::string::npos || left_brace_pos > semi_colon_pos) {
    conn_str.erase(start_pos, semi_colon_pos - start_pos + 1);
    return;
  }
  //;PWD={***;***};
  size_t right_brace_pos = tmp.find('}', semi_colon_pos);
  if(right_brace_pos != std::string::npos) {
    size_t next_semi_colon_pos = tmp.find(';', right_brace_pos);
    if(next_semi_colon_pos == std::string::npos)
      conn_str.erase(start_pos);
    else
      conn_str.erase(start_pos, next_semi_colon_pos - start_pos + 1);
    return;
  } else
  {
    conn_str.erase(start_pos, semi_colon_pos - start_pos + 1);
  }

}

TEST(remove_pwd, normal)
{
    std::string connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD=;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD =012asapwd=;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD =012asapwd=;ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD ={012a;sapwd=};ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD ={012a;sapwd=;ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;
}

void print_filelock(const flock& lock)
{
  LOG(INFO) << "lock type: " << lock.l_type;
  LOG(INFO) << "lock len: " << lock.l_len;
  LOG(INFO) << "lock start: " << lock.l_start;
  LOG(INFO) << "lock pid: " << lock.l_pid;
  LOG(INFO) << "lock whence: " << lock.l_whence;
}

void print_filelock_of_file(const char* filename)
{
  auto fd = ::open(filename, O_CREAT | O_APPEND);
  flock lock{};
  ::fcntl(fd, F_GETLK, &lock);
  print_filelock(lock);
  ::close(fd);
}

TEST(fcntl, file_lock)
{
  print_filelock_of_file("filelock");
  print_filelock_of_file("test.out");
  print_filelock_of_file("boost_1_72_0.tar");
}

std::vector<double> put_together_and_sort(const std::vector<std::vector<double>>& v)
{
  vector<double> tv;
  for(auto & vv : v) {
    for(auto i : vv) {
      tv.push_back(i);
    }
  }
  std::sort(tv.begin(), tv.end());
  return tv;
}

void binary_insert_sort(const std::vector<std::vector<double>>& v)
{

}

void minheap_merg_sort(const std::vector<std::vector<double>>& v)
{
}

struct MergeNode
{
  MergeNode(double value, int index) : value(value), indexInArray(index){}
  double value;
  int indexInArray;
  bool operator>(const MergeNode& node) const {return value > node.value;}//have to be const for std::greater
  bool operator>=(const MergeNode& node) const {return value >= node.value;}
  bool operator<(const MergeNode& node) const {return value < node.value;}
  bool operator<=(const MergeNode& node) const {return value <= node.value;}
  bool operator==(const MergeNode& node) const {return value == node.value;}
};

//no matter of CPU usage and memory usage minheap sorting is slower than put together and sort
std::vector<double> mergeMultiSortedArrayWithMinHeap(std::vector<std::vector<double>> &arrays)
{
  std::vector<double> ret{};
  if(arrays.size() == 0) return ret;
  util::min_heap<MergeNode> minHeap{};

  std::vector<std::decay<decltype(arrays[0])>::type::const_iterator> its;
  for(size_t i = 0; i < arrays.size(); i++ ) //O(k)
  {
    its.push_back(arrays[i].cbegin());
  }

  for(size_t i = 0; i < arrays.size(); i++)//O(k)
  {
    minHeap.emplace(*its[i], i);
  }

  while(true)// 在 MinHeap中, 一共有K个元素, 一共进行了pop或者emplace N次 (N为所有的元素个数), 因此复杂度为: O(2Nlog(K))
  {
    MergeNode minNode = minHeap.top();
    ret.push_back(minNode.value);
    minHeap.pop();
    if(arrays[minNode.indexInArray].cend() != (its[minNode.indexInArray] + 1)) {
      minHeap.emplace(*(its[minNode.indexInArray] + 1), minNode.indexInArray);
      ++its[minNode.indexInArray];
    }
    if(minHeap.size() == 0) break;
  }
  return ret;
}

TEST(al, mul_merge_sort)
{
  LOG(DEBUG) << "al.mul_merge_sort";

  using namespace std;
  vector<vector<double>> v;
  v.resize(100);

  for(int i = 0; i < 100; ++i) {
    for(int j = 0; j < 80000; ++j) {
      v[i].push_back((j + i) * (i + 0.1));
    }
  }

  vector<double> result;

  {
    utils::timer _{"using put together and sort: "};
    result = put_together_and_sort(v);
  }

  for(int i = 0; i < 200; ++i)
  {
    //LOG(DEBUG) << result[i];
  }

  {
    utils::timer _{"using binary insert sort: "};
  }

  {
    utils::timer _{"using minheap sort: "};
    result = mergeMultiSortedArrayWithMinHeap(v);
  }
  for(int i = 0; i < 200; ++i)
  {
    //LOG(DEBUG) << result[i];
  }
}
