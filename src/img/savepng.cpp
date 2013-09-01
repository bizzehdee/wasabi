#include "precomp.h"

#include <img/pngload/pnglib/png.h>

#include "savepng.h"

int savePNG(const char *filename, ARGB32 *bits, int w, int h, int wantalpha) {
  FILE *fp=NULL;
  png_structp png_ptr=NULL;
  png_infop info_ptr=NULL;

  fp = FOPEN(filename, "wb");
  if (fp == NULL) return 0;

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
     //png_voidp user_error_ptr, user_error_fn, user_warning_fn);
     NULL, NULL, NULL);

  if (png_ptr == NULL) {
    FCLOSE(fp);
    return 0;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    FCLOSE(fp);
    png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    /* If we get here, we had a problem reading the file */
    FCLOSE(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  #define BITDEPTH 8
  png_set_IHDR(png_ptr, info_ptr, w, h, BITDEPTH, wantalpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_set_bgr(png_ptr);

  // kill alpha bytes if not wanted
  if (!wantalpha) png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

  MemBlock<ARGB32 *> row_pointers(h);
  for (int k = 0; k < h; k++)
    row_pointers.m()[k] = bits + k*w;

  png_write_image(png_ptr, (unsigned char **)row_pointers.m());

  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  FCLOSE(fp);

  return 1;
}
