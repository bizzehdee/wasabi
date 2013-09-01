#ifndef _SAVERJPG_H
#define _SAVERJPG_H

#include <api/services/svcs/svc_imgwrite.h>

class JpgSave : public svc_imageWriterI {
public:
  static const char *getServiceName() { return "JPEG writer"; }
  JpgSave();

  virtual const char *getImageTypeName() { return "JPEG"; }
  virtual const char *getExtensions() { return "jpg;jpeg;jpe"; }

  virtual int isLossless() { return FALSE; }
  virtual int setBasicQuality(int q);

  virtual unsigned char *convert(const ARGB32 *pixels, int w, int h, int *length);
private:
  int quality;
};

#endif
