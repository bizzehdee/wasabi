#ifndef __API_XML_H
#define __API_XML_H

#include <bfc/dispatch.h>

class XmlReaderCallback;

#include <api/service/services.h>

class api_xml : public Dispatchable {
  public:
    void xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback);
    void xmlreader_unregisterCallback(XmlReaderCallback *callback);
    int xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object = NULL);
    int xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object = NULL);
    int xmlreader_loadChunk(int handle, int chunksize);
    void xmlreader_close(int handle);

  enum {
    API_XML_REGISTERCALLBACK = 0,
    API_XML_UNREGISTERCALLBACK = 10,
    API_XML_LOADFILE = 20,
    API_XML_STARTXMLLOAD = 30,
    API_XML_LOADCHUNK = 40,
    API_XML_CLOSE = 50,
  };
};

inline void api_xml::xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback) {
  _voidcall(API_XML_REGISTERCALLBACK, matchstr, callback);
}

inline void api_xml::xmlreader_unregisterCallback(XmlReaderCallback *callback) {
  _voidcall(API_XML_UNREGISTERCALLBACK, callback);
}

inline int api_xml::xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object) {
  return _call(API_XML_LOADFILE, 0, filename, only_this_object);
}

inline int api_xml::xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object) {
  return _call(API_XML_STARTXMLLOAD, (int)0, filename, only_this_object);
}

inline int api_xml::xmlreader_loadChunk(int handle, int chunksize) {
  return _call(API_XML_LOADCHUNK, (int)0, handle, chunksize);
}

inline void api_xml::xmlreader_close(int handle) {
  _voidcall(API_XML_CLOSE, handle);
}

class api_xmlI : public api_xml {
  public:
    static const char *getServiceName() { return "XML Parser API"; }
    static GUID getServiceType() { return WaSvc::UNIQUE; }
    
    virtual void xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback)=0;
    virtual void xmlreader_unregisterCallback(XmlReaderCallback *callback)=0;
    virtual int xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object = NULL)=0;
    virtual int xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object = NULL)=0;
    virtual int xmlreader_loadChunk(int handle, int chunksize)=0;
    virtual void xmlreader_close(int handle)=0;

  protected:
    RECVS_DISPATCH;
};

// {D327E1FB-21A8-4144-888E-3011E878F4A8}
static const GUID xmlApiServiceGuid = 
{ 0xd327e1fb, 0x21a8, 0x4144, { 0x88, 0x8e, 0x30, 0x11, 0xe8, 0x78, 0xf4, 0xa8 } };

extern api_xml *xmlApi;

#endif
