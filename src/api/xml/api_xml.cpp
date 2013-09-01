#include <precomp.h>
#include "api_xml.h"

#ifdef CBCLASS
#undef CBCLASS
#endif
#define CBCLASS api_xmlI
START_DISPATCH;
  VCB(API_XML_REGISTERCALLBACK, xmlreader_registerCallback);
  VCB(API_XML_UNREGISTERCALLBACK, xmlreader_unregisterCallback);
  CB(API_XML_LOADFILE, xmlreader_loadFile);
  CB(API_XML_STARTXMLLOAD, xmlreader_startXmlLoad);
  CB(API_XML_LOADCHUNK, xmlreader_loadChunk);
  VCB(API_XML_CLOSE, xmlreader_close);
END_DISPATCH;
