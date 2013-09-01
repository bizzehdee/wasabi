#include <precomp.h>
#include "xmlapi.h"
#include <api/xml/xmlreader.h>

api_xml *xmlApi = NULL;

void XmlApi::xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback) {
  if(callback && matchstr) XmlReader::registerCallback(matchstr, callback);
}

void XmlApi::xmlreader_unregisterCallback(XmlReaderCallback *callback) {
  if(callback) XmlReader::unregisterCallback(callback);
}

int XmlApi::xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object) {
  if (filename == NULL) return 0;
  String path;
  if(STRSTR(filename,"/") || STRSTR(filename,"\\")) {
    path=filename;
    char *p=(char *)path.getValue();
    *((char *)Std::scanstr_back(p,"/\\",p)+1)=0;
  }
  return XmlReader::loadFile(Std::filename(filename), path.vs(), only_this_object);
}

int XmlApi::xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object) {
  if(filename) return XmlReader::startXmlLoad(filename,only_this_object);
  return 0;
}

int XmlApi::xmlreader_loadChunk(int handle, int chunksize) {
  if(handle) return XmlReader::loadChunk(handle,chunksize);
  return 0;
}

void XmlApi::xmlreader_close(int handle) {
  if(handle) XmlReader::close(handle);
}
