#ifndef _JPGSAVE_H
#define _JPGSAVE_H

#ifdef _WASABIRUNTIME

#include <studio/wac.h>

#define WACNAME WACjpgsave

class WACNAME : public WAComponentClient {
public:
  WACNAME();
  virtual ~WACNAME();

  virtual const char *getName() { return "JPEG saver"; };//EDITME
  virtual GUID getGUID();
};

#endif

#endif
