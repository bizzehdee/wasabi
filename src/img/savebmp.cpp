#include "precomp.h"

#include "savebmp.h"

int saveBMP(const char *filename, ARGB32 *bits, int w, int h) {
  if (w <= 0 || h <= 0 || bits == NULL) return 0;
  FILE *fp = FOPEN(filename, "wb");
  if (fp == NULL) return 0;

  BITMAPFILEHEADER bfh;
  ZERO(bfh);
  bfh.bfType = 19778;	// magic number that identifies .bmp
  bfh.bfSize = 0;
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  FWRITE(&bfh, sizeof(bfh), 1, fp);

  BITMAPINFOHEADER bih;
  ZERO(bih);
  bih.biSize = sizeof(bih);
  bih.biWidth = w;
  bih.biHeight = h;
  bih.biPlanes = 1;
  bih.biBitCount = 24;
  bih.biCompression = BI_RGB;
  // the rest are zeroed
  FWRITE(&bih, sizeof(bih), 1, fp);

  ARGB32 *line_in = bits+(h-1)*w;
  MemBlock<char> line_out(w * 3);
  for (int i = 0; i < h; i++) {
    char *src = (char*)line_in;
    char *dest = line_out.m();
    for (int j = 0; j < w; j++) {
      dest[0] = src[0];
      dest[1] = src[1];
      dest[2] = src[2];
      dest += 3;
      src += 4;
    }
    FWRITE(line_out.m(), w*3, 1, fp);
    int bytes_written = w * 3;
    while (bytes_written % 4) {	// pad to 4-byte boundary
      unsigned char zero=0;
      FWRITE(&zero, 1, 1, fp);
      bytes_written++;
    }
    line_in -= w;
  }

  FCLOSE(fp);

  return 1;
}
