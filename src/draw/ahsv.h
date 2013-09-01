#ifndef _AHSV_H
#define _AHSV_H

#include "argb32.h"

template <class T=float>
class AHSV {
public:
  AHSV(ARGB32 val) {
#if 1
    const T one_over_255 = 1.f/255.f;
    T r = RED(val) * one_over_255;
    T g = GRN(val) * one_over_255;
    T b = BLU(val) * one_over_255;
#else
    T r = RED(val) / 255.;
    T g = GRN(val) / 255.;
    T b = BLU(val) / 255.;
#endif
    a = ALP(val);

    T min = MIN(MIN(r, g), b);
    v = MAX(MAX(r, g), b);
    T diff = v - min;
    if (v <= 0.0001) {
      h = 0;	// h is undefined actually, who knows
    } else {
      s = diff / v;
//      const T one_over_diff = 1.f/diff;
      if (r == v)
        h = (g - b) / diff;
//        h = (g - b) * one_over_diff;
      else if (g == v)
        h = 2 + (b - r) / diff;
//        h = 2 + (b - r) * one_over_diff;
      else if (b == v)
        h = 4 + (r - g) / diff;
//        h = 4 + (r - g) * one_over_diff;
      h *= 60;
      if (fastfisneg(h)) h += 360;
    }
  }

  operator ARGB32() {
    T r, g, b;
    if (/*!s*/!fastfnz(s)) {
      r = g = b = v;
    } else {
      if (h == 360) h = 0;
      T fh = h / 60;
      int i = (int)(fh);	// can't be fastfrnd
//CUT      double f = fh - i;
#define _P (v * (1 - s))
//      double p = v * (1 - s);
#define _Q (v * (1 - s * (fh - i)))
//      double q = v * (1 - s * (fh - i));
#define _T (v * (1 - (s * (1 - (fh - i)))))
//      double t = v * (1 - (s * (1 - (fh - i))));
// todo: optimize this into a table of double *'s
// then it's just r = *table[i][0];
#if 0
{ &v, &t, &p }, // #0
{ &q, &v, &p }, // #1
{ &p, &v, &t }, // #2
{ &p, &v, &t }, // #3
{ &t, &p, &v }, // #4
{ &v, &p, &q }, // #5
#endif
      switch (i) {
        default:
          // fall thru
        case 0:
          r = v;
          g = /*t;*/ _T ;
          b = /*p;*/ _P ;
        break;
        case 1:
          r = /*q;*/ _Q ;
          g = v;
          b = /*p;*/ _P ;
        break;
        case 2:
          r = /*p;*/ _P ;
          g = v;
          b = /*t;*/ _T ;
        break;
        case 3:
          r = /*p;*/ _P ;
          g = /*q;*/ _Q ;
          b = v;
        break;
        case 4:
          r = /*t;*/ _T ;
          g = /*p;*/ _P ;
          b = v;
        break;
        case 5:
          r = v;
          g = /*p;*/ _P ;
          b = /*q;*/ _Q ;
        break;
      }
    }
    // MKARGB32 will clamp for us
    return MKARGB32(a,
                    fastfrnd(r*255), 
                    fastfrnd(g*255), 
                    fastfrnd(b*255) );
//    return MKARGB32(a,
//                    (int)(r*255), 
//                    (int)(g*255), 
//                    (int)(b*255) );
  }

  int a;	// 0..255
  T h, s, v;	// 0..1
};

#endif
