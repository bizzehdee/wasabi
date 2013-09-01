#include "precomp.h"

#include "contrast.h"

// 10-may-2004 BU created

#include <draw/argb32.h>

void applyContrast(ARGB32 *bits, int w, int h, int pct) {
  if (pct <= 0) return;
  unsigned char chart[256];
  int bot = (int)(255. * (pct / 100.));
  int top = 255-bot;
  for (unsigned int i = 0; i <= 255; i++) {
    if (i < (unsigned int)bot) chart[i] = 0;
    else if (i > (unsigned int)top) chart[i] = 255;
    else {
      chart[i] = (int)(((float)(i - bot) / (float)(top - bot))*255.f);
    }
  }

  for (int l = w*h; l; l--, bits++) {
    ARGB32 color = *bits;
    *bits = MKARGB32(chart[ALP(color)],chart[RED(color)],chart[GRN(color)],chart[BLU(color)]);
  }
}
