#include "precomp.h"

#include "solid.h"
#include <api/xml/xmlparams.h>

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(SolidGen_Svc);
DECLARE_SERVICETSINGLE(svc_imageGenerator, SolidImage);
END_SERVICES(SolidGen_Svc, _SolidGen_Svc);

#ifdef _X86_
extern "C" { int _link_SolidGen_Svc; }
#else
extern "C" { int __link_SolidGen_Svc; }
#endif

#endif

int SolidImage::testDesc(const char *desc) {
  return STRCASEEQL(desc, "$solid");
}

void premultiply(ARGB32 *m_pBits, int nwords) {
  for (; nwords > 0; nwords--, m_pBits++) {
    unsigned char *pixel = (unsigned char *)m_pBits;
    unsigned int alpha = pixel[3];
    if (alpha == 255) continue;
    pixel[0] = (pixel[0] * alpha) / 255.0f + 0.5f;	// blue
    pixel[1] = (pixel[1] * alpha) / 255.0f + 0.5f;	// green
    pixel[2] = (pixel[2] * alpha) / 255.0f + 0.5f;	// red
  }
}

ARGB32 *SolidImage::genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params) {
  int _w = ATOI(params->getItemValue("w"));
  if (_w == 0) _w = 1;
  int _h = ATOI(params->getItemValue("h"));
  if (_h == 0) _h = 1;
  if (_w <= 0 || _h <= 0) return NULL;
  ARGB32 color = BSWAP(WASABI_API_SKIN->parse(params->getItemValue("color"), "color")<<8);
  const char *alphastr = params->getItemValue("alpha");
  unsigned int alpha = (alphastr==NULL || *alphastr=='\0') ? 255 : ATOI(alphastr);
  color |= ((alpha & 0xff) << 24);

  premultiply(&color, 1);

#ifdef WASABI_COMPILE_MEMMGR
  ARGB32 *ret = (ARGB32*)WASABI_API_MEMMGR->sysMalloc(_w * _h * sizeof(ARGB32));
#else
  ARGB32 *ret = (ARGB32*)MALLOC_NOINIT(_w * _h * sizeof(ARGB32));
#endif

  MEMFILL<ARGB32>(ret, color, _w * _h);

  *w = _w;
  *h = _h;

  *has_alpha = (alpha == 255) ? 0 : 1;

  return ret;
}
