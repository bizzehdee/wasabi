#ifndef _NAMED_H
#define _NAMED_H

#include <bfc/string/string.h>

class NOVTABLE Named {
public:
  Named(const char *initial_name=NULL) : name(initial_name) {}
  virtual ~Named() {}	// placeholder to ensure name is destructed properly
  const char *getName() const { return name; }
  const char *getNameSafe(const char *defval=NULL) const {
    const char *str = name;
    return str ? str : (defval ? defval : "(null)");
  }
  void setName(const char *newname, int nocb=FALSE) { name = newname; if (!nocb) onSetName(); }
  // override this to catch name settings
  virtual void onSetName() {}

private:
  String name;
};

#endif
