#ifndef _XMLREADER_H
#define _XMLREADER_H

#include <bfc/ptrlist.h>
#include <bfc/string/string.h>
#include <bfc/stack.h>
#include <bfc/dispatch.h>
#include <api/xml/xmlparams.h>

class svc_xmlProvider;

  typedef enum {
    TYPE_CLASS_CALLBACK=1,
    TYPE_STATIC_CALLBACK,
  } xmlreader_callbackType;

class xmlreader_cb_struct {
public:
  xmlreader_cb_struct(const char *m, xmlreader_callbackType t, XmlReaderCallback *cb) : matchstr(m), type(t), callback(cb) { }

  String matchstr;
  xmlreader_callbackType type;
  XmlReaderCallback * callback;
};


class XmlReaderCallback : public Dispatchable {
protected:
  XmlReaderCallback() {} // protect constructor

public:
  void xmlReaderOnStartElementCallback(const char *xmlpath, const char *xmltag, XmlReaderParams *params) {
    _voidcall(ONSTARTELEMENT, xmlpath, xmltag, params);
  }
  void xmlReaderOnEndElementCallback(const char *xmlpath, const char *xmltag) {
    _voidcall(ONENDELEMENT, xmlpath, xmltag);
  }
  void xmlReaderOnCharacterDataCallback(const char *xmlpath, const char *xmltag, const char *str) {
    _voidcall(ONCHARDATA, xmlpath, xmltag, str);
  }
  int xmlReaderDisplayErrors() {
    return _call(DISPLAYERRORS, 1);
  }
  int xmlReaderIncludeFiles() {
    return _call(INCLUDEFILES, 1);
  }
  void xmlReaderOnError(const char *filename, int linenum, const char *incpath, int errcode, const char *errstr) {
    _voidcall(ONERROR, filename, linenum, incpath, errcode, errstr);
  }
  const char *xmlGetIncludePath() {
    return _call(GETINCPATH, (const char *)NULL);
  }
  const char *xmlGetFileName() {
    return _call(GETFILENAME, (const char *)NULL);
  }
  int xmlGetFileLine() {
    return _call(GETFILELINE, -1);
  }
  void xmlSetParserHandle(void *_handle) { 
    _voidcall(SETHANDLE, _handle);
  }
  void *xmlGetParserHandle() { 
    return _call(GETHANDLE, (void *)NULL);
  }

  enum {
    ONSTARTELEMENT  = 100,
    ONENDELEMENT    = 200,
    ONCHARDATA      = 300,
    DISPLAYERRORS   = 1000,
    INCLUDEFILES    = 1100,
    ONERROR         = 1200,
    GETINCPATH      = 1300,
    GETFILENAME     = 1400,
    GETFILELINE     = 1500,
    SETHANDLE       = 1600,
    GETHANDLE       = 1700,
  };
};

class XmlReaderCallbackI : public XmlReaderCallback {
public:
  XmlReaderCallbackI() : handle(NULL) {}

  virtual void xmlReaderOnStartElementCallback(const char *xmlpath, const char *xmltag, XmlReaderParams *params) { }
  virtual void xmlReaderOnEndElementCallback(const char *xmlpath, const char *xmltag) { }
  virtual void xmlReaderOnCharacterDataCallback(const char *xmlpath, const char *xmltag, const char *str) { }
  virtual int xmlReaderDisplayErrors() { return 1; }
  virtual int xmlReaderIncludeFiles() { return 0; }	//BU for safety this should be off by default I think
  virtual void xmlReaderOnError(const char *filename, int linenum, const char *incpath, int errcode, const char *errstr) { }
  virtual const char *xmlGetIncludePath(); 
  virtual const char *xmlGetFileName(); 
  virtual int xmlGetFileLine();
  virtual void xmlSetParserHandle(void *_handle) { handle = _handle; }
  virtual void *xmlGetParserHandle() { return handle; }

#undef CBCLASS
#define CBCLASS XmlReaderCallbackI
START_DISPATCH;
      VCB(ONSTARTELEMENT, xmlReaderOnStartElementCallback);
      VCB(ONENDELEMENT, xmlReaderOnEndElementCallback);
      VCB(ONCHARDATA, xmlReaderOnCharacterDataCallback);
      CB(DISPLAYERRORS, xmlReaderDisplayErrors);
      CB(INCLUDEFILES, xmlReaderIncludeFiles);
      VCB(ONERROR, xmlReaderOnError);
      CB(GETINCPATH, xmlGetIncludePath);
      CB(GETFILENAME, xmlGetFileName);
      CB(GETFILELINE, xmlGetFileLine);
      VCB(SETHANDLE, xmlSetParserHandle);
      CB(GETHANDLE, xmlGetParserHandle);
END_DISPATCH;
#undef CBCLASS
private:
  void *handle;
};

class XmlFileHandle {
public:
  void *m_parser;
  FILE *m_file;
  String m_filename;
  String m_incpath;
  XmlReaderCallback *m_callback;
};

class XmlReader {
public:
  // matchstr is a regexp string such as "WinampAbstractionLayer/Layer[a-z]"
  // or "Winamp*Layer/*/Layout"
  static void registerCallback(const char *matchstr, XmlReaderCallback *callback);
  static void registerCallback(const char *matchstr, void (*static_callback)(int start, const char *xmlpath, const char *xmltag, XmlReaderParams *params));

  static void unregisterCallback(XmlReaderCallback *callback);
  static void unregisterCallback(void (*static_callback)(int start, const char *xmlpath, const char *xmltag, XmlReaderParams *params));

  // if only_this_class param is specified, only this class will be called back
  // returns 1 on success, 0 on error
  static int loadFile(const char *filename, const char *incpath = NULL, void *only_this_callback = NULL, const char **params = NULL, int isinclude=0);
  static const char *getIncludePath();
  static const char *getFileName();
  static int getFileLine(void *xmlparserhandle=NULL);

  static long startXmlLoad(const char *filename, XmlReaderCallback *only_this_object = NULL);
  static int loadChunk(long handle, int chunksize);
  static void close(long handle);

  static int getNumCallbacks() { return callback_list.getNumItems(); }

  enum {
    ALL_MATCHING,
    LAST_MATCHING,
  } XmlReaderModeEnum;
  static void setCallbackMode(int mode);
  static int getCallbackMode();

private:
  static void parserCallbackOnStartElement(void *userData, const char *name, const char **parms);
  static void parserCallbackOnEndElement(void *userData, const char *nameC);
  static void parserCallbackOnCharacterData(void *userData, const char *str, int len);

  static const char *peekParam(const char *txt, const char **parms);
  static int doLoadFile(FILE *fp, svc_xmlProvider *svc, const char *filename, const char *incpath, void *only_this_callback, const char **params = NULL);

  static int parseBuffer(void *parser, const char *xml, int size, int final, XmlReaderCallback *only_this_callback, const char *filename, const char *incpath);

  static PtrList<xmlreader_cb_struct> callback_list;
  static String currentpos;
  static String cont_char;
  static Stack<String *> include_stack;
  static Stack<String *> file_stack;
  static int curline;
  static void *defaultparser;
  static String last_error;
  static int cbmode;
};

#endif
