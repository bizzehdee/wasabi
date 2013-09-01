#ifndef _WASABI_ATTRBOOL_H
#define _WASABI_ATTRBOOL_H

#include "attribute.h"

// inherit from this one, or just use it

/**
  Boolean configuration attributes have two values, true or false.
  They can be used like any other config item.
  
  @short Boolean configuration attribute.
  @ver 1.0
  @author Nullsoft
  @see _int
  @see _string
  @see _float
*/
class _bool : public Attribute {
public:
  /**
    Optionally set the name and default value of 
    your configuration attribute during construction.
    
    @param name Name of the configuration attribute.
    @param default_val Default value.
  */
  _bool(const char *name=NULL, int default_val=0) : Attribute(name) {
    setValueAsInt(!!default_val, true);
  }

  // convenience operators
  /**
    Get the value of the attribute.
  */
  operator bool() { return !!getValueAsInt(); }
  
  /**
    Set the value of the attribute.
  */
  bool operator =(int newval) { setValueAsInt(!!newval); return *this; }

  bool operator =(const char *newval) { setValueAsInt(!!ATOI(newval)); return *this; }

  void toggle() { setValueAsInt(!getValueAsInt()); }

  // from Attribute

  /**
    Get the attribute type. This will return 
    a constant representing the attribute type.
    
    These constants can be: BOOL, FLOAT, STRING and INT.
    
    @see AttributeType
    @ret The attribute type.
  */
  virtual int getAttributeType() { return AttributeType::BOOL; }
  
  /**
    Get the configuration group to be used to represent
    this attribute in the registry.
    
    @ret Config group to be used.
  */
  virtual const char *getConfigGroup() { return "studio.configgroup.bool"; }
};

#endif
