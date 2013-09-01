#include <precomp.h>

#include "std_math.h"

unsigned long COMEXP BSWAP_C(unsigned long input) {
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64
  _asm {
    mov eax, input
    bswap eax
    mov input, eax
  };
  return input;
#else
#  ifdef GCC
  __asm__ volatile (
		    "\tmov %0, %%eax\n"
		    "\tbswap %%eax\n"
		    : "=m" (input)
		    : 
		    : "%eax" );
#  else
  unsigned char ret[4];
  unsigned char *s = reinterpret_cast<unsigned char *>(&input);
  ret[0] = s[3];
  ret[1] = s[2];
  ret[2] = s[1];
  ret[3] = s[0];
  return *reinterpret_cast<unsigned long *>(ret);
#  endif
#endif
}

// this is a slow version - there is an optimized version in Canvas::premultiply
void StdMath::premultiplyARGB32(ARGB32 *words, int nwords) {
  for (; nwords > 0; nwords--, words++) {
    unsigned char *pixel = (unsigned char *)words;
    unsigned int alpha = pixel[3];
    if (alpha == 255) continue;
    pixel[0] = (unsigned char)((pixel[0] * alpha) / 255.0f + 0.5f);	// blue
    pixel[1] = (unsigned char)((pixel[1] * alpha) / 255.0f + 0.5f);	// green
    pixel[2] = (unsigned char)((pixel[2] * alpha) / 255.0f + 0.5f);	// red
  }
}

int StdMath::mulDiv(int m1, int m2, int d) {
#ifdef WIN32
  return MulDiv(m1, m2, d);
#elif defined(LINUX) && !defined(WASABI_PLATFORM_DARWIN)
  __asm__ volatile (
		    "mov %0, %%eax\n"
		    "mov %1, %%ebx\n"
		    "mov %2, %%ecx\n"
		    "mul %%ebx\n"
		    "div %%ecx\n"
		    : : "m" (m1), "m" (m2), "m" (d) 
		    : "%eax", "%ebx", "%ecx", "%edx" );
#elif defined(WASABI_PLATFORM_DARWIN)
  __int64 m = m1*m2;
  return m/d;
#else
#error port me!
#endif
}

bool StdMath::isOdd(int n) {
  if((n % 2) == 0) return false;
  return true;
}

bool StdMath::isEven(int n) {
  return !isOdd(n);
}
