#ifndef _WASABI_CFGITEMI_H
#define _WASABI_CFGITEMI_H

#include "cfgitem.h"

#include <bfc/named.h>
#include <bfc/ptrlist.h>
#include <bfc/depend.h>
#include <bfc/multimap.h>

class AttrCallback;
class Attribute;

// this is the one you inherit from/use
/**
  
  @short Configuration Item
  @ver 1.0
  @author Nullsoft
  @see Attribute
  @see _bool
  @see _int
  @see _float
  @see _string
*/
class CfgItemI : public CfgItem, public DependentI, private Named {
public:
  /**
    Optionally sets the name and the GUID of the
    configuration item if they are specified
    upon creation of the object.
    
    @param name Name of the configuration item.
    @param guid GUID of the configuration item.
  */
  CfgItemI(const char *name=NULL, GUID guid=INVALID_GUID);
  
  /**
    Does nothing.
  */
  virtual ~CfgItemI();

  /**
    Get the name of the configuration item.
    
    @ret Name of the configuration item.
  */
  const char *cfgitem_getName();
  
  /**
    Get the GUID of the configuration item.
    
    @ret GUID of the configuration item.
  */
  GUID cfgitem_getGuid();

  /**
    Sets the prefix to be prepended in the config file for all attributes
    of this item.

    @see cfgitem_getPrefix
    @param prefix The prefix.
  */
  void cfgitem_setPrefix(const char *prefix);
/**
  Gets the config prefix, if any was set.

  @see cfgitem_setPrefix
  @ret Pointer to the config prefix.
*/
  const char *cfgitem_getPrefix();
  
  /**
    Get the number of attributes registered
    to this configuration item.
    
    @ret Number of attributes.
  */
  int cfgitem_getNumAttributes();
  
  /**
    Enumerate the attributes registered
    with this configuration item.
    
    @ret 
  */
  const char *cfgitem_enumAttribute(int n);

  const char *cfgitem_getConfigXML();
  virtual void cfgitem_onCfgGroupCreate(RootWnd *cfggroup, const char *attrname) {}
  virtual void cfgitem_onCfgGroupDelete(RootWnd *cfggroup) {}

  virtual int cfgitem_getNumChildren();
  virtual CfgItem *cfgitem_enumChild(int n);
  virtual GUID cfgitem_getParentGuid();

  virtual void cfgitem_onRegister();
  virtual void cfgitem_onDeregister();

  int cfgitem_getAttributeType(const char *name);
  const char *cfgitem_getAttributeConfigGroup(const char *name);
  int cfgitem_getDataLen(const char *name);
  int cfgitem_getData(const char *name, char *data, int data_len);
  int cfgitem_setData(const char *name, const char *data);

  // override these to catch notifications from attribs, call down
  virtual int cfgitem_onAttribSetValue(Attribute *attr);

  virtual int cfgitem_usePrivateStorage() { return 0; } //override and return 1 to keep stuff out of system settings

protected:
  void cfgitem_setGUID(GUID guid);

public:
  int setName(const char *name);
  int cfgitem_registerAttribute(Attribute *attr, AttrCallback *acb=NULL);
  // does not call delete on the attribute
  int cfgitem_deregisterAttribute(Attribute *attr);
  void cfgitem_deregisterAll();

  void cfgitem_addCallback(Attribute *attr, AttrCallback *acb);

  int cfgitem_addAttribute(const char *name, const char *defval);
  int cfgitem_delAttribute(const char *name);

protected:
  RECVS_DISPATCH;

  // derived classes can override this to catch name changes
  virtual void cfgitem_onSetName() { }

  Attribute *getAttributeByName(const char *name);
 
  void addChildItem(CfgItemI *child);

  void setCfgXml(const char *groupname);

  void setParentGuid(GUID guid);

private:
  Dependent *cfgitem_getDependencyPtr() { return this; };
  virtual void *dependent_getInterface(const GUID *classguid);

  // from Named
  virtual void onSetName() { cfgitem_onSetName(); }

  String prefix;
  PtrList<Attribute> attributes;
  MultiMap<Attribute*, AttrCallback> callbacks;	//CUT
  PtrList<CfgItemI> children;
  String cfgxml;
  GUID myguid, parent_guid;
  PtrList<Attribute> newattribs;
};

#endif
