#ifndef _STD_MATH_H
#define _STD_MATH_H

#include <bfc/std.h>

// FG> doesn't work for me without this include (error C2039: 'sin' : is not a member of '`global namespace'')
#ifdef __cplusplus
#include <math.h>
static inline double SIN(double a) { return ::sin(a); }
static inline double COS(double a) { return ::cos(a); }
static inline double SQRT(double a) { return ::sqrt(a); }

static inline unsigned long BSWAP(unsigned long input) {
#if defined(WIN32) && !defined(_WIN64)
  _asm {
    mov eax, input
    bswap eax
    mov input, eax
  };
  return input;
#else
#  if defined(GCC) && !defined(WASABI_PLATFORM_DARWIN)
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
#else
#define SIN(a) sin(a)
#define COS(a) sin(a)
#define SQRT(a) sqrt(a)
unsigned long BSWAP_C(unsigned long input);
#define BSWAP BSWAP_C
#endif

#ifdef __cplusplus

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

// neat trick from C++ book, p. 161
template<class T> inline T MAX(T a, T b) { return a > b ? a : b; }
template<class T> inline T MIN(T a, T b) { return a > b ? b : a; }
template<class T> inline T MINMAX(T a, T minval, T maxval) {
  return (a < minval) ? minval : ( (a > maxval) ? maxval : a );
}


#ifdef ABS
#undef ABS
#endif

#ifdef SQR
#undef SQR
#endif

#ifdef CMP3
#undef CMP3
#endif

#ifdef SIGN
#undef SIGN
#endif

// and a couple of my own neat tricks :) BU
template<class T> inline T ABS(T a) { return a < 0 ? -a : a; }
template<class T> inline T SQR(T a) { return a * a; }
template<class T> inline int CMP3(T a, T b) {
  if (a < b) return -1;
  if (a == b) return 0;
  return 1;
}
template<class T> inline T SIGN(T a) {
  return (a < 0) ? -T(1) : ( (a == 0) ? T(0) : T(1) );
}

static inline RGB24 RGBTOBGR(RGB24 col) {
  return ((col & 0xFF00FF00) | ((col & 0xFF0000) >> 16) | ((col & 0xFF) << 16));
}
static inline RGB24 BGRTORGB(RGB24 col) { return RGBTOBGR(col); }
static inline ARGB32 BGRATOARGB(ARGB32 col) { return RGBTOBGR(col); }

namespace StdMath {
  int mulDiv(int m1, int m2, int d);
  bool isOdd(int n);
  bool isEven(int n);
  void premultiplyARGB32(ARGB32 *words, int nwords=1);
};

#ifdef WIN32
#include <float.h>
#endif
#define STDMATH_FLOAT_MAX FLT_MAX

// The following functions are optimized versions of various floating point operations, be careful when using them
// as most do not support the whole floating point domain. Check the comments at the top of each for info on
// limitations

// You may turn off any use of these functions by defining NO_FAST_FLOAT_OPS

#ifdef WASABI_PLATFORM_POSIX
//until someone ports this to gcc inline asm
#define NO_FAST_FLOAT_OPS
#endif

#ifdef _WIN64
//x64 compiler does not support inline asm
#define NO_FAST_FLOAT_OPS
#endif

#ifndef NO_FAST_FLOAT_OPS

#ifdef WIN32
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
#endif

// fast float round - cannot handle NaN
// this is much faster than doing (int)(x+0.5f)
int __inline fastfrnd(float x) {
  __declspec(align(4)) int n;
  __asm fld x;
  __asm fistp n;
  __asm mov eax, n;
}

// fast float nonzero test - this cannot handle NaN
// faster than doing (x != 0)
int __inline fastfnz(float x) {
  __asm {
    mov eax, x
    add eax, eax
  }
}

// fast negative float test 
// faster than doing (x == 0) 
int __inline fastfisneg(float x) {
  __asm {
    mov eax, x
    cmp eax, 80000000h
    ja isneg
    xor eax, eax
    isneg:
  }
}

// fast float above test
// faster than doing (a > b), works only for values >= 0
int __inline fastfabove(float a, float b) {
  __asm {
    mov eax, a
    mov ecx, b
    cmp eax, ecx
    ja aaboveb
    xor eax, eax
    aaboveb:
    // if we jumped, eax must be > 0 since it's above b
  }
}

// fast float above or equal test, works only for values >= 0
// faster than doing (a >= b)
int __inline fastfaboveequal(float a, float b) {
  __asm {
    mov eax, a
    mov ecx, b
    cmp eax, ecx
    jae aaboveeqb
    xor eax, eax
    jmp doneaaboveeqb
    aaboveeqb:
    inc eax // can't assume eax is > 0 since it could have been >= (b == 0)
    jmp doneaaboveeqb
    doneaaboveeqb:
  }
}

// fast float invert
// 2.12 times faster than doing (1/p), but less precise
float __inline fastfinv(float p) {
  int _i = 2 * 0x3F800000 - *(int *)&p;
  float r = *(float *)&_i; 
  return r * (2.0f - p * r); 
}

#ifdef WIN32
#pragma warning( pop ) 
#endif

#else // NO_FAST_FLOAT_OPS

#define fastfrnd(x) ((int)((x)+0.5f))
#define fastfnz(x) ((x)!=0.0f)
#define fastfisneg(x) ((x)<0.0f)
#define fastfabove(a,b) ((a)>(b))
#define fastfinv(p) (1.0f/(p))

#endif // NO_FAST_FLOAT_OPS


#else // not __cplusplus
void premultiplyARGB32(ARGB32 *words, int nwords);

#endif

#endif
