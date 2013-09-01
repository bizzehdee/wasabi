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

#ifndef __C_SCRIPTOBJ_H
#define __C_SCRIPTOBJ_H

#include "../scriptobj.h"

class COMEXP C_RootObject{

  public:

    C_RootObject(ScriptObject *o);
    C_RootObject();
    virtual ~C_RootObject();

    virtual void C_hook(ScriptObject *o);

    virtual const char *getClassName();
    virtual void notify(const char *a, const char *b, int c, int d);
    virtual ScriptObject *getScriptObject();

    operator ScriptObject *() { return getScriptObject(); }

  private:

    ScriptObject *object;  
    static int getclassname_id;
    static int notify_id;
    static int inited;
    static int count;
};



#endif
