#pragma once

#include <string>

namespace leveldb {

struct NVMOption {
  std::string pmem_path = "/mnt/pmemdir/dbbench";
  //size_t nvm_write_buffer_size = 4* 1024 * 1024 * 1024;
};

}  // namespace leveldb