#ifndef _WASABI_STRINGPOOL_H
#define _WASABI_STRINGPOOL_H

#include <bfc/ptrlist.h>
#include <bfc/memblock.h>

/**
  Use this when you want to allocate a large number of strings and free them
  all at once. This class allocates large blocks of storage for your strings
  and packs them tightly. You can delete individual strings as well, but the
  storage won't be freed until the entire block is empty. This is very useful
  for caching db output, for example. Each string follows the next in the
  packing, so no bytes are wasted.
*/

class StringPool {
public:
  StringPool(int blocksize=65500);
  ~StringPool();

/**
  Makes space in the pool and duplicates the given string.
*/
  const char *dupStr(const char *str);
/**
  Marks this string as deleted. Storage will not be released back to the system
  until every string in the block is deleted.
*/
  void delStr(const char *ps);

private:
  int blocksize;
  struct block {
    block(int size) : mem(size) {
      freeblock = mem.getMemory();
      freeblock_len = size;
      nallocated = 0;
      nfreed = 0;
    }
    MemBlock<char> mem;
    char *freeblock;
    int freeblock_len;
    int nallocated;
    int nfreed;
  };
  PtrList<block> blocks;
  int mem_used;
};

#endif
