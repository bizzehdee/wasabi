#ifndef __XMLAPI_H
#define __XMLAPI_H

#include <api/xml/api_xml.h>

class XmlApi : public api_xmlI {
    virtual void xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback);
    virtual void xmlreader_unregisterCallback(XmlReaderCallback *callback);
    virtual int xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object = NULL);
    virtual int xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object = NULL);
    virtual int xmlreader_loadChunk(int handle, int chunksize);
    virtual void xmlreader_close(int handle);
};

#endif
