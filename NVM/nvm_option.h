#pragma once

#include <string>

namespace leveldb {

struct NVMOption {
  std::string pmem_path = "/mnt/pmemdir/";
  std::string sst_path="/media/eros/disk1/tmpthanos";
  //size_t nvm_write_buffer_size = 4* 1024 * 1024 * 1024;
};

}  // namespace leveldb