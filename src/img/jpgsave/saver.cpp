#include "precomp.h"

#include "saver.h"

extern "C" {
#include "libjpeg\jpeglib.h"
};

#define DEFAULT_QUAL 75

JpgSave::JpgSave() {
  quality = DEFAULT_QUAL;
}

int JpgSave::setBasicQuality(int q) {
  return quality = q;
}

struct bufdest {
  bufdest() {
    mgr.init_destination = init;
    mgr.empty_output_buffer = empty;
    mgr.term_destination = term;
  }

  static void init(j_compress_ptr cinfo) {
    empty(cinfo);
  }
  static boolean empty(j_compress_ptr cinfo) {
    bufdest *me = (bufdest*)cinfo->dest;
    me->mgr.next_output_byte = me->list.addItem(new MemBlock<unsigned char>(65536))->m();
    me->mgr.free_in_buffer = 65536;
    return TRUE;
  }
  static void term(j_compress_ptr cinfo) {
    bufdest *me = (bufdest*)cinfo->dest;
    MemBlock<unsigned char> *last = me->list.getLast();
    if (last == NULL) return;
    last->setSize(65536 - me->mgr.free_in_buffer);

    // now concat
    int siz = 0;
    foreach(me->list)
      siz += me->list.getfor()->getSize();
    endfor
    ASSERT(siz >= 0);
    me->final = (unsigned char *)api->sysMalloc(siz);
    me->final_size = 0;
    foreach(me->list)
      me->list.getfor()->copyTo(me->final+me->final_size);
      me->final_size += me->list.getfor()->getSize();
    endfor
    ASSERT(me->final_size == siz);
  }
  jpeg_destination_mgr mgr;
  PtrList< MemBlock<unsigned char> > list;
  unsigned char *final;
  int final_size;
};

unsigned char *JpgSave::convert(const ARGB32 *pixels, int w, int h, int *length) {
  unsigned char *ret = NULL;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_compress(&cinfo);

  cinfo.dest = (jpeg_destination_mgr *)new bufdest;

  cinfo.image_width = w;
  cinfo.image_height = h;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
  jpeg_start_compress(&cinfo, TRUE);
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = (unsigned char *)&pixels[cinfo.next_scanline * w * sizeof(ARGB32)];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);

  ret = ((bufdest*)cinfo.dest)->final;
  *length = ((bufdest*)cinfo.dest)->final_size;

  delete (bufdest*)cinfo.dest; cinfo.dest = NULL;

  jpeg_destroy_compress(&cinfo);

  return ret;
}
