#include "precomp.h"

#include "ppmload.h"
#include "loader_ppm.h"

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(PpmLoader_Svc)
DECLARE_SERVICETSINGLE(svc_imageLoader, PpmLoad);
END_SERVICES(PpmLoader_Svc, _PpmLoader_Svc);

#ifdef _X86_
extern "C" { int _link_PpmLoadSvc; }
#else
extern "C" { int __link_PpmLoadSvc; }
#endif

#else

static WACPpmLoad wac;
WAComponentClient *the = &wac;

// {70F911B2-FC5A-40c6-80C1-B082A9F18F80}
static const GUID guid = 
{ 0x70f911b2, 0xfc5a, 0x40c6, { 0x80, 0xc1, 0xb0, 0x82, 0xa9, 0xf1, 0x8f, 0x80 } };

WACPpmLoad::WACPpmLoad() {
  registerService(new waServiceTSingle<svc_imageLoader, PpmLoad>);
}

GUID WACPpmLoad::getGUID() {
  return guid;
}

#endif
