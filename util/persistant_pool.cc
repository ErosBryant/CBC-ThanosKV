#include "leveldb/persistant_pool.h"

#include <libpmemobj.h>
#include <libpmem.h>
#include <cstdio>
#include <NVM/nvm_option.h>

namespace leveldb {
namespace nvram {

#define LAYOUT_NAME "PMINDEXDB"

static PMEMobjpool* pm_pool;
static bool init = true;
static uint64_t allocs = 0;

char* pmemaddr=nullptr;
size_t mapped_len;
int is_pmem;
char* raw_;
size_t mapped_len_;
int nvm_file_exists(const char* file) { return access(file, F_OK); }

void create_pool(const std::string& dir, const size_t& s) {
  init = true;
  printf("Opening NVM pool\n");
  if (nvm_file_exists(dir.data()) != 0) {
  size_t size = (s < PMEMOBJ_MIN_POOL) ? PMEMOBJ_MIN_POOL : s;
  printf("Creating NVM pool size of %lu\n", size);
  pm_pool = pmemobj_create(dir.data(), LAYOUT_NAME, size, 0666);
    if (pm_pool == nullptr) {
    fprintf(stderr, "pmem create error\n");
    perror(dir.data());
    exit(1);
  }
  }
}

// void create_pool(const std::string& dir, const size_t& s) {

  
//   // size_t size = (s < PMEMOBJ_MIN_POOL) ? PMEMOBJ_MIN_POOL : s;
//   // printf("Creating NVM pool size of %lu\n", size);
//   // pm_pool = pmemobj_create(dir.data(), LAYOUT_NAME, size, 0666);
//   // init = true;
//   // if (pm_pool == nullptr) {
//   //   fprintf(stderr, "pmem create error\n");
//   //   perror(dir.data());
//   //   exit(1);
//   // }

//   char* pmemaddr = (char *)(pmem_map_file(pol_path.c_str(), s,
//                                                 PMEM_FILE_CREATE, 0666,
//                                                 &mapped_len, &is_pmem));
//     assert(pmemaddr != nullptr);
//     raw_ = pmemaddr;
//     mapped_len_=mapped_len;
// }

//pthread_t pt
void close_pool() {

    fprintf(stdout, "pmem allocs %lu\n", allocs);
    pmemobj_close(pm_pool);
    std::remove("/mnt/pmemdir/my_pool");
  
  // pmem_unmap(raw_, mapped_len_);
}

void pfree(void* ptr) {
  //printf("pfree\n");
  if (!init) {
    free(ptr);
  } else {
    PMEMoid oid = pmemobj_oid(ptr); 
    if (!OID_IS_NULL(oid)) pmemobj_free(&oid);
  }
}

void* pmalloc(size_t size) {
  void* ptr;
  if (!init) {
    //printf("pmallo1c\n");
    ptr = malloc(size);
  } else {
    allocs++;
    PMEMoid oid; 
    if (pmemobj_zalloc(pm_pool, &oid, size, 0) != 0) 
        ptr = nullptr; 
    else 
      ptr = pmemobj_direct(oid); 
    
    if (ptr == nullptr) {
      fprintf(stderr, "pmem malloc error 2 \n");
      perror("vmem_malloc");
      exit(1);
    }
  }
  return ptr;
    // char* alloc = nullptr;
    // for(unsigned int i=0; i < num_; i++){
    //   if(bitmap_->get(i) == 0) {
    //     index = (int)i;
    //     alloc = raw_ + index * size;
    //     bitmap_->set(index);
    //     use_num_ = use_num_ + 1;
    //     return alloc;
    //   }
    // }


}

void stats() {
//  char *msg;
//  pmemcto_stats_print(vmem, msg);
//  printf("%s\n", msg);
}

}
}