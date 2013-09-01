#ifndef CMDS_H
#define CMDS_H

#include <wnd/wnds/os/commandcb.h>

#define MDITEST_FILE                0
#define MDITEST_FILE_OPEN          10
#define MDITEST_FILE_CLOSE         20
#define MDITEST_FILE_EXIT          30
#define MDITEST_WINDOW            100

class Commands : public CommandCallback {
public:  
  virtual int onCommand(int cmd);
  virtual int onGetEnabled(int cmd, int defval);
  virtual int onGetChecked(int cmd, int defval);
};

#endif
