#include "rootobjcb.h"

#define CBCLASS RootObjectCallbackI
START_DISPATCH;
  VCB(ROOT_ONNOTIFY,             rootobjectcb_onNotify);
END_DISPATCH;
