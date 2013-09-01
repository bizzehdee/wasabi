#ifndef _SVC_IMGLOAD_H
#define _SVC_IMGLOAD_H

#include <api/service/services.h>

#include <bfc/platform/platform.h>
#include <bfc/dispatch.h>
#include <bfc/string/string.h>

#ifdef WASABI_API_XML
class XmlReaderParams;
#else
#define XmlReaderParams void
#endif

class NOVTABLE svc_imageLoader : public Dispatchable {
public:
  static GUID getServiceType() { GUID g = WaSvc::IMAGELOADER; return g; }

  // assuming there is an extension of this type, is it yours?
  int isMine(const char *filename);

  // returns how many bytes needed to get image info
  int getHeaderSize();

  // test image data, return TRUE if you can load it
  int testData(const void *data, int datalen);

  // just gets the width and height from the data, if possible
  int getDimensions(const void *data, int datalen, int *w, int *h);

  // converts the data into pixels, use api->sysFree to deallocate
  ARGB32 *loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params=NULL);

  enum {
    ISMINE=50,
    GETHEADERSIZE=100,
    TESTDATA=200,
    GETDIMENSIONS=300,
    LOADIMAGE=400,
  };
};

inline int svc_imageLoader::isMine(const char *filename) {
  return _call(ISMINE, 0, filename);
}

inline int svc_imageLoader::getHeaderSize() {
  return _call(GETHEADERSIZE, -1);
}

inline int svc_imageLoader::testData(const void *data, int datalen) {
  return _call(TESTDATA, 0, data, datalen);
}

inline int svc_imageLoader::getDimensions(const void *data, int datalen, int *w, int *h) {
  return _call(GETDIMENSIONS, 0, data, datalen, w, h);
}

inline ARGB32 *svc_imageLoader::loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params) {
  return _call(LOADIMAGE, (ARGB32*)0, data, datalen, w, h, params);
}

// derive from this one
class NOVTABLE svc_imageLoaderI : public svc_imageLoader {
public:
  virtual int isMine(const char *filename)=0;
  // return the header size needed to get w/h and determine if it can be loaded
  virtual int getHeaderSize() { return -1; }//don't know
  // test image data, return TRUE if you can load it
  virtual int testData(const void *data, int datalen)=0;
  // just gets the width and height from the data, if possible
  virtual int getDimensions(const void *data, int datalen, int *w, int *h) { return 0; }
  // convert the data into pixels, use api->sysMalloc to allocate the space
  virtual ARGB32 *loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params=NULL)=0;

protected:
  RECVS_DISPATCH;
};

#include <api/service/svc_enum.h>

class ImgLoaderEnum : public SvcEnumT<svc_imageLoader> {
public:
  ImgLoaderEnum(unsigned char *data, int datalen) : mem(datalen, data) { }
  ImgLoaderEnum(const char *filename) : fname(filename) { }

protected:
  virtual int testService(svc_imageLoader *svc) {
    int imbfn = !svc->isMine(fname.vs());
    if (mem.isNull()) {
      return imbfn;
    } else {
      return svc->testData(mem, mem.getSizeInBytes());
    }
  }

private:
  String fname;
  MemBlock<unsigned char> mem;
};

#endif
