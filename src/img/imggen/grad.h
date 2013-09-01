#ifndef _GRAD_H
#define _GRAD_H

#include <api/service/svcs/svc_imggen.h>
#include <bfc/draw/gradient.h>

class GradientImage : public svc_imageGeneratorI, public Gradient {
public:
  static const char *getServiceName() { return "Gradient Image Generator"; }
  virtual int testDesc(const char *desc);
  virtual ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL);
};

#endif
