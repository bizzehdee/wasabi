#ifndef _STD_MEM_H
#define _STD_MEM_H

#include "platform/platform.h"

// safer delete, avoids problems where the object's destructor causes reentry to a piece of
// code that accesses the pointer while it is being deleted. this sets the pointer to NULL first, 
// then deletes the object.
#define safedelete(objectPointer, TYPE) { TYPE tempPtr = objectPointer; objectPointer = NULL; delete tempPtr; }

//#define USE_MEM_CHECK

#ifdef USE_MEM_CHECK
#define EXTRA_INFO                , char *file, int line
#define EXTRA_PARAMS              , file, line
#define EXTRA_INFO_HERE                , __FILE__, __LINE__
#define MALLOC(size)                DO_MALLOC(size, 1, __FILE__, __LINE__)
#define MALLOC_NOINIT(size)         DO_MALLOC(size, 0, __FILE__, __LINE__)
#define CALLOC(records, recordsize) DO_CALLOC(records, recordsize, __FILE__, __LINE__)
#define REALLOC(ptr, size)          DO_REALLOC(ptr, size, 1, __FILE__, __LINE__)
#define REALLOC_NOINIT(ptr, size)   DO_REALLOC(ptr, size, 0, __FILE__, __LINE__)
#define FREE(ptr)                   DO_FREE(ptr, __FILE__, __LINE__)
#define MEMDUP(src, n)              DO_MEMDUP(src, n, __FILE__, __LINE__)
#define STRDUP(ptr)                 DO_STRDUP(ptr, __FILE__, __LINE__)
#define WSTRDUP(ptr)                DO_WSTRDUP(ptr, __FILE__, __LINE__)
#else
#define EXTRA_INFO
#define EXTRA_PARAMS
#define EXTRA_INFO_HERE
#define MALLOC(size)                DO_MALLOC(size, 1)
#define MALLOC_NOINIT(size)         DO_MALLOC(size, 0)
#define CALLOC(records, recordsize) DO_CALLOC(records, recordsize)
#define REALLOC(ptr, size)          DO_REALLOC(ptr, size, 1)
#define REALLOC_NOINIT(ptr, size)   DO_REALLOC(ptr, size, 0)
#define FREE(ptr)                   DO_FREE(ptr)
#define MEMDUP(src, n)              DO_MEMDUP(src, n)
#define STRDUP(ptr)                 DO_STRDUP(ptr)
#define WSTRDUP(ptr)                DO_WSTRDUP(ptr)
#endif

// these values were empirically determined on a P4/3.06Ghz
#ifdef CUSTOM_MMXMEMCPY_THRESHOLD
#define EXTMMX_MEMCPY_THRESHOLD CUSTOM_MMXMEMCPY_THRESHOLD
#else
#define EXTMMX_MEMCPY_THRESHOLD 268*1024
#endif

#ifdef CUSTOM_MMXMEMFILLS_THRESHOLD
#define EXTMMX_MEMFILLS_THRESHOLD CUSTOM_MMXMEMFILLS_THRESHOLD
#else
#define EXTMMX_MEMFILLS_THRESHOLD 204*1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *DO_MALLOC(size_t size, int zero EXTRA_INFO);
void *DO_CALLOC(size_t records, size_t recordsize EXTRA_INFO);
void *DO_REALLOC(void *ptr, size_t size, int zero EXTRA_INFO);
void DO_FREE(void *ptr EXTRA_INFO);

void *DO_MEMDUP(const void *src, int n EXTRA_INFO);
void *MEMCPY(void *dest, const void *src, size_t n);
void *MEMMOVE(void *dest, const void *src, size_t n); // same as MEMCPY, except ovrlapping segments are allowed

int MEMSIZE(void *ptr);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
static __inline int MEMCMP(const void *buf1, const void *buf2, int count) {
  return memcmp(buf1, buf2, count);
}

extern int stdmem_mmx_available;
extern int stdmem_sse_available; // SSE flag is necessary for movntq/prefetchnta extended mmx instructions
extern int stdmem_cpuid_check_done;
void StdMemCheckCPUID();

static __inline void *MEMSET(void *dest, int c, int n) {
  void *ret = dest;
#if defined(WIN32) && !defined(_WIN64)
  if (!stdmem_cpuid_check_done) StdMemCheckCPUID();
  if (n >= EXTMMX_MEMFILLS_THRESHOLD && stdmem_sse_available) {

    unsigned int mmx_c[2]={c|(c<<8)|(c<<16)|(c<<24),c|(c<<8)|(c<<16)|(c<<24)};
    int size_div_64 = n>>6;
    __asm {
        movq mm0, mmx_c
	      mov	edx, [dest]
	      mov	eax, size_div_64
      l:
	      movntq	[edx], mm0
	      movntq	[edx+8], mm0
	      movntq	[edx+16], mm0
	      movntq	[edx+24], mm0
	      movntq	[edx+32], mm0
	      movntq	[edx+40], mm0
	      movntq	[edx+48], mm0
	      movntq	[edx+56], mm0
	      add	edx, 64
	      dec	eax
	      jnz	l
	      sfence
	      emms 
      };

    int offset = size_div_64<<6;
    n -= offset;
    if (n == 0) return ret;
    dest = (char *)dest + offset; 
  }
  // fallthru for the rest of the buffer
#endif
  memset(dest, c, n);
  return ret;
}

static __inline void *MEMZERO(void *dest, int nbytes) {
  return MEMSET(dest, 0, nbytes);
}
#else
#define MEMCMP memcmp
#define MEMSET memset
#define MEMZERO(dest, nbytes) memset(dest, 0, nbytes)
#endif

#ifdef __cplusplus

// these are for structs and basic classes only
static __inline void ZERO(int &obj) { obj = 0; }
template<class T>
inline void ZERO(T &obj) { MEMZERO(&obj, sizeof(T)); }

// generic version that should work for all types
template<class T>
inline void MEMFILL(T *ptr, T val, unsigned int n) {
  for (int i = 0; i < n; i++) ptr[i] = val;
}

// asm 32-bits version
void MEMFILL32(void *ptr, unsigned long val, unsigned int n);

// helpers that call the asm version
template<>
inline void MEMFILL<unsigned long>(unsigned long *ptr, unsigned long val, unsigned int n) { MEMFILL32(ptr, val, n); }

template<>
void MEMFILL<unsigned short>(unsigned short *ptr, unsigned short val, unsigned int n);

// int
template<>
inline void MEMFILL<int>(int *ptr, int val, unsigned int n) {
  MEMFILL32(ptr, *reinterpret_cast<unsigned long *>(&val), n);
}

// float
template<>
inline void MEMFILL<float>(float *ptr, float val, unsigned int n) {
  MEMFILL32(ptr, *reinterpret_cast<unsigned long *>(&val), n);
}

// works in 32-bit chunks only
void MEMCPY32(void *dest, void *src, unsigned int nwords);

#endif	// __cplusplus defined

#endif
