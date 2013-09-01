#include "precomp.h"

#include "loader_ppm.h"

// note: this has been extended to check for alpha channel
// the signifier is "P6a\n" if it's alphafied

int PpmLoad::isMine(const char *filename) {
  return STRCASEEQLSAFE(Std::extension(filename), "ppm");
}

int PpmLoad::getHeaderSize() {
  return 2;
}

int PpmLoad::testData(const void *data, int datalen) {
  if (datalen < getHeaderSize()) return 0;
  if (MEMCMP(data, "P6", 2)) return 0;
  const char *p = static_cast<const char *>(data);
  if ( (p[2] == 'a' && ISSPACE(p[3])) || ISSPACE(p[2]) ) return 1;
  return 0;
}

ARGB32 *PpmLoad::loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params) {
  if (!testData(data, datalen)) return NULL;
  const char *p = ((const char *)data)+2;	// skip P6
  const char *bounds = (const char *)data + datalen;
  int alphachan = (*p == 'a');	// check for magic 'a' meaning alpha channel
  if (alphachan) p++;
// skip whitespace
  while (ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
  if (*p == '#') {
// skip fuckin comment
    p++;
    while (*p != '\r' && *p != '\n') { p++; if (p >= bounds) return NULL; }
// skip whitespace
    while (ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
  }
// get width
  int _w = ATOI(p);
  if (_w <= 0) return NULL;
  while (!ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
// skip whitespace
  p++; while (ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
// get height
  int _h = ATOI(p);
  if (_h <= 0) return NULL;
  while (!ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
// skip whitespace
  p++; while (ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
// get maxval
  int mv = ATOI(p);
  if (mv <= 0 || mv > 255) return NULL;
  while (!ISSPACE(*p)) { p++; if (p >= bounds) return NULL; }
// skip single whitespace
  p++; if (p >= bounds) return NULL;

  int amt_to_copy = ((const char *)data + datalen) - p;
  if (amt_to_copy > _w * _h * (alphachan ? 4 : 3)) return NULL;
#ifdef WASABI_COMPILE_MEMMGR
  ARGB32 *ret = (ARGB32*)WASABI_API_MEMMGR->sysMalloc(_w * _h * sizeof(ARGB32));
#else //WASABI_COMPILE_MEMMGR
  ARGB32 *ret = (ARGB32*)MALLOC_NOINIT(_w * _h * sizeof(ARGB32));
#endif
  const unsigned char *src = (const unsigned char *)p;
  ARGB32 *dest = ret;
  for (int i = 0; i < _w*_h; i++) {
    unsigned int red, grn, blu, alpha;
    if (!alphachan) {
      alpha = 0xff;
      red = src[0], grn = src[1], blu = src[2];
    } else {
      alpha = src[0];
      red = src[1], grn = src[2], blu = src[3];
    }
    if (mv != 255) {
      float vv = (float)mv / 255.f;
      red = (int)floor(((float)red * vv)); ASSERT(red <= 255);
      grn = (int)floor(((float)grn * vv)); ASSERT(grn <= 255);
      blu = (int)floor(((float)blu * vv)); ASSERT(blu <= 255);
      alpha = (int)floor(((float)alpha * vv)); ASSERT(alpha <= 255);
    }
    *dest++ = (alpha << 24) | (red << 16) | (grn << 8) | blu;
    src += alphachan ? 4 : 3;
  }

//?  premultiplyARGB32(ret, _w * _h);

  *w = _w;
  *h = _h;

  return ret;
}
