#ifndef COMMANDCALLBACK_H
#define COMMANDCALLBACK_H

class CommandCallback {
public:
  virtual int onCommand(int cmd) { return 0; }                     // for toolbar buttons and menu items
  virtual int onGetEnabled(int cmd, int defval) { return defval; } // for toolbar buttons and menu items
  virtual int onGetChecked(int cmd, int defval) { return defval; } // for menu items
};

#endif
