#ifndef _JPGLOAD_H
#define _JPGLOAD_H

#ifdef _WASABIRUNTIME

#include <api/wac/wac.h>

#define WACNAME WACjpgload

class WACNAME : public WAComponentClient {
public:
  WACNAME();
  virtual ~WACNAME();

  virtual const char *getName() { return "JPEG loader"; };//EDITME
  virtual GUID getGUID();

  virtual int getDisplayComponent() { return FALSE; };
};

#endif

#endif
