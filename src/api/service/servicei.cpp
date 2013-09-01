
#include <precomp.h>
#include "servicei.h"


void MODULE_SVCMGR_ONINIT() {
  AutoServiceList modules(staticServiceMgr->__m_modules);
  int count = 1;
  while(modules.notempty() && count > 0) {
    count = 0;
    foreach(modules)
      if(modules.getfor()->prereqsMet()) {
        modules.getfor()->registerServices();
        modules.removeItem(modules.getfor());
        count++;
      }
    endfor;
  }
  if(count == 0) {
    DebugString("Warning: %d service modules not started due to missing prerequisites", modules.n());
  }
}

void MODULE_SVCMGR_SYSTEM_ONINIT() {
  AutoServiceList modules(staticServiceMgr->__m_modules2);
  int count = 1;
  while(modules.notempty() && count > 0) {
    count = 0;
    foreach(modules)
      if(modules.getfor()->prereqsMet()) {
        ASSERT(modules.getfor() != NULL);
        modules.getfor()->registerServices();
        modules.removeItem(modules.getfor());
        count++;
      }
    endfor
  }
  if(count == 0) {
    DebugString("Warning: %d system service modules not started due to missing prerequisites", modules.n());
  }
}
