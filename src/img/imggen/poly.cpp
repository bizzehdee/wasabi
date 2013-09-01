#include "precomp.h"

#include "poly.h"

#include <api/xml/xmlparams.h>

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(PolyGen_Svc);
DECLARE_SERVICETSINGLE(svc_imageGenerator, PolyImage);
END_SERVICES(PolyGen_Svc, _PolyGen_Svc);

#ifdef _X86_
extern "C" { int _link_PolyGen_Svc; }
#else
extern "C" { int __link_PolyGen_Svc; }
#endif

#endif

int PolyImage::testDesc(const char *desc) {
  return STRCASEEQL(desc, "$polygon");
}

void premultiply(ARGB32 *m_pBits, int nwords);

#include <bfc/draw/drawpoly.h>

ARGB32 *PolyImage::genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params) {
  int _w = ATOI(params->getItemValue("w"));
  if (_w == 0) _w = 1;
  int _h = ATOI(params->getItemValue("h"));
  if (_h == 0) _h = 1;
  if (_w <= 0 || _h <= 0) return NULL;

  const char *bgcolorstr = params->getItemValue("bgcolor");
  ARGB32 bgcolor = (bgcolorstr == NULL || *bgcolorstr=='\0') ? 0 : BSWAP(WASABI_API_SKIN->parse(params->getItemValue("bgcolor"), "color")<<8);
  const char *bgalphastr = params->getItemValue("bgalpha");
  unsigned int bgalpha = (bgalphastr==NULL || *bgalphastr=='\0') ? 0 : ATOI(bgalphastr);
  bgcolor |= ((bgalpha & 0xff) << 24);

  premultiply(&bgcolor, 1);

#ifdef WASABI_COMPILE_MEMMGR
  ARGB32 *ret = (ARGB32*)WASABI_API_MEMMGR->sysMalloc(_w * _h * sizeof(ARGB32));
#else
  ARGB32 *ret = (ARGB32*)MALLOC_NOINIT(_w * _h * sizeof(ARGB32));
#endif

  MEMFILL<ARGB32>(ret, bgcolor, _w * _h);

  Draw::drawPointList(ret, _w, _h, params->getItemValue("points"));

  *w = _w;
  *h = _h;
  *has_alpha = 1;
  return ret;
}
