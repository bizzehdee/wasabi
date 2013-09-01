#include "precomp.h"

#include "std_mem.h"

#include "std_math.h"
#include "assert.h"

#define USE_MALLOC
//#define COUNTUSAGE

#ifdef WIN32
#ifdef USE_MALLOC
#include <malloc.h>
#endif
#endif

#ifdef USE_MEM_CHECK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef COUNTUSAGE
static totalMem=0;
#endif

#if defined(ALLOC_ALIGN_128)
#define ALLOCMGR_HDR_SIZE 16
#define USE_MALLOC
#ifdef USE_MEM_CHECK
#undef USE_MEM_CHECK
#endif
#elif defined(ALLOC_ALIGN_64)
#define ALLOCMGR_HDR_SIZE 8
#define USE_MALLOC
#ifdef USE_MEM_CHECK
#undef USE_MEM_CHECK
#endif
#else
#define ALLOCMGR_HDR_SIZE 4
#endif

#define USER_PTR_TO_ALLOCMGR_PTR(x) (((char *)x)-ALLOCMGR_HDR_SIZE)
//CUT #define ALLOCMGR_PTR_TO_USER_PTR(x) (((char *)x)+4)
static inline void *ALLOCMGR_PTR_TO_USER_PTR(void *x) {
  if (x == 0) return x;	// don't hide NULL
  return ((char *)x + ALLOCMGR_HDR_SIZE);
}
#define ALLOCMGR_SIZE(x) *((int *)x)

void *DO_MALLOC(size_t size, int zero EXTRA_INFO) {
  ASSERT(size > 0);
  if (size <= 0) return NULL;

#ifdef COUNTUSAGE
  totalMem+=size;
#endif

#ifndef USE_MEM_CHECK
 #ifndef USE_MALLOC
  void *p = GlobalAlloc(GPTR, size+ALLOCMGR_HDR_SIZE);
 #else
  #if defined(ALLOC_ALIGN_128)
   void *p = _aligned_malloc(size+ALLOCMGR_HDR_SIZE, 16);
  #elif defined(ALLOC_ALIGN_64)
   void *p = _aligned_malloc(size+ALLOCMGR_HDR_SIZE, 8);
  #else
   void *p = malloc(size+ALLOCMGR_HDR_SIZE);
  #endif
  if (zero && p != NULL) MEMSET(p, 0, (int)size+ALLOCMGR_HDR_SIZE);//BU added in response to talkback info
 #endif
#else
 #ifndef USE_MALLOC
  void *p = GlobalAlloc(GPTR, size+ALLOCMGR_HDR_SIZE, file, line);
 #else
  void *p = _malloc_dbg(size+ALLOCMGR_HDR_SIZE, _NORMAL_BLOCK, file, line);
  if (zero && p != NULL) memset(p, 0, size+ALLOCMGR_HDR_SIZE);
 #endif
#endif
 if (p) ALLOCMGR_SIZE(p) = (int)size;
 return ALLOCMGR_PTR_TO_USER_PTR(p);
}

void *DO_CALLOC(size_t records, size_t recordsize EXTRA_INFO) {
  ASSERT(records > 0);
  ASSERT(recordsize > 0);
  return DO_MALLOC(records * recordsize, 1 EXTRA_PARAMS);
}

void DO_FREE(void *ptr EXTRA_INFO) {
  if (ptr == NULL) return;

#ifdef COUNTUSAGE
 totalMem -= ALLOCMGR_SIZE(USER_PTR_TO_ALLOCMGR_PTR(ptr));
#endif

#ifndef USE_MEM_CHECK
 #ifndef USE_MALLOC
  GlobalFree(USER_PTR_TO_ALLOCMGR_PTR(ptr));
 #else
  #if defined(ALLOC_ALIGN_128) || defined(ALLOC_ALIGN_64)
   _aligned_free(USER_PTR_TO_ALLOCMGR_PTR(ptr));
  #else
   free(USER_PTR_TO_ALLOCMGR_PTR(ptr));
  #endif
 #endif
#else
 #ifndef USE_MALLOC
  GlobalFree(USER_PTR_TO_ALLOCMGR_PTR(ptr), file, line); // FG> same as GlobalAlloc here, i'd like a way to translate my define inside the macro
 #else
  _free_dbg(USER_PTR_TO_ALLOCMGR_PTR(ptr), _NORMAL_BLOCK);
 #endif
#endif
}

// Note: as of 5/20/2005, MEMCPY does NO LONGER allow dest and src to overlap, use MEMMOVE if you need this behavior
void *MEMCPY(void *dest, const void *src, size_t n) {
  if (n == 0) return dest;
  ASSERT(dest != NULL);
  ASSERT(src != NULL);
  //ASSERT(n > 0); 
  void *ret = dest;
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64
  if (!stdmem_cpuid_check_done) StdMemCheckCPUID();
  if (n >= EXTMMX_MEMCPY_THRESHOLD && stdmem_sse_available) {
    int size_div_64 = n>>6;
    _asm {
      mov esi, src
      mov edi, dest
      mov ecx, size_div_64

    loop1:
      prefetchnta [esi+64] // prefetch next loop, non-temporal
      prefetchnta [esi+96]

      movq mm1, [esi]   // read in source data
      movq mm2, [esi+8]
      movq mm3, [esi+16]
      movq mm4, [esi+24]
      movq mm5, [esi+32]
      movq mm6, [esi+40]
      movq mm7, [esi+48]
      movq mm0, [esi+56]

      movntq [edi],    mm1 // non-temporal stores
      movntq [edi+8],  mm2
      movntq [edi+16], mm3
      movntq [edi+24], mm4
      movntq [edi+32], mm5
      movntq [edi+40], mm6
      movntq [edi+48], mm7
      movntq [edi+56], mm0

      add esi, 64
      add edi, 64
      dec ecx
      jnz loop1

      sfence
      emms
    }
    
    int offset = size_div_64<<6;
    n -= offset;
    if (n == 0) return ret;
    dest = (unsigned char *)dest + offset;
    src = (unsigned char *)src + offset;
  }
  // fallthru for the rest of the buffer
#endif

  memcpy(dest, src, n);

  return ret;
}
    
void *MEMMOVE(void *dest, const void *src, size_t n) {
  ASSERT(dest != NULL);
  ASSERT(src != NULL);
  ASSERT(n >= 0);
  memmove(dest, src, n);
  return dest;
}

void *DO_MEMDUP(const void *src, int n EXTRA_INFO) {
  void *ret;
  ASSERT(n >= 0);
  if (src == NULL || n == 0) return NULL;
  ret = DO_MALLOC(n, 0 EXTRA_PARAMS);
  if (ret == NULL) return NULL;
  MEMCPY(ret, src, n);
  return ret;
}

#define FAKE_REALLOC

void *DO_REALLOC(void *ptr, size_t size, int zero EXTRA_INFO) {
  ASSERT(size >= 0);

  if (size == 0) {
#ifdef USE_MEM_CHECK
    DO_FREE(ptr, file, line);
#else
    DO_FREE(ptr);
#endif
    return NULL;
  }

#ifdef USE_MEM_CHECK
  if(ptr == NULL) return DO_MALLOC(size, zero, file, line);
#else
  if(ptr == NULL) return DO_MALLOC(size, zero);
#endif
  void *r = NULL;

  size_t oldsize = ALLOCMGR_SIZE(USER_PTR_TO_ALLOCMGR_PTR(ptr));

#ifdef COUNTUSAGE
 #ifndef USE_MALLOC
 totalMem -= oldsize;
 #endif
#endif

#ifndef USE_MEM_CHECK
 #ifndef USE_MALLOC
  r = GlobalReAlloc((HGLOBAL)USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE, (zero ? GMEM_ZEROINIT : 0));
 #else
  #if defined(ALLOC_ALIGN_128)
   r = _aligned_realloc(USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE, 16);
  #elif defined(ALLOC_ALIGN_64)
   r = _aligned_realloc(USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE, 8);
  #else
   r = realloc(USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE);
  #endif
 #endif // use_malloc
#else // mem_spy
 #ifndef USE_MALLOC
  r = GlobalReAlloc((HGLOBAL)USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE, file, line);
 #else
  r = _realloc_dbg(USER_PTR_TO_ALLOCMGR_PTR(ptr), size+ALLOCMGR_HDR_SIZE, _NORMAL_BLOCK, file, line);
 #endif
#endif

 // r is an ALLOCMGR pointer

  if (r != NULL) {
    ALLOCMGR_SIZE(r) = (int)size;
    r = ALLOCMGR_PTR_TO_USER_PTR(r);
  } else {
    // realloc failed !
    void *newblock = MALLOC_NOINIT(size);
//BU um shouldn't there be error checking here?
    MEMCPY(newblock, ptr, MIN<int>((int)oldsize, (int)size));
    // no need to zero the remaining of the buffer here it's done a few lines down
    FREE(ptr);
    r = newblock;
  }

 // r is now a USER pointer

#ifdef COUNTUSAGE
  totalMem += size;
#endif

  #ifdef USE_MALLOC
   if (zero && size > oldsize)
    MEMZERO((char *)r+oldsize, (int)(size-oldsize));
  #endif

  return r;
}

int MEMSIZE(void *ptr) {
  return ALLOCMGR_SIZE(USER_PTR_TO_ALLOCMGR_PTR(ptr));
}

int stdmem_mmx_available = 0;
int stdmem_sse_available = 0;
int stdmem_cpuid_check_done = 0;

void StdMemCheckCPUID() {
  if (stdmem_cpuid_check_done) return;
#if defined(WASABI_PLATFORM_WIN32) && !defined(_WIN64) // no __asm on x64
//PORTME
  int retval1, retval2;
  _TRY { 		
    _asm {
      mov eax, 1  // set up CPUID to return processor version and features
                  // 0 = vendor string, 1 = version info, 2 = cache info
      _emit 0x0f  // code bytes = 0fh,  0a2h
      _emit 0xa2
      mov retval1, eax		
      mov retval2, edx
    }	
  } _EXCEPT(EXCEPTION_EXECUTE_HANDLER) { retval1 = retval2 = 0;}
  stdmem_mmx_available = retval1 && (retval2&0x800000);
  stdmem_sse_available = retval1 && (retval2&0x2000000);
#endif
  stdmem_cpuid_check_done = 1;
}
