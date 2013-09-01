#ifndef _LOADERJPG_H
#define _LOADERJPG_H

#include <api/service/svcs/svc_imgload.h>

class JpgLoad : public svc_imageLoaderI {
public:
  static const char *getServiceName() { return "JPEG loader"; }

  virtual int isMine(const char *filename);

  virtual int testData(const void *data, int datalen);
  virtual int getDimensions(const void *data, int datalen, int *w, int *h);
  virtual ARGB32 *loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params=NULL);
};
#endif
