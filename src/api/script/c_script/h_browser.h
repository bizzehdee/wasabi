#ifndef __HOOK_BROWSER_H
#define __HOOK_BROWSER_H

#include "h_guiobject.h"

#define H_BROWSER_PARENT H_GuiObject

class COMEXP H_Browser : public H_BROWSER_PARENT {

public:

  H_Browser(ScriptObject *o);
  H_Browser();
  virtual ~H_Browser();
  virtual void H_hook(ScriptObject *o);
  ScriptObject *getHookedObject();

  virtual int eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams);
  virtual void hook_onBeforeNavigate(const char *url, int flags, const char *targetframename) {  }
  virtual void hook_onDocumentComplete(const char *url) {  }

  private:

  ScriptObject *obj;
  int inited;
  static int loaded;
  static int onbeforenavigate_id;
  static int ondocumentcomplete_id;
};

#endif
