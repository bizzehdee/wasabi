/*

  Nullsoft WASABI Source File License

  Copyright 1999-2001 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


  Brennan Underwood
  brennan@nullsoft.com

*/

#ifndef __SCRIPTHOOK_H
#define __SCRIPTHOOK_H

#include "api/script/vcputypes.h"
#include "bfc/dispatch.h"
#include "bfc/ptrlist.h"


class ScriptObject;
class ScriptObjectController;

// ----------------------------------------------------------------------------------------------------------

class COMEXP ScriptHook : public Dispatchable {
  protected:
    ScriptHook() {};

  public:
    int eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams);

  enum {
    EVENTCALLBACK            = 100,
  };

};

inline int ScriptHook::eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams) {
  return _call(EVENTCALLBACK, 0, object, dlfid, params, nparams);
}

class COMEXP ScriptHookI : public ScriptHook {

public:

  ScriptHookI();
  virtual ~ScriptHookI();

  virtual int eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams)=0;

  void addMonitorObject(ScriptObject *o, const GUID *hookedclass=NULL); // NULL = all classes of object o
  void addMonitorClass(ScriptObject *o);

protected:
  RECVS_DISPATCH;

  PtrList<ScriptObjectController> controllers;
};

#endif