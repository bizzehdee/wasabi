#include <precomp.h>

#include "pngload.h"
#include "loader.h"
#include <api/service/servicei.h>

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(PngLoader_Svc)
DECLARE_SERVICETSINGLE(svc_imageLoader, PngLoad);
END_SERVICES(PngLoader_Svc, _PngLoader_Svc);

#ifdef _X86_
extern "C" { int _link_PngLoadSvc; }
#else
extern "C" { int __link_PngLoadSvc; }
#endif

#else

static WACPngLoad wac;
WAComponentClient *the = &wac;

// {5E04FB28-53F5-4032-BD29-032B87EC3725}
static const GUID guid = 
{ 0x5e04fb28, 0x53f5, 0x4032, { 0xbd, 0x29, 0x3, 0x2b, 0x87, 0xec, 0x37, 0x25 } };


WACPngLoad::WACPngLoad() {
  registerService(new waServiceTSingle<svc_imageLoader, PngLoad>);
}

GUID WACPngLoad::getGUID() {
  return guid;
}

#endif
