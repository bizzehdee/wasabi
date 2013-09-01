#include <precomp.h>
#include "cmds.h"
#include "mdiframe.h"

int Commands::onCommand(int cmd) { 
  switch (cmd) {
    case MDITEST_FILE_CLOSE: {  
      if (g_frame) {
        OSMDIChildWnd *child = g_frame->getCurMDIChild();
        if (child) {
          child->onUserClose(); 
        }
      }
      break;
    }
    case MDITEST_FILE_OPEN: {
      if (g_frame) g_frame->onOpen(); 
      break;
    }
    case MDITEST_FILE_EXIT: {
      WASABI_API_APP->main_shutdown();
      break;
    }
    default: return 0;
  }
  return 1;
}

int Commands::onGetEnabled(int cmd, int defval) {
  switch (cmd) {
    case MDITEST_FILE: return 1;
    case MDITEST_FILE_OPEN: return 1;
    case MDITEST_FILE_CLOSE: return g_frame && g_frame->getCurMDIChild();
    case MDITEST_FILE_EXIT: return 1;
    case MENU_COMMAND_ARRANGE: 
    case MENU_COMMAND_CASCADE: 
    case MENU_COMMAND_VTILE: 
    case MENU_COMMAND_HTILE: 
    case MDITEST_WINDOW: 
      return g_frame->getNumMDIChildren() > 0;
  }
  return defval; 
} 

int Commands::onGetChecked(int cmd, int defval) { 
  return defval; 
} 
