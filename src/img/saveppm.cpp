#include "precomp.h"

#include "saveppm.h"

int savePPM(const char *savefn, ARGB32 *bits, int w, int h, int alphachan) {
  FILE *fp = FOPEN(savefn, "wb");
  if (fp == NULL) return 0;
  StringPrintf header("P6%s\n%d %d\n255\n", alphachan ? "a" : "", w, h);
  FPRINTF(fp, "%s", header.v());
  ARGB32 *src = bits;
  MemBlock<unsigned char> outrow(w * (alphachan ? 4 : 3));
  for (int y = 0; y < h; y++) {
    unsigned char *dst = outrow.m();
    for (int x = 0; x < w; x++, src++) {
      ARGB32 val = BSWAP(*src);
      char *p = (char *)&val;
      if (alphachan) {
        *(ARGB32 *)dst = val;
        dst += sizeof(ARGB32);
      } else {
        dst[0] = p[1];
        dst[1] = p[2];
        dst[2] = p[3];
        dst += 3;
      }
    }
    FWRITE(outrow.m(), outrow.getSizeInBytes(), 1, fp);
  }
  FCLOSE(fp);
  return 1;
}
