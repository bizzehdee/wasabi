#include "precomp.h"

#include "loader.h"

#include <bfc/std.h>
#include <api/api.h>

#include "pnglib/png.h"

int PngLoad::isMine(const char *filename) {
  return STRCASEEQLSAFE(Std::extension(filename), "png");
}

int PngLoad::getHeaderSize() {
  return 8;
}

int PngLoad::testData(const void *data, int datalen) {
  unsigned char *ptr = (unsigned char *)data;
  return !png_sig_cmp(ptr, 0, datalen);
}

typedef struct {
  const unsigned char *data;
  int pos;
  int datalen;
} my_read_info;

static void my_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
  my_read_info *mri = (my_read_info *)png_get_io_ptr(png_ptr);
  if (mri->datalen - mri->pos < (int)length)
    length = mri->datalen - mri->pos;
  MEMCPY(data, mri->data + mri->pos, length);
  mri->pos += length;
}

ARGB32 *PngLoad::loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params) {
  ARGB32 *pixels = read_png(data, datalen, w, h, FALSE);

  if (pixels == NULL) return NULL;

#ifndef NO_PREMUL
  StdMath::premultiplyARGB32(pixels, *w * *h);
#endif

  return pixels;
}

int PngLoad::getDimensions(const void *data, int datalen, int *w, int *h) {
  //doesn't work return (read_png(data, datalen, w, h, TRUE) == reinterpret_cast<ARGB32*>(0xffffffff));
  return 0;// not supported by this loader
}

// From libpng example.c
ARGB32 *PngLoad::read_png(const void *data, int datalen, int *w, int *h, int dimensions_only) {
   png_structp png_ptr;
   png_infop info_ptr;
   unsigned int sig_read = 0;
   png_uint_32 width=0, height=0;
   int bit_depth, color_type, interlace_type;
  my_read_info mri;
  mri.data = static_cast<const unsigned char *>(data);
  mri.pos = 0;
  mri.datalen = datalen;

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      return NULL;
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return NULL;
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
   if (setjmp(png_ptr->jmpbuf))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      /* If we get here, we had a problem reading the file */
      return NULL;
   }

  png_set_read_fn(png_ptr, &mri, my_png_read_data);

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, NULL, NULL);

//DebugString("png: got %d x %d", width, height);

  if (w) *w = (int)width;
  if (h) *h = (int)height;

  ARGB32 *retval = reinterpret_cast<ARGB32*>(0xffffffff);
  png_bytep *row_pointers = NULL;

  if (!dimensions_only) {

    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (bit_depth < 8) png_set_packing(png_ptr);

    /* flip the RGB pixels to BGR (or RGBA to BGRA) */
    png_set_bgr(png_ptr);

    /* Expand paletted colors into true RGB triplets */
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_gray_1_2_4_to_8(png_ptr);

    png_set_gray_to_rgb(png_ptr);
    png_set_palette_to_rgb(png_ptr);
   
    /* Expand paletted or RGB images with transparency to full alpha channels
     * so the data will be available as RGBA quartets.
     */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_ptr);
    }

    /* Add filler (or alpha) byte (before/after each RGB triplet) */
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

    /* Optional call to gamma correct and add the background to the palette
     * and update info structure.  REQUIRED if you are expecting libpng to
     * update the palette for you (ie you selected such a transform above).
     */
    png_read_update_info(png_ptr, info_ptr);

    /* Allocate the memory to hold the image using the fields of info_ptr. */

    /* The easiest way to read the image: */
    row_pointers = (png_bytep*)MALLOC_NOINIT(sizeof(png_bytep*)*height);

#ifdef WASABI_COMPILE_MEMMGR
    ARGB32 *bytes = (ARGB32 *)WASABI_API_MEMMGR->sysMalloc(width*height*4);
#else //WASABI_COMPILE_MEMMGR
    ARGB32 *bytes = (ARGB32 *)MALLOC_NOINIT(width*height*4);
#endif //WASABI_COMPILE_MEMMGR

    for (unsigned int row = 0; row < height; row++) {
      row_pointers[row] = ((unsigned char *)bytes) + width * 4 * (row);
    }

    /* Now it's time to read the image.  One of these methods is REQUIRED */
    png_read_image(png_ptr, row_pointers);

    /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end(png_ptr, info_ptr);

    retval = bytes;
  
    FREE(row_pointers);
  }

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

  /* that's it */
  return retval;
}
