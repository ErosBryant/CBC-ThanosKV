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

  
//   size_t size = (s < PMEMOBJ_MIN_POOL) ? PMEMOBJ_MIN_POOL : s;
//   printf("Creating NVM pool size of %lu\n", size);
//   pm_pool = pmemobj_create(dir.data(), LAYOUT_NAME, size, 0666);
//   init = true;
//   if (pm_pool == nullptr) {
//     fprintf(stderr, "pmem create error\n");
//     perror(dir.data());
//     exit(1);
//   }

//   char* pmemaddr = (char *)(pmem_map_file(pol_path.c_str(), s,
//                                                 PMEM_FILE_CREATE, 0666,
//                                                 &mapped_len, &is_pmem));


// }

//pthread_t pt
void close_pool() {
  if (init) {
    fprintf(stdout, "pmem allocs %lu\n", allocs);


    // 如果您想确保线程已经完全结束，您可以使用pthread_join
    //pthread_join(pt, NULL);

    pmemobj_close(pm_pool);
    //std::remove("/mnt/pmemdir/my_pool");
  }
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
}

void stats() {
//  char *msg;
//  pmemcto_stats_print(vmem, msg);
//  printf("%s\n", msg);
}

}
}