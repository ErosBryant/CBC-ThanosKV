### Challenge

1. Changed from B+tree to learned index for space efficiency
2. How to set the size of S.Level 0 ?
   - Big size : low read performance
   - Small size : key learning frequently
3. Which learning unit do we choose?
   - SSTable
   - Level
   - Number of guards in Level 

----

### Structure

1. The SST life time of the upper level (0~3) was too short to be suitable for the learned index
    1. So we use NVM at the upper level
       1. Perform KV separation and then into NVM
          - benefit: the longer SST lifetime 
       2. Just put hot upper level data into NVM and perform KV separation on SSD
          - benefit: Hot Key does not do kv separation

2. How to effectively configure the structure of an NVM index
   1. B+tree & Learned index
      1. After learning the key, delete the keys from b+tree.
      2. How to manage key after compaction? 
         - Use learned index because of space efficiency
   2. Learned index
      1. Key range guard
      2. Number of SSTs




