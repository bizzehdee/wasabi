#ifndef _LOADER_PPM_H
#define _LOADER_PPM_H

#include <api/service/svcs/svc_imgload.h>
#ifdef WASABI_API_XML
class XmlReaderParams;
#else
#define XmlReaderParams void
#endif

class PpmLoad : public svc_imageLoaderI {
public:
  // service
  static const char *getServiceName() { return "PPM loader"; }

  virtual int isMine(const char *filename);
  virtual int getHeaderSize();
  virtual int testData(const void *data, int datalen);
//not yet  virtual int getDimensions(const void *data, int datalen, int *w, int *h);
  virtual ARGB32 *loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params=NULL);

private:
  void premultiply(ARGB32 *m_pBits, int nwords);
  ARGB32 *read_png(const void *data, int datalen, int *w, int *h, int dimensions_only);
};

#endif
