#ifndef _F_SATURATE_H
#define _F_SATURATE_H

#include "filter.h"
#include "ahsv.h"

class SaturationFn {
public:
  static inline ARGB32 execPixel(ARGB32 src, float p1, float p2) {
    AHSV<float> p(src);
    if (fastfisneg(p1))	// p1 < 0
      p.s += p1 * p.s;
    else {
      if (fastfabove(p.s, 0.01f)) p.s += p1 * (1 - p.s);
    }
    return p;
  }
};

class SaturationFnPos {
public:
  static inline ARGB32 execPixel(ARGB32 src, float p1, float p2) {
    AHSV<float> p(src);
//    if (fastfisneg(p1))	// p1 < 0
//      p.s += p1 * p.s;
//    else {
      if (fastfabove(p.s, 0.01f)) p.s += p1 * (1 - p.s);
//    }
    return p;
  }
};

class SaturationFnNeg {
public:
  static inline ARGB32 execPixel(ARGB32 src, float p1, float p2) {
    AHSV<float> p(src);
//    if (fastfisneg(p1))	// p1 < 0
      p.s += p1 * p.s;
//    else {
//      if (fastfabove(p.s, 0.01f)) p.s += p1 * (1 - p.s);
//    }
    return p;
  }
};

#endif
