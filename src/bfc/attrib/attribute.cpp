#include "precomp.h"

#include "attribute.h"
#include "cfgitemi.h"

#ifdef WASABI_COMPILE_CONFIG
#include <api/config/api_config.h>
#endif

static int id_gen;

Attribute::Attribute(const char *newname, const char *_desc) :
  Named(newname), desc(_desc), cfgitemi(NULL), private_storage(NULL) {
  if (newname == NULL || *newname == '\0')
    Named::setName(StringPrintf("#attribute%d", id_gen++), TRUE);
  if ((getName() != NULL && *getName() == '#') || 
#ifndef WASABI_COMPILE_CONFIG
    1
#else
    0
#endif
  ) {
    private_storage = new String;	// no central config api, use private storage
  }
}

Attribute::~Attribute() {
  delete private_storage;
}

const char *Attribute::getAttributeName() {
  return Named::getName();
}

const char *Attribute::getAttributeDesc() {
  return desc;
}

int Attribute::getValueAsInt() {
  char buf[1024]="";
  getData(buf, sizeof buf);
  return ATOI(buf);
}

int Attribute::setValueAsInt(int newval, bool def) {
  return setData(StringPrintf(newval), def);
}

double Attribute::getValueAsDouble() {
  char buf[1024];
  getData(buf, sizeof buf);
  return ATOF(buf);
}

double Attribute::setValueAsDouble(double newval, bool def) {
  return setData(StringPrintf(newval), def);
}

int Attribute::getDataLen() {
#ifdef WASABI_COMPILE_CONFIG
  if (private_storage != NULL)
#endif
    return private_storage->len()+1;

#ifdef WASABI_COMPILE_CONFIG
  ASSERTPR(WASABI_API_CONFIG != NULL, "getDataLen() before API");
  int r = WASABI_API_CONFIG->getStringPrivateLen(mkTag());
  if (r < 0) {
    r = default_val.len()+1;
  }
  return r;
#endif
}

int Attribute::getData(char *data, int data_len) {
  if (data == NULL || data_len < 0) return 0;
#ifdef WASABI_COMPILE_CONFIG
  if (private_storage != NULL) {
#endif
    if (private_storage->isempty()) {
      if (data_len >= 1) {
        *data = 0;
        return 1;
      }
      return 0;
    }
    private_storage->strncpyTo(data, data_len);
//CUT    STRNCPY(data, private_storage->getValue(), data_len);
    return MIN(private_storage->len(), data_len);
#ifdef WASABI_COMPILE_CONFIG
  }
  ASSERTPR(WASABI_API_CONFIG != NULL, "can't get without api");
  if (WASABI_API_CONFIG == NULL) return 0;
//CUT //  StringPrintf s("fetching %s, dl %d, defval %s\n", getName(), data_len, default_val.getValue());
//CUT //  DebugString(s);
  int r = WASABI_API_CONFIG->getStringPrivate(mkTag(), data, data_len, default_val.isempty() ? "" : default_val.getValue());
//CUT //  StringPrintf s2("ret '%s'\n", data);
//CUT //  DebugString(s2);
  return r;
#endif
}

int Attribute::setData(const char *data, bool def) {
  if (def) {	// setting default value
    default_val = data;
//CUT #ifndef WASABI_COMPILE_CONFIG
    if (private_storage != NULL) private_storage->setValue(data);
//CUT #endif
    return 1;
  }
#ifdef WASABI_COMPILE_CONFIG
  ASSERTPR(WASABI_API_CONFIG != NULL, "can't set data before api");
  if (WASABI_API_CONFIG == NULL) return 0;
#endif

  int r = setDataNoCB(data);
  if (r && cfgitemi != NULL) cfgitemi->cfgitem_onAttribSetValue(this);
  return r;
}

int Attribute::setDataNoCB(const char *data) {
#ifdef WASABI_COMPILE_CONFIG
  if (private_storage != NULL) {
#endif
    private_storage->setValue(data);
#ifdef WASABI_COMPILE_CONFIG
  } else {
    WASABI_API_CONFIG->setStringPrivate(mkTag(), data);
  }
#endif
//BU: shouldn't this be in setData()? wtf?
  dependent_sendEvent(Attribute::depend_getClassGuid(), Event_DATACHANGE);
  return 1;
}

void Attribute::setCfgItem(CfgItemI *item) {
  delete private_storage; private_storage = NULL;
  ASSERT(cfgitemi == NULL || item == NULL);
  cfgitemi = item;
#ifdef WASABI_COMPILE_CONFIG	// otherwise we always use private storage
  if (cfgitemi != NULL) {
    if (cfgitemi->cfgitem_usePrivateStorage()) private_storage = new String;
  }
#endif
}

String Attribute::mkTag() {
  String ret(cfgitemi ? cfgitemi->cfgitem_getPrefix() : NULL);
  if (ret.notempty() && ret.lastChar() != '/') ret.cat("/");
  ret.cat(getName());
  return ret;
}

void Attribute::disconnect() {
  setCfgItem(NULL);
}
