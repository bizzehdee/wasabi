#include "precomp.h"

#include "cfgitemi.h"

#include <bfc/attrib/attrcb.h>
#include <bfc/attrib/attribs.h>

#include <bfc/std.h>
#include <bfc/memblock.h>

CfgItemI::CfgItemI(const char *name, GUID guid) :
  Named(name), myguid(guid), parent_guid(INVALID_GUID) { }

CfgItemI::~CfgItemI() {
  cfgitem_deregisterAll();
}

const char *CfgItemI::cfgitem_getName() {
  return Named::getName();
}

GUID CfgItemI::cfgitem_getGuid() {
  return myguid;
}

void CfgItemI::cfgitem_setPrefix(const char *_prefix) {
  prefix = _prefix;
}

const char *CfgItemI::cfgitem_getPrefix() {
  return prefix;
}

int CfgItemI::cfgitem_getNumAttributes() {
  return attributes.getNumItems();
}

const char *CfgItemI::cfgitem_enumAttribute(int n) {
  Attribute *attr = attributes[n];
  if (attr) return attr->getAttributeName();
  return NULL;
}

const char *CfgItemI::cfgitem_getConfigXML() {
  return cfgxml;
}

int CfgItemI::cfgitem_getNumChildren() {
  return children.getNumItems();
}

CfgItem *CfgItemI::cfgitem_enumChild(int n) {
  return children[n];
}

GUID CfgItemI::cfgitem_getParentGuid() {
  return parent_guid;
}

void CfgItemI::cfgitem_onRegister() {
#ifdef WASABI_COMPILE_CONFIG
  foreach(children)
    WASABI_API_CONFIG->config_registerCfgItem(children.getfor());
  endfor
#endif
}
void CfgItemI::cfgitem_onDeregister() {
#ifdef WASABI_COMPILE_CONFIG
  foreach(children)
    WASABI_API_CONFIG->config_deregisterCfgItem(children.getfor());
  endfor
#endif
}

Attribute *CfgItemI::getAttributeByName(const char *name) {
  Attribute *attr;
  foreach(attributes)
    attr = attributes.getfor();
    if (!STRICMP(name, attr->getAttributeName())) return attr;
  endfor
  return NULL;
}

int CfgItemI::cfgitem_getAttributeType(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return AttributeType::NONE;
  return attr->getAttributeType();
}

const char *CfgItemI::cfgitem_getAttributeConfigGroup(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return NULL;
  return attr->getConfigGroup();
}

int CfgItemI::cfgitem_getDataLen(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  return attr->getDataLen();
}

int CfgItemI::cfgitem_getData(const char *name, char *data, int data_len) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  return attr->getData(data, data_len);
}

int CfgItemI::cfgitem_setData(const char *name, const char *data) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  int ret = attr->setDataNoCB(data);
  if (ret) cfgitem_onAttribSetValue(attr);
  return ret;
}

int CfgItemI::cfgitem_onAttribSetValue(Attribute *attr) {
  // notify dependency watchers that an attribute changed
  dependent_sendEvent(CfgItem::depend_getClassGuid(), Event_ATTRIBUTE_CHANGED, 0, (void*)attr->getAttributeName());

  for (int i = 0; ; i++) {
    AttrCallback *acb;
    if (!callbacks.multiGetItem(attr, i, &acb)) break;
    acb->onValueChange(attr);
  }
  
  return 0;
}

void CfgItemI::cfgitem_setGUID(GUID guid) {
  myguid = guid;
}

int CfgItemI::setName(const char *name) {
  Named::setName(name);
  // notify dependency watchers that name changed?
  dependent_sendEvent(CfgItem::depend_getClassGuid(), Event_NAMECHANGE);
  return 1;
}

int CfgItemI::cfgitem_registerAttribute(Attribute *attr, AttrCallback *acb) {
  if (attributes.haveItem(attr)) return 0;
  int ret = attributes.addItem(attr) != NULL;
  if (!ret) return ret;

  attr->setCfgItem(this);

  // set optional callback
  if (acb != NULL) {
    cfgitem_addCallback(attr, acb);
  }

  // notify dependency watchers of new attribute
  dependent_sendEvent(CfgItem::depend_getClassGuid(), Event_ATTRIBUTE_ADDED, 0, (void*)attr->getAttributeName());

  return ret;
}

