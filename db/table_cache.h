// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Thread-safe (provides internal synchronization)

#ifndef STORAGE_LEVELDB_DB_TABLE_CACHE_H_
#define STORAGE_LEVELDB_DB_TABLE_CACHE_H_

#include <string>
#include <cstdint>
#include "db/dbformat.h"
#include "leveldb/cache.h"
#include "leveldb/table.h"
#include "leveldb/index.h"
#include "port/port.h"

namespace leveldb {

class Env;

struct TableHandle {
  typedef void (*CleanupFunction)(void* arg1, void* arg2);

  TableHandle() : table_(nullptr), func(nullptr), arg1(nullptr), arg2(nullptr) { }

  void RegisterCleanup(CleanupFunction arg, void* cache, void* handle) {
    func = arg;
    arg1 = cache;
    arg2 = handle;
  }

  ~TableHandle() {
    (*func)(arg1, arg2);
  }

  Table* table_;
  CleanupFunction func;
  void* arg1;
  void* arg2;
};

class TableCache {
 public:
  TableCache(const std::string& dbname, const Options* options, int entries);
  ~TableCache();

  // Return an iterator for the specified file number (the corresponding
  // file length must be exactly "file_size" bytes).  If "tableptr" is
  // non-NULL, also sets "*tableptr" to point to the Table object
  // underlying the returned iterator, or NULL if no Table object underlies
  // the returned iterator.  The returned "*tableptr" object is owned by
  // the cache and should not be deleted, and is valid for as long as the
  // returned iterator is live.
  Iterator* NewIterator(const ReadOptions& options,
                        uint64_t file_number,
                        uint64_t file_size,
                        Table** tableptr = nullptr);

  // If a seek to internal key "k" in specified file finds an entry,
  // call (*handle_result)(arg, found_key, found_value).
  Status Get(const ReadOptions& options,
             const IndexMeta* index,
             const Slice& k,
             void* arg,
             void(*handle_result)(void*, const Slice&, const Slice&));

  Status Get(const ReadOptions& options,
             uint64_t file_number,
             uint64_t file_size,
             const Slice& k,
             void* arg,
             void (*handle_result)(void*, const Slice&, const Slice&));

  Status Get(const ReadOptions& options, uint64_t file_number,
             uint64_t file_size, const Slice& k, void* arg,
             void (*handle_result)(void*, const Slice&, const Slice&), int level,
             FileMetaData* meta = nullptr, uint64_t lower = 0, uint64_t upper = 0, bool learned = false, Version_sst* version = nullptr,
             adgMod::LearnedIndexData** model = nullptr, bool* file_learned = nullptr);


  Status GetBlockIterator(const ReadOptions& options,
                          const IndexMeta* index,
                          Iterator** iterator);

  Status GetTable(uint64_t file_number, uint64_t, TableHandle* table_handle);

  // Evict any entry for the specified file number
  void Evict(uint64_t file_number);

 bool FillData(const ReadOptions& options, FileMetaData* meta, adgMod::LearnedIndexData* data);
  
  void LevelRead(const ReadOptions& options, uint64_t file_number,
                 uint64_t file_size, const Slice& k, void* arg,
                 void (*handle_result)(void*, const Slice&, const Slice&), int level,
                 FileMetaData* meta = nullptr, uint64_t lower = 0, uint64_t upper = 0, bool learned = false, Version_sst* version = nullptr);



 private:
  Env* const env_;
  const std::string dbname_;
  const Options* options_;
  Cache* cache_;

  Status FindTable(uint64_t file_number, uint64_t file_size, Cache::Handle**);
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_TABLE_CACHE_H_
