#ifndef _PPMLOAD_H
#define _PPMLOAD_H

#ifdef _WASABIRUNTIME

#include "../studio/wac.h"

class WACPpmLoad : public WAComponentClient {
public:
  WACPpmLoad();

  virtual const char *getName() { return "PPM loader"; }
  virtual GUID getGUID();
};

#endif

#endif
