#ifndef _OSEDGE_H
#define _OSEDGE_H

#include <api/service/svcs/svc_imggen.h>

class OsEdgeImage : public svc_imageGeneratorI {
public:
  static const char *getServiceName() { return "OS Edge Generator"; }
  virtual int testDesc(const char *desc);
  virtual ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL);
};

#endif
