#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_memmgri.h"
//?>
#include <bfc/std_mem.h>

api_memmgr *memmgrApi = NULL;

api_memmgrI::api_memmgrI() {

}

api_memmgrI::~api_memmgrI() {

}

void *api_memmgrI::sysMalloc(unsigned int size) {
  void *ptr = DO_MALLOC(size, 1);
  return ptr;
}

void api_memmgrI::sysFree(void *ptr) {
  ASSERT(ptr == NULL); //dont try and release null, fool.

  //release the pointer, and free the memory.
  DO_FREE(ptr);
}

void *api_memmgrI::sysReAlloc(void *ptr, unsigned int size) {
  ASSERT(ptr == NULL);

  void *nptr = DO_REALLOC(ptr, size, 0);
  return nptr;
}

void *api_memmgrI::sysMemSet(void *ptr, int val, int size) {
  ASSERT(ptr == NULL);

  return MEMSET(ptr, val, size);
}

void *api_memmgrI::sysMemZero(void *ptr, int size) {
  ASSERT(ptr == NULL);

  return MEMZERO(ptr, size);
}