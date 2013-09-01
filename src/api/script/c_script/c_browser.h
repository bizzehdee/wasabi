#ifndef __C_BROWSER_H
#define __C_BROWSER_H

#include "c_guiobject.h"

#define C_BROWSER_PARENT C_GuiObject

class COMEXP C_Browser : public C_BROWSER_PARENT {
  public:

  C_Browser(ScriptObject *object);
  C_Browser();
  virtual ~C_Browser();

  virtual void C_hook(ScriptObject *o);

  ScriptObject *getScriptObject();

  virtual void navigateUrl(const char *url);
  virtual void back();
  virtual void forward();
  virtual void stop();
  virtual void refresh();
  virtual void home();
  virtual void setTargetName(const char *targetname);
  virtual int onBeforeNavigate(const char *url, int flags, const char *targetframename);
  virtual void onDocumentComplete(const char *url);

  private:

  ScriptObject *obj;
  int inited;
  static int loaded;
  static int navigateurl_id;
  static int back_id;
  static int forward_id;
  static int stop_id;
  static int refresh_id;
  static int home_id;
  static int settargetname_id;
  static int onbeforenavigate_id;
  static int ondocumentcomplete_id;
};

#endif
