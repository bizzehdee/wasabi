#ifndef _PNGLOAD_H
#define _PNGLOAD_H

#ifdef _WASABIRUNTIME

#include "../studio/wac.h"

class WACPngLoad : public WAComponentClient {
public:
  WACPngLoad();

  virtual const char *getName() { return "PNG loader"; }
  virtual GUID getGUID();
};

#endif

#endif
