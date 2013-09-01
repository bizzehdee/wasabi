#include "precomp.h"

#include "grad.h"

#include <api/xml/xmlparams.h>

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(GradientGen_Svc);
DECLARE_SERVICETSINGLE(svc_imageGenerator, GradientImage);
END_SERVICES(GradientGen_Svc, _GradientGen_Svc);

#ifdef _X86_
extern "C" { int _link_GradientGen_Svc; }
#else
extern "C" { int __link_GradientGen_Svc; }
#endif

#endif


int GradientImage::testDesc(const char *desc) {
  return STRCASEEQL(desc, "$gradient");
}

ARGB32 *GradientImage::genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params) {
  int _w = ATOI(params->getItemValue("w"));
  if (_w == 0) _w = 1;
  int _h = ATOI(params->getItemValue("h"));
  if (_h == 0) _h = 1;
  if (_w <= 0 || _h <= 0) return NULL;

#ifdef WASABI_COMPILE_MEMMGR
  ARGB32 *ret = (ARGB32*)WASABI_API_MEMMGR->sysMalloc(_w * _h * sizeof(ARGB32));
#else
  ARGB32 *ret = (ARGB32*)MALLOC_NOINIT(_w * _h * sizeof(ARGB32));
#endif

  setX1((float)ATOF(params->getItemValue("gradient_x1")));
  setY1((float)ATOF(params->getItemValue("gradient_y1")));
  setX2((float)ATOF(params->getItemValue("gradient_x2")));
  setY2((float)ATOF(params->getItemValue("gradient_y2")));

  setPoints(params->getItemValue("points"));

  setMode(params->getItemValue("mode"));

  setReverseColors(TRUE);	// cuz we're imggen

  setAntialias(ATOI(params->getItemValue("antialias")));

  renderGradient(ret, _w, _h);

  *w = _w;
  *h = _h;
  *has_alpha = 1;	// will be optimized anyway

  return ret;
}
