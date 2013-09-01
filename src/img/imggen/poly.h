#ifndef _POLY_H
#define _POLY_H

#include <api/service/svcs/svc_imggen.h>

class PolyImage : public svc_imageGeneratorI {
public:
  static const char *getServiceName() { return "Polygon Image Generator"; }
  virtual int testDesc(const char *desc);
  virtual ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL);
};

#endif
