#include "leveldb/persistant_pool.h"

#include <libpmemobj.h>

namespace leveldb {
namespace nvram {

#define LAYOUT_NAME "PMINDEXDB"

static PMEMobjpool* pm_pool;
static bool init = false;
static uint64_t allocs = 0;


void create_pool(const std::string& dir, const size_t& s) {

  size_t size = (s < PMEMOBJ_MIN_POOL) ? PMEMOBJ_MIN_POOL : s;
  printf("Creating NVM pool size of %lu\n", size);
  pm_pool = pmemobj_create(dir.data(), LAYOUT_NAME, size, 0666);
  init = true;

  printf("pmemobj_create\n");
  if (pm_pool == nullptr) {
    fprintf(stderr, "pmem create error\n");
    perror(dir.data());
    exit(1);
  }
}

void close_pool() {
  if (init) {
    fprintf(stdout, "pmem allocs %lu\n", allocs);
    pmemobj_close(pm_pool);
  }
}

void pfree(void* ptr) {
  //printf("pfree\n");
  if (!init) {
    free(ptr);
  } else {
    PMEMoid oid = pmemobj_oid(ptr); if (!OID_IS_NULL(oid)) pmemobj_free(&oid);
  }
}

void* pmalloc(size_t size) {
  void* ptr;
  if (!init) {
    //printf("pmallo1c\n");
    ptr = malloc(size);
  } else {
    allocs++;
    printf("pmalloc\n");
    PMEMoid oid; if (pmemobj_zalloc(pm_pool, &oid, size, 0) != 0) ptr = nullptr; else ptr = pmemobj_direct(oid); if (ptr == nullptr) {
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