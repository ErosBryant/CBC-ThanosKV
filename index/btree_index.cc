#include <stdlib.h>
#include "util/coding.h"
#include "leveldb/slice.h"
#include "btree_index.h"
#include "index_iterator.h"
#include "table/format.h"

namespace leveldb {

BtreeIndex::BtreeIndex() : condvar_(&mutex_) {
  bgstarted_ = false;
  should_run = true;
}

IndexMeta* BtreeIndex::Get(const Slice& key) {
  IndexMeta* result = (IndexMeta*)tree_.Search(fast_atoi(key));
  return result;
}

void BtreeIndex::Insert(const entry_key_t& key, const IndexMeta& meta) {
  // check btree if updated

IndexMeta* ptr = (IndexMeta*) nvram::pmalloc(sizeof(IndexMeta));
if (ptr == nullptr) {
    return; // Or appropriate error handling
}
  ptr->size = meta.size;
  ptr->file_number = meta.file_number;
  ptr->offset = meta.offset;
  clflush((char*)ptr, sizeof(IndexMeta));
  IndexMeta* old_ptr = (IndexMeta*) tree_.Insert(key, ptr);
  if (old_ptr != nullptr) {
    nvram::pfree(old_ptr);
  }
}


void BtreeIndex::Runner() {
  while (should_run) {
    mutex_.Lock();
    while (queue_.empty() && should_run) {
      condvar_.Wait();
    }
    while (!queue_.empty()) {
      uint64_t key = queue_.front().key;
      std::shared_ptr<IndexMeta> value = queue_.front().meta;
      queue_.pop_front();
      Insert(key, *value);
    }
    mutex_.Unlock();
  }
}

void* BtreeIndex::ThreadWrapper(void* ptr) {
  reinterpret_cast<BtreeIndex*>(ptr)->Runner();
  return NULL;
}



void BtreeIndex::AddQueue(std::deque<KeyAndMeta>& queue) {
  mutex_.Lock();
  assert(queue_.size() == 0);
  queue_.swap(queue);
  if (!bgstarted_) {
    bgstarted_ = true;
    pthread_create(&thread_, NULL, &BtreeIndex::ThreadWrapper, this);
  }
  condvar_.Signal();  // Signal the runner thread to process tasks
  mutex_.Unlock();
}


Iterator* BtreeIndex::NewIterator(const ReadOptions& options, TableCache* table_cache) {
  return new IndexIterator(options, tree_.GetIterator(), table_cache);
}

FFBtreeIterator* BtreeIndex::BtreeIterator() {
  return tree_.GetIterator();
}

void BtreeIndex::Break() {

  mutex_.Lock();
  should_run = false;
  condvar_.Signal();
  mutex_.Unlock();
  pthread_join(thread_, NULL);

}

pthread_t BtreeIndex::getThreadID() const {
    return thread_;
}




} // namespace leveldb