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

#ifndef __HOOK_SCRIPTOBJECT_H
#define __HOOK_SCRIPTOBJECT_H

#include "scripthook.h"

class COMEXP H_RootObject : public ScriptHookI {

  public:
    
    H_RootObject(ScriptObject *o);
    H_RootObject();
    virtual ~H_RootObject();

    virtual void H_hook(ScriptObject *o);

    virtual int eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams);

    virtual void hook_onNotify(const char *s, const char *t, int u, int v) {}

    virtual ScriptObject *getHookedObject() { return me; }

  private:

    ScriptObject *me;
    static int onnotify_id;
    static int inited;
    static int count;
};

#endif