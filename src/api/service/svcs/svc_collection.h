#ifndef _SVC_COLLECTION_H
#define _SVC_COLLECTION_H

#include <bfc/dispatch.h>
#include <bfc/ptrlist.h>
#include <bfc/pair.h>
#include <bfc/string/string.h>
#include <api/xml/xmlreader.h>
#include <api/service/services.h>

class CollectionElement;
class svc_collectionI;

class svc_collection : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::COLLECTION; }
  int testTag(const char *xmltag);
  void addElement(const char *id, const char *includepath, int removalid, XmlReaderParams *params); 
  void removeElement(int recored_value);
  void removeAllElements();
  int getNumElements();
  int getNumElementsUnique();
  CollectionElement *enumElement(int n, int *ancestor);
  CollectionElement *enumElementUnique(int n, int *ancestor);
  CollectionElement *getElement(const char *id, int *ancestor);
  CollectionElement *getAncestor(CollectionElement *e);

  enum {
    COLLECTION_TESTTAG=10,
    COLLECTION_ADDELEMENT=20,
    COLLECTION_REMOVEELEMENT=30,
    COLLECTION_REMOVEALLELEMENTS=35,
    COLLECTION_GETNUMELEMENTS=40,
    COLLECTION_GETNUMELEMENTSUNIQUE=50,
    COLLECTION_ENUMELEMENT=60,
    COLLECTION_ENUMELEMENTUNIQUE=70,
    COLLECTION_GETELEMENT=80,
  };
};

inline int svc_collection::testTag(const char *xmltag) {
  return _call(COLLECTION_TESTTAG, 0, xmltag);
}

inline void svc_collection::addElement(const char *id, const char *includepath, int incrementalremovalid, XmlReaderParams *params) {
  _voidcall(COLLECTION_ADDELEMENT, id, includepath, incrementalremovalid, params);
}

inline void svc_collection::removeElement(int removalid) {
  _voidcall(COLLECTION_REMOVEELEMENT, removalid);
}

inline void svc_collection::removeAllElements() {
  _voidcall(COLLECTION_REMOVEALLELEMENTS);
}

inline int svc_collection::getNumElements() {
  return _call(COLLECTION_GETNUMELEMENTS, 0);
}

inline int svc_collection::getNumElementsUnique() {
  return _call(COLLECTION_GETNUMELEMENTSUNIQUE, 0);
}

inline CollectionElement *svc_collection::enumElement(int n, int *ancestor) {
  return _call(COLLECTION_ENUMELEMENT, (CollectionElement *)NULL, n, ancestor);
}

inline CollectionElement *svc_collection::enumElementUnique(int n, int *ancestor) {
  return _call(COLLECTION_ENUMELEMENTUNIQUE, (CollectionElement *)NULL, n, ancestor);
}

inline CollectionElement *svc_collection::getElement(const char *id, int *ancestor) {
  return _call(COLLECTION_GETELEMENT, (CollectionElement *)NULL, id, ancestor);
}

class SortPairString {
public:
  static int compareItem(Pair<String, String> *p1, Pair<String, String> *p2) {
    return STRICMP(p1->a, p2->a);
  }
  static int compareAttrib(const char *attrib, Pair<String, String> *item) {
    return STRICMP(attrib, item->a);
  }
};

class CollectionElement : public Dispatchable {
  public:
    const char *getId();
    const char *getParamValue(const char *param, CollectionElement **item=NULL);
    int getParamValueInt(const char *param);
    const char *getIncludePath(const char *param=NULL); 
    CollectionElement *getAncestor();

    enum {
      COLLECTIONELEMENT_GETID=10,
      COLLECTIONELEMENT_GETPARAMVALUE=20,
      COLLECTIONELEMENT_GETPARAMVALUEINT=30,
      COLLECTIONELEMENT_GETANCESTOR=40,
      COLLECTIONELEMENT_GETINCLUDEPATH=50,
    };
};

inline const char *CollectionElement::getId() {
  return _call(COLLECTIONELEMENT_GETID, (const char *)NULL);
}