int CfgItemI::cfgitem_deregisterAttribute(Attribute *attr) {
  if (!attributes.haveItem(attr)) return 0;
  int ret = attributes.removeItem(attr);
  // notify dependency watchers of attribute going away
  dependent_sendEvent(CfgItem::depend_getClassGuid(), Event_ATTRIBUTE_REMOVED, 0, (void*)attr->getAttributeName());

  // remove callbacks
  callbacks.multiDelAllForItem(attr, TRUE);
  attr->disconnect();

  return ret;
}

void CfgItemI::cfgitem_addCallback(Attribute *attr, AttrCallback *acb) {
  ASSERT(attr != NULL);
  ASSERT(acb != NULL);
  callbacks.multiAddItem(attr, acb);
}

void CfgItemI::cfgitem_deregisterAll() {
  foreach(children)
    children.getfor()->cfgitem_deregisterAll();
  endfor
  while (attributes.getNumItems()) cfgitem_deregisterAttribute(attributes[0]);
}

void CfgItemI::addChildItem(CfgItemI *child) {
  ASSERT(child != NULL);
  if (!children.haveItem(child)) {
    children.addItem(child);
    child->setParentGuid(myguid);
  }
}

void CfgItemI::setCfgXml(const char *groupname) {
  cfgxml = groupname;
}

void CfgItemI::setParentGuid(GUID guid) {
  parent_guid = guid;
}

void *CfgItemI::dependent_getInterface(const GUID *classguid) {
  HANDLEGETINTERFACE(CfgItem);
  return NULL;
}

int CfgItemI::cfgitem_addAttribute(const char *name, const char *defval) {
  if (getAttributeByName(name)) return 0;
  cfgitem_registerAttribute(newattribs.addItem(new _string(name, defval)));
  return 1;
}

int CfgItemI::cfgitem_delAttribute(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (!newattribs.haveItem(attr)) return 0;
  cfgitem_deregisterAttribute(attr);
  delete attr;
  newattribs.removeItem(attr);
  return 1;
}

#define CBCLASS CfgItemI
START_DISPATCH
  CB(CFGITEM_GETNAME, cfgitem_getName);
  CB(CFGITEM_GETGUID, cfgitem_getGuid);
  CB(CFGITEM_GETNUMATTRIBUTES, cfgitem_getNumAttributes);
  CB(CFGITEM_ENUMATTRIBUTE, cfgitem_enumAttribute);
  CB(CFGITEM_GETCONFIGXML, cfgitem_getConfigXML);
  VCB(CFGITEM_ONCFGGROUPCREATE, cfgitem_onCfgGroupCreate);
  VCB(CFGITEM_ONCFGGROUPDELETE, cfgitem_onCfgGroupDelete);
  CB(CFGITEM_GETNUMCHILDREN, cfgitem_getNumChildren);
  CB(CFGITEM_ENUMCHILD, cfgitem_enumChild);
  CB(CFGITEM_GETPARENTGUID, cfgitem_getParentGuid);
  VCB(CFGITEM_ONREGISTER, cfgitem_onRegister);
  VCB(CFGITEM_ONDEREGISTER, cfgitem_onDeregister);
  CB(CFGITEM_GETATTRIBUTETYPE, cfgitem_getAttributeType);
  CB(CFGITEM_GETATTRIBUTECONFIGGROUP, cfgitem_getAttributeConfigGroup);
  CB(CFGITEM_GETDATALEN, cfgitem_getDataLen);
  CB(CFGITEM_GETDATA, cfgitem_getData);
  CB(CFGITEM_SETDATA, cfgitem_setData);
  CB(CFGITEM_GETDEPENDENCYPTR, cfgitem_getDependencyPtr);
  CB(CFGITEM_ADDATTRIB, cfgitem_addAttribute);
  CB(CFGITEM_DELATTRIB, cfgitem_delAttribute);
END_DISPATCH
#undef CBCLASS
