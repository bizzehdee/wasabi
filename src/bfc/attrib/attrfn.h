#ifndef _WASABI_ATTRFN_H
#define _WASABI_ATTRFN_H

#include "attrstr.h"

class _filename : public _string {
public:
  _filename(const char *name, const char *default_val="")
    : _string(name, default_val) { }

  virtual int getAttributeType() { return AttributeType::FILENAME; }
  virtual const char *getConfigGroup() { return "studio.configgroup.filename"; }
};

#endif
