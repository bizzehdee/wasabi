#ifndef _SVC_IMGWRITE_H
#define _SVC_IMGWRITE_H

#include <bfc/std.h>
#include <bfc/dispatch.h>
#include <bfc/parse/pathparse.h>

class NOVTABLE svc_imageWriter /*: public Dispatchable*/ {
public:
  static FOURCC getServiceType() { return WaSvc::IMAGEWRITER; }

  virtual const char *getImageTypeName()=0;
  virtual const char *getExtensions()=0;	// "jpg;jpeg" etc

  virtual int isLossless()=0;
  virtual int setBasicQuality(int q)=0;	// 0..100

  // free the returned buffer with sysFree()
  virtual unsigned char *convert(const ARGB32 *pixels, int w, int h, int *length)=0;
};

class svc_imageWriterI : public svc_imageWriter {
public:
  virtual const char *getImageTypeName()=0;
  virtual int isLossless()=0;
  virtual int setBasicQuality(int q)=0;	// 0..100
  virtual unsigned char *convert(const ARGB32 *pixels, int w, int h, int *length)=0;
};

#include <bfc/svc_enum.h>

class ImgWriterEnum : public SvcEnumT<svc_imageWriter> {
public:
  ImgWriterEnum(const char *_ext=NULL) : ext(_ext) { }

protected:
  virtual int testService(svc_imageWriter *svc) {
    if (ext.isempty()) return 1;
    else {
      PathParser pp(svc->getExtensions(), ";");
      for (int i = 0; i < pp.getNumStrings(); i++)
        if (STRCASEEQL(ext, pp.enumString(i))) return 1;
      return 0;
    }
  }
  String ext;
};

#endif