inline const char *CollectionElement::getParamValue(const char *param, CollectionElement **item) {
  return _call(COLLECTIONELEMENT_GETPARAMVALUE, (const char *)NULL, param, item);
}

inline int CollectionElement::getParamValueInt(const char *param) {
  return _call(COLLECTIONELEMENT_GETPARAMVALUEINT, 0, param);
}

inline CollectionElement *CollectionElement::getAncestor() {
  return _call(COLLECTIONELEMENT_GETANCESTOR, (CollectionElement *)NULL);
}

inline const char *CollectionElement::getIncludePath(const char *param) {
  return _call(COLLECTIONELEMENT_GETINCLUDEPATH, (const char *)NULL, param);
}

class CollectionElementI : public CollectionElement {
  public:
    CollectionElementI(svc_collectionI *collectionI, const char *id, XmlReaderParams *params, int seccount, const char *includepath);
    virtual ~CollectionElementI();

    virtual const char *getId();
    virtual const char *getParamValue(const char *param, CollectionElement **item=NULL);
    virtual int getParamValueInt(const char *param);
    virtual CollectionElement *getAncestor();
    const char *getIncludePath(const char *param=NULL);  // null returns last override's include path

    int getSecCount();

  protected:
    RECVS_DISPATCH;

    PtrListQuickSorted < Pair < String, String >, SortPairString > params;
    String id;
    int seccount;
    svc_collectionI *collection;
    String path;
};

class SortCollectionElementsI {
public:
  static int compareItem(CollectionElementI *p1, CollectionElementI *p2) {
    int r = STRICMP(p1->getId(), p2->getId());
    if (r == 0) {
      if (p1->getSecCount() < p2->getSecCount()) return -1;
      if (p1->getSecCount() > p2->getSecCount()) return 1;
      return 0;
    }
    return r;
  }
  static int compareAttrib(const char *attrib, CollectionElementI *item) {
    return STRICMP(attrib, item->getId());
  }
};

// derive from this one
class svc_collectionI : public svc_collection {
public:
  svc_collectionI();
  virtual ~svc_collectionI();
  virtual int testTag(const char *xmltag)=0;
  virtual void addElement(const char *id, const char *includepath, int incrementalremovalid, XmlReaderParams *params);
  virtual void removeElement(int removalid);
  virtual void removeAllElements();
  virtual int getNumElements();
  virtual int getNumElementsUnique();
  virtual CollectionElement *enumElement(int n, int *ancestor);
  virtual CollectionElement *enumElementUnique(int n, int *ancestor);
  virtual CollectionElement *getElement(const char *id, int *ancestor);
  virtual CollectionElement *getAncestor(CollectionElement *e);

protected:
  RECVS_DISPATCH;

  PtrListQuickMultiSorted < CollectionElementI, SortCollectionElementsI > elements;
  int count;
};

#include <api/service/servicei.h>
template <class T>
class CollectionCreator : public waServiceFactoryTSingle<svc_collection, T> {};

template <char TAG[]>
class CollectionSvc : public svc_collectionI {
  public:
    int testTag(const char *xmltag) {
      if (STRCASEEQL(xmltag, TAG)) return 1;
      return 0;
    }
  static const char *getServiceName() { return StringPrintf("Collection Service for \"%s\"", TAG); }
};

template <class T>
class CollectionSvc2 : public svc_collectionI {
  public:
    int testTag(const char *xmltag) {
      if (STRCASEEQL(xmltag, T::collection_getXmlTag())) return 1;
      return 0;
    }
  static const char *getServiceName() { return StringPrintf("Collection Service for \"%s\"", T::collection_getXmlTag()); }
};


#include <api/service/svc_enum.h>

class CollectionSvcEnum : public SvcEnumT<svc_collection> {
public:
  CollectionSvcEnum(const char *xmltag) : tag(xmltag) {}
protected:
  virtual int testService(svc_collection *svc) {
    return (svc->testTag(tag));
  }
private:
  String tag;
};

#endif
