#ifndef _SOLID_H
#define _SOLID_H

#include <api/service/svcs/svc_imggen.h>

class SolidImage : public svc_imageGeneratorI {
public:
  static const char *getServiceName() { return "Solid Color Image Generator"; }
  virtual int testDesc(const char *desc);
  virtual ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL);
};

#endif
