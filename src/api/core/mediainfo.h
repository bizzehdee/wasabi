#ifndef _MEDIAINFO_H
#define _MEDIAINFO_H

#include <bfc/tlist.h>
#include <bfc/string/string.h>
#include <bfc/dispatch.h>
#include <api/service/svcs/svc_fileread.h>

// classes used to pass metadatas around all core converters

class NOVTABLE MediaInfo : public Dispatchable {
protected:
  MediaInfo() {} // protect constructor

public:
  
  enum {
    INFO_NOEDIT=0,

    INFO_EDIT_BOOL=1<<0,
    INFO_EDIT_STRING=1<<1, // param1=max. length
    INFO_EDIT_INT=1<<2,    // param1=minimum, param2=maximum
    INFO_EDIT_COMBO=1<<3,  // param1=pointer to a table of \0 delimited strings (ends with \0\0)
    INFO_TEXT=1<<4,        // param1=text
    //INFO_EDIT_SLIDER,    // param1=minimum, param2=maximum

    INFO_VISIBLE=1<<8,

    INFO_DISPLAY_TIME=1<<12, // time in ms

    INFO_NOSENDCB=1<<20,
  } InfoAttributes;

  // the "attributes" parameter can be a combination of the above enums. ex: INFO_EDIT_STRING|INFO_VISIBLE
  void setData(const char *name, const char *value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { _voidcall(SETDATA,name,value,attributes,param1,param2); }
  void setDataInt(const char *name, int value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { _voidcall(SETDATAINT,name,value,attributes,param1,param2); }
  void setDataLong(const char *name, long value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { _voidcall(SETDATALONG,name,value,attributes,param1,param2); }
  void setDataPtr(const char *name, void * value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { _voidcall(SETDATAPTR,name,value,attributes,param1,param2); }

  const char *getData(const char *name) { return _call(GETDATA,"",name); }
  int getDataInt(const char *name) { return _call(GETDATAINT,0,name); }
  long getDataLong(const char *name) { return _call(GETDATALONG,0,name); }
  void * getDataPtr(const char *name) { return _call<void *>(GETDATAPTR,0,name); }

  const char *enumDataName(int offset) { return _call(ENUMDATANAME,(const char *)NULL,offset); }
  const char *enumDataValue(int offset) { return _call(ENUMDATAVALUE,(const char *)NULL,offset); }
  int enumDataAttributes(int offset, int *param1=NULL, int *param2=NULL) { return _call(ENUMDATAATTRIBUTES,0,offset,param1,param2); }
  int getAttribute(const char *name, int *param1=NULL, int *param2=NULL) { return _call(GETATTRIBUTE,0,name,param1,param2); }

  const char *getFilename() { return _call(GETFILENAME,""); }
  void setFilename(const char *name) { _voidcall(SETFILENAME,name); }

  const char *getTitle() { return _call(GETTITLE,""); }
  void setTitle(const char *str) { _voidcall(SETTITLE,str); }
  const char *getTitle2() { return _call(GETTITLE2,""); }
  void setTitle2(const char *str) { _voidcall(SETTITLE2,str); }
  const char *getInfo() { return _call(GETINFO,""); }
  void setInfo(const char *str) { _voidcall(SETINFO,str); }
  const char *getUrl() { return _call(GETURL,""); }
  void setUrl(const char *str) { _voidcall(SETURL,str); }
  int getLength() { return _call(GETLENGTH,-1); }
  void setLength(int val) { _voidcall(SETLENGTH,val); }

  svc_fileReader *getReader() { return _call(GETREADER,(svc_fileReader *)NULL); }
  void setReader(svc_fileReader *r) { _voidcall(SETREADER,r); }

  void warning(const char *str) { _voidcall(WARNING,str); }
  void error(const char *str) { _voidcall(ERR,str); }
  void status(const char *str) { _voidcall(STATUS,str); }
  
  enum {
    GETFILENAME=100,
    SETFILENAME=110,

    SETDATA=205,
    SETDATA2=200,
    SETDATAPTR=217,
    SETDATALONG=216,
    SETDATAINT=215,
    SETDATAINT2=210,
    GETDATA=220,
    GETDATAINT=230,
    GETDATALONG=236,
    GETDATAPTR=237,
    ENUMDATANAME=240,
    ENUMDATAVALUE=250,
    ENUMDATAATTRIBUTES=260,
    GETATTRIBUTE=270,
    
    GETTITLE=300,
    SETTITLE=310,
    GETTITLE2=320,
    SETTITLE2=330,
    GETINFO=340,
    SETINFO=350,
    GETURL=360,
    SETURL=370,
    GETLENGTH=380,
    SETLENGTH=390,

    GETREADER=500,
    SETREADER=510,

    WARNING=600,
    ERR=610,
    STATUS=620,
  };
};

class MediaInfoData {
public:
  MediaInfoData(const char *pname, const char *pvalue, int pattribs, int pparam1, int pparam2) 
    : name(pname), value(pvalue), attributes(pattribs),param1(pparam1),param2(pparam2) { }
  const char *getName() { return name; }
  const char *getValue() { return value; }
  int getAttributes() { return attributes; }
  int getParam1() { return param1; }
  int getParam2() { return param2; }
  void setValue(const char *pvalue) { value=pvalue; }
  void setAttributes(int pvalue, int pparam1, int pparam2) { attributes=pvalue; param1=pparam2; param2=pparam2; }
private:
  String name;
  RecycleString value;
  int attributes,param1,param2;
};

class MediaInfoDataSort {
public:
  static int compareAttrib(const char *attrib, void *item) {
    return STRICMP(attrib, ((MediaInfoData *)item)->getName());
  }
  static int compareItem(void *i1, void *i2) {
    return STRICMP(((MediaInfoData *)i1)->getName(), ((MediaInfoData *)i2)->getName());
  }
};


class MediaInfoI : public MediaInfo {
public:
  MediaInfoI() { }
  virtual ~MediaInfoI() { infos.deleteAll(); }

  // functions used to set any metadata
  // "visible" sets whether it will appear or not in the media info editor
  // "editable" sets whether it will be editable or not in the media info editor
  void setData(const char *name, const char *value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { 
    if(!value) value="";
    MediaInfoData *i=infos.findItem(name);
    if(!i) infos.addItem(new MediaInfoData(name,value,attributes,param1,param2));
    else {
      i->setValue(value);
      i->setAttributes(attributes,param1,param2);
    }
    if(!(attributes & MediaInfo::INFO_NOSENDCB)) mediaInfo_onDataSet(name,value);
  }
  void setData2(const char *name, const char *value) { setData(name,value); }
  void setDataInt(const char *name, int value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { char tmp[64]; sprintf(tmp, "%d", value); setData(name,tmp, attributes, param1, param2); }
  void setDataLong(const char *name, long value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { char tmp[64]; sprintf(tmp, "%d", value); setData(name,tmp, attributes, param1, param2); }
  void setDataPtr(const char *name, void * value, int attributes=MediaInfo::INFO_NOEDIT, int param1=0, int param2=0) { char tmp[64]; sprintf(tmp, "%p", value); setData(name,tmp, attributes, param1, param2); }

  const char *getData(const char *name) { 
    MediaInfoData *i=infos.findItem(name);
    if(!i) return "";
    else return i->getValue();
  }
  void setDataInt2(const char *name, int value) { setDataInt(name,value); }
  int getDataInt(const char *name) { return ATOI(getData(name)); }
  long getDataLong(const char *name) { return atol(getData(name)); }
  void *getDataPtr(const char *name) { void *p=NULL; sscanf(getData(name), "%p", &p); return p; }

  const char *enumDataName(int offset) { 
    MediaInfoData *i=infos[offset];
    if(i) return i->getName();
    return NULL;
  }
  const char *enumDataValue(int offset) { 
    MediaInfoData *i=infos[offset];
    if(i) return i->getValue();
    return NULL;
  }
  int enumDataAttributes(int offset, int *param1=NULL, int *param2=NULL) {
    MediaInfoData *i=infos[offset];
    if(i) {
      if(param1) *param1=i->getParam1();
      if(param2) *param2=i->getParam2();
      return i->getAttributes();
    }
    return 0;
  }
  int getAttribute(const char *name, int *param1=NULL, int *param2=NULL) {
    MediaInfoData *i=infos.findItem(name);
    if(i) {
      if(param1) *param1=i->getParam1();
      if(param2) *param2=i->getParam2();
      return i->getAttributes();
    }
    return 0;
  }

  // some basic helper functions
  const char *getFilename() { return getData("filename"); }
  void setFilename(const char *name) { setData("filename",name,INFO_VISIBLE); }
  const char *getTitle() { return getData("title"); }
  void setTitle(const char *str) { setData("title",str,INFO_VISIBLE); }
  const char *getTitle2() { return getData("title2"); }
  void setTitle2(const char *str) { setData("title2",str,INFO_VISIBLE); }
  const char *getInfo() { return getData("info"); }
  void setInfo(const char *str) { setData("info",str,INFO_VISIBLE); }
  const char *getUrl() { return getData("url"); }
  void setUrl(const char *str) { setData("url",str,INFO_VISIBLE); }
  int getLength() { return getDataInt("length"); }
  void setLength(int val) { setDataInt("length",val,INFO_VISIBLE|INFO_DISPLAY_TIME); }
  svc_fileReader *getReader() { return reinterpret_cast<svc_fileReader *>(getDataPtr("reader")); }
  void setReader(svc_fileReader *r) { setDataPtr("reader",reinterpret_cast<void *>(r)); }
  
  // status infos
  void warning(const char *str) { setData("warning",str); } /* warning (something recoverable, like file not found, etc) */
  void error(const char *str) { setData("error",str); }     /* playback should stop (soundcard driver error, etc) */
  void status(const char *str) { setData("status",str); }   /* status update (nothing really wrong) */

  // callbacks
  virtual void mediaInfo_onDataSet(const char *name, const char *value) { }

protected:
  PtrListInsertSorted<MediaInfoData,MediaInfoDataSort> infos;

#undef CBCLASS
#define CBCLASS MediaInfoI
START_DISPATCH;
  CB(GETFILENAME, getFilename);
  VCB(SETFILENAME, setFilename);

  VCB(SETDATA, setData);
  VCB(SETDATA2, setData2);
  VCB(SETDATAINT, setDataInt);
  VCB(SETDATAINT2, setDataInt2);
  VCB(SETDATALONG, setDataLong);
  VCB(SETDATAPTR, setDataPtr);
  CB(GETDATA, getData);
  CB(GETDATAINT, getDataInt);
  CB(GETDATALONG, getDataLong);
  CB(GETDATAPTR, getDataPtr);
  CB(ENUMDATANAME, enumDataName);
  CB(ENUMDATAVALUE, enumDataValue);
  CB(ENUMDATAATTRIBUTES, enumDataAttributes);
  CB(GETATTRIBUTE, getAttribute)

  CB(GETTITLE, getTitle);
  VCB(SETTITLE, setTitle);
  CB(GETTITLE2, getTitle2);
  VCB(SETTITLE2, setTitle2);
  CB(GETINFO, getInfo);
  VCB(SETINFO, setInfo);
  CB(GETURL, getUrl);
  VCB(SETURL, setUrl);
  CB(GETLENGTH, getLength);
  VCB(SETLENGTH, setLength);

  CB(GETREADER, getReader);
  VCB(SETREADER, setReader);

  VCB(WARNING, warning);
  VCB(ERR, error);
  VCB(STATUS, status);
END_DISPATCH;
#undef CBCLASS
};

#endif
