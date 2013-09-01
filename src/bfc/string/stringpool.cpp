#include "precomp.h"

#include "stringpool.h"

StringPool::StringPool(int _blocksize)
  : blocksize(_blocksize) {
  mem_used = 0;
}

StringPool::~StringPool() {
  blocks.deleteAll();
}

const char *StringPool::dupStr(const char *str) {
  int len = STRLEN(str)+1;
  block *b = blocks.getLast();
  for (;;) {
    if (b == NULL) {
      b = new block(blocksize);
      blocks.addItem(b);
    }
    if (b->freeblock_len < len) {
      if (b->nallocated == 0) return NULL;	// string too big
      b = NULL;
      continue;
    }
    break;
  }
  char *s = b->freeblock;
  STRCPY(s, str);	// including terminating 0
  b->nallocated++;
  b->freeblock += len;
  b->freeblock_len -= len;

  mem_used += len;

  return s;
}

void StringPool::delStr(const char *val) {
  mem_used -= STRLEN(val) + 1;
  for (int w = 0; w < blocks.getNumItems(); w++) {
    block *b = blocks[w];
    if (b->mem.isMine(const_cast<char *>(val))) {
      b->nfreed++;
      if (b->nfreed == b->nallocated) {
        blocks.delItem(b);
        delete b;
      }
      return;
    }
  }
}
