#ifndef _SVC_ENUM_H
#define _SVC_ENUM_H

#include "waservicefactory.h"

// try to use this one, it does a clientLock for you
template <class T>
class castService {
public:
  castService(waServiceFactory *w, int global_lock=TRUE) : was(w), gl(global_lock) { }
  operator T *() {
    if (was == NULL) return NULL;
    T *ret = static_cast<T *>(was->getInterface(gl));
    return ret;
  }
private:
  waServiceFactory *was;
  int gl;
};

#include "svcenumbyguid.h"
#include "svcenum.h"
#include "svcenumt.h"

#endif
