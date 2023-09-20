#include "leveldb/persistant_pool.h"

#include <libpmemobj.h>
#include <libpmem.h>
#include <stdexcept>
#include <mutex>

#include <cstdio>
#include <NVM/nvm_option.h>

namespace leveldb {
namespace nvram {

#define LAYOUT_NAME "PMINDEXDB"

static PMEMobjpool* pm_pool;
// static bool init = true;
static uint64_t allocs = 0;

char* pmemaddr=nullptr;
size_t mapped_len;
int is_pmem;
char* raw_;
size_t mapped_len_;
static std::mutex pmem_mutex;

int nvm_file_exists(const char* file) { return access(file, F_OK); }


void create_pool(const std::string& dir, const size_t& size) {
    printf("Attempting to open NVM pool\n");
    pm_pool = pmemobj_open(dir.data(), LAYOUT_NAME);

    if (pm_pool == nullptr) {
        printf("Pool not found. Creating NVM pool with size of %lu\n", size);
        pm_pool = pmemobj_create(dir.data(), LAYOUT_NAME, size, 0666);
        
        if (pm_pool == nullptr) {
            fprintf(stderr, "pmem create error\n");
            perror(dir.data());
            exit(1);
        }
    } else {
        printf("NVM pool opened successfully\n");
    }
}

//pthread_t pt
void close_pool() {

    fprintf(stdout, "pmem allocs %lu\n", allocs);
    pmemobj_close(pm_pool);
    //std::remove("/mnt/pmemdir/my_pool");
  
}

void pfree(void* ptr) {
std::lock_guard<std::mutex> lock(pmem_mutex);
    PMEMoid oid = pmemobj_oid(ptr); 
        if (OID_IS_NULL(oid)) {
        return; // Don't attempt to free a null pointer.
    } 
    pmemobj_free(&oid);
  
}


void* pmalloc(size_t size) {
   std::lock_guard<std::mutex> lock(pmem_mutex);
    void* ptr;
    allocs++;
    PMEMoid oid; 
    
    if (pmemobj_zalloc(pm_pool, &oid, size, 0) != 0) 
        return nullptr; 

    return pmemobj_direct(oid);
}


void stats() {
//  char *msg;
//  pmemcto_stats_print(vmem, msg);
//  printf("%s\n", msg);
}

}
}