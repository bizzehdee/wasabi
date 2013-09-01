#ifndef __XMLOBJECT_H
#define __XMLOBJECT_H

#include "bfc/nsguid.h"
#include "bfc/ptrlist.h"
#include "bfc/dispatch.h"
#include "bfc/string/string.h"

#define XML_ATTRIBUTE_IMPLIED  0
#define XML_ATTRIBUTE_REQUIRED 1

class XmlObject : public Dispatchable {

 public:

	int setXmlParam(const char *name, const char *strvalue);
	int getNumXmlParams();
	const char *getXmlParamName(int n);
	const char *getXmlParamValue(int n);
  int getXmlParam(const char *paramname);

 enum {
  SETXMLPARAM=10,
  GETNUMXMLPARAMS=20,
  GETXMLPARAMNAME=30,
  GETXMLPARAMVALUE=40,
  GETXMLPARAM=50,
 };

};

inline int XmlObject::setXmlParam(const char *name, const char *strvalue) {
  return _call(SETXMLPARAM, 0, name, strvalue);
}

inline int XmlObject::getNumXmlParams() {
  return _call(GETNUMXMLPARAMS, 0);
}

inline const char *XmlObject::getXmlParamName(int n) {
  return _call(GETXMLPARAMNAME, (const char *)0, n);
}

inline const char *XmlObject::getXmlParamValue(int n) {
  return _call(GETXMLPARAMVALUE, (const char *)0, n);
}

inline int XmlObject::getXmlParam(const char *paramname) {
  return _call(GETXMLPARAM, 0, paramname);
}

class XmlObjectParam {

  public:

    XmlObjectParam(int xmlhandle, const char *xmlattribute, int xmlattributeid, int required=0) : handle(xmlhandle), xmlattributename(xmlattribute), attributeid(xmlattributeid), isrequired(required) {}
    virtual ~XmlObjectParam() {}

    const char *getXmlAttribute() { return xmlattributename; }
    int isRequired() { return isrequired; }
    int getXmlAttributeId() { return attributeid; }
    int getXmlHandle() { return handle; }
    void setLastValue(const char *val) { lastvalue = val; }
    const char *getLastValue() {return lastvalue; }

  private:
    String xmlattributename;
    int isrequired, attributeid;
    int handle;
    String lastvalue;
};

class XmlObjectParamSort {
public:
  static int compareItem(XmlObjectParam *p1, XmlObjectParam*p2) {
    return STRICMP(p1->getXmlAttribute(), p2->getXmlAttribute());
  }
  static int compareAttrib(const char *attrib, XmlObjectParam *item) {
    return STRICMP(attrib, item->getXmlAttribute());
  }
};

class XmlObjectI : public XmlObject {

 public:

  XmlObjectI();
  virtual ~XmlObjectI();

  virtual int setXmlParam(const char *name, const char *strvalue); // receives from system
  virtual int setXmlParamById(int xmlhandle, int xmlattributeid, const char *name, const char *strvalue); // distributes to inheritors
  virtual int getNumXmlParams();
  virtual const char *getXmlParamName(int n);
  virtual const char *getXmlParamValue(int n);
  virtual int getXmlParam(const char *paramname); 

  const char *getXmlParamByName(const char *paramname);

  void addParam(int xmlhandle, const char *xmlattribute, int xmlattributeid, int isrequired=0) {
    addXmlParam(xmlhandle, xmlattribute, xmlattributeid, isrequired);
  }

  void addXmlParam(int xmlhandle, const char *xmlattribute, int xmlattributeid, int isrequired=0);

  virtual int onUnknownXmlParam(const char *param, const char *value); 

  int newXmlHandle();

  private:

    XmlObjectParam *enumParam(int n) { return params[n]; }

    PtrListInsertSorted<XmlObjectParam, XmlObjectParamSort> params;
    int handlepos;

 protected:

  RECVS_DISPATCH;
};


#endif
