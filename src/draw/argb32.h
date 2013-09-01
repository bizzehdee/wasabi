#ifndef _WASABI_ARGB32_H
#define _WASABI_ARGB32_H

#define RED(a) (((a)>>16)&0xff)
#define GRN(a) (((a)>>8)&0xff)
#define BLU(a) (((a)&0xff))
#define ALP(a) (((a)>>24))

typedef unsigned long ARGB32;

static inline ARGB32 MKARGB32(unsigned int a, unsigned int r, unsigned int g,
  unsigned int b) {
  a &= 0xff;
  r &= 0xff;
  g &= 0xff;
  b &= 0xff;
  return (a<<24) | (r << 16) | (g << 8) | b;
}

static inline ARGB32 MKARGB32_NOMASK(unsigned int a, unsigned int r, unsigned int g,
  unsigned int b) {
// note this version does not mask its inputs (for speed)
  return (a<<24) | (r << 16) | (g << 8) | b;
}

static inline ARGB32 MKARGB32_ALPHA(unsigned int a) {
  a &= 0xff;
  return a<<24;
}

#endif
