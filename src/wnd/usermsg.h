#ifndef _USERMSG_H
#define _USERMSG_H

// WM_USER+ messages for common classes
// DO NOT PUT ANY MORE FUCKING NON WNDPROC SHIT IN HERE
// childNotify stuff goes into notifmsg.h

enum {
  UMSG_NOP = WM_USER,

  UMSG_DEFERRED_CALLBACK,

//CUT  UMSG_TREEVIEW_EDITLABELDEFERRED,
//CUT  UMSG_TREEVIEW_SELECTITEM,

  UMSG_COMPON_POSTMESSAGE,

  LAST_COMMON_UMSG
};

#endif
