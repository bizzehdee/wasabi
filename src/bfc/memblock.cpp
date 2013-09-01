#include <precomp.h>
#include "memblock.h"

#ifdef WASABI_DEBUG
int memblocks_totalsize=0;
#endif

VoidMemBlock::VoidMemBlock(int _size, const void *data) {
  mem = NULL;
  size = 0;
  setSize(_size);
  if (data != NULL && size > 0) MEMMOVE(mem, data, size); // Using memmove because data might be in the middle of our buffer, who knows
}

VoidMemBlock::~VoidMemBlock() {
#ifdef WASABI_DEBUG
  memblocks_totalsize -= size;
#endif
  FREE(mem);
}

void *VoidMemBlock::setSize(int newsize, int noinit) {
#ifdef WASABI_DEBUG
  memblocks_totalsize -= size;
#endif
  ASSERT(newsize >= 0);
  if (newsize < 0) newsize = 0;
  if (newsize == 0) {
    FREE(mem);
    mem = NULL;
  } else if (size != newsize) {
    mem = noinit ? REALLOC_NOINIT(mem, newsize) : REALLOC(mem, newsize);
  }
  size = newsize;
#ifdef WASABI_DEBUG
  memblocks_totalsize += size;
#endif
  return getMemory();
}

void *VoidMemBlock::setMinimumSize(int newminsize, int increment) {
  if (newminsize > size) setSize(newminsize+increment);
  return getMemory();
}

void VoidMemBlock::setMemory(const void *data, int datalen, int offsetby) {
  if (datalen <= 0) return;
  ASSERT(mem != NULL);
  ASSERT(offsetby >= 0);
  char *ptr = reinterpret_cast<char *>(mem);
  ASSERT(ptr + offsetby + datalen <= ptr + size);
  MEMMOVE(ptr + offsetby, data, datalen); // using memmove because data migth be in the middle of our buffer, who knows
}

int VoidMemBlock::getSize() const {
  return size;
}

int VoidMemBlock::isMine(void *ptr) {
  return (ptr >= mem && ptr < (char*)mem + size);
}

void VoidMemBlock::terminate(int character) {
  if (mem == NULL || size < 1) return;
  *((char*)mem + size - 1) = character & 0xff;
}

void VoidMemBlock::zeroMemory() {
  if (mem == NULL || size < 1) return;
  MEMZERO(mem, size);
}
