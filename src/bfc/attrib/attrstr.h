#ifndef _WASABI_ATTRSTR_H
#define _WASABI_ATTRSTR_H

#include "attribute.h"

#include <bfc/string/string.h>

/**
  String configuration attributes, can have any string value 
  of any length. They can be used like any other config item.
  
  @short String configuration attribute.
  @ver 1.0
  @author Nullsoft
  @see _int
  @see _bool
  @see _float
*/
class _string : public Attribute {
public:
  /**
    Optionally set the name and default value of 
    your configuration attribute during construction.
      
    @param name
    @param default_val
  */
  _string(const char *name=NULL, const char *default_val=NULL)
    : Attribute(name) {
    setData(default_val, true);
  }

  /**
    Get the attribute type. This will return 
    a constant representing the attribute type.
    
    These constants can be: BOOL, FLOAT, STRING and INT.
    
    @see AttributeType
    @ret The attribute type.
  */
  virtual int getAttributeType() { return AttributeType::STRING; }
  
  /**
    Get the configuration group to be used to represent
    this attribute in the registry.
    
    @ret Config group to be used.
  */
  virtual const char *getConfigGroup() { return "studio.configgroup.string"; }

//CUT  virtual int getPermissions() { return ATTR_PERM_ALL; }

  /**
    Get the value of the attribute.
    
    @ret The value of the attribute
  */
  const char *getValue();
  inline const char *v() { return getValue(); }

  char *getNonConstValue() { return const_cast<char *>(getValue()); }
  inline char *ncv() { return getNonConstValue(); }

  int isempty() {
    const char *v = getValue();
    return (v == NULL || *v == '\0');
  }
  int notempty() { return !isempty(); }
  
  /**
    Set the value of the attribute.
    
    @param val The value you want to set.
    @ret 1, success; 0, failure;
  */
  int setValue(const char *val) { return setData(val); }

  // convenience operators
  /**
    Get the value of the attribute.
  */
  operator const char *() { return getValue(); }

  /**
    Set the value of the attribute.
  */
  const char *operator =(const char *newval) { return setValue(newval) ? newval : getValue(); }

private:
  String returnval;
};

#endif
