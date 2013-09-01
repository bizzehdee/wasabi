#include "precomp.h"
#include "api_timer.h"

#ifdef CBCLASS
#undef CBCLASS
#endif
#define CBCLASS timer_apiI
START_DISPATCH;
  VCB(TIMER_API_ADD, timer_add);
  VCB(TIMER_API_REMOVE, timer_remove);
END_DISPATCH;
