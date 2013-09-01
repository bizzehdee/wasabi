#include "precomp.h"

#include "blending.h"

#include "blendrect.h"

void blendRect(ARGB32 *src, int srcw, int srch, ARGB32 *dest, int destx, int desty, int destw, int desth, int alpha) {
  ASSERT(src != NULL);
  ASSERT(srcw > 0);
  ASSERT(srch > 0);
  ASSERT(dest != NULL);
  ASSERT(destw > 0);
  ASSERT(desth > 0);

  alpha = MINMAX(alpha, 0, 255);

  // stuff
  ARGB32 *in_line = src;
  ARGB32 *out_line = dest + destx + desty * destw;
  for (int y = 0; y < srch; y++) {
    int fy = y + desty;
    if (fy >= 0 && fy < desth) {
      ARGB32 *s = in_line;
      ARGB32 *d = out_line;
      #undef BLEND_JOB
      #define BLEND_JOB(version) \
      for (int x = 0; x < srcw; x++) { \
        int fx = x + destx; \
        if (fx >= 0 && fx < destw) \
          *d = Blenders::BLEND_ADJ3##version(*d, *s, alpha); \
        s++; \
        d++; \
      }
      MAKE_BLEND_JOB;
    }
    in_line += srcw;
    out_line += destw;
  }
}
