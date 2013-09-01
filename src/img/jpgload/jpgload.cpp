#include <precomp.h>

#include "jpgload.h"
#include "loader_jpg.h"
#include <api/service/servicei.h>

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(JpgLoader_Svc)
DECLARE_SERVICETSINGLE(svc_imageLoader, JpgLoad);
END_SERVICES(JpgLoader_Svc, _JpgLoader_Svc);

#ifdef _X86_
extern "C" { int _link_JpgLoadSvc; }
#else
extern "C" { int __link_JpgLoadSvc; }
#endif

#else

static WACNAME wac;
WAComponentClient *the = &wac;

#include <api/service/servicei.h>

// {AE04FB30-53F5-4032-BD29-032B87EC3404}
static const GUID guid = 
{ 0xae04fb30, 0x53f5, 0x4032, { 0xbd, 0x29, 0x3, 0x2b, 0x87, 0xec, 0x34, 0x04 } };


WACNAME::WACNAME() {
  registerService(new waServiceTSingle<svc_imageLoader, JpgLoad>);
}

WACNAME::~WACNAME() {
}

GUID WACNAME::getGUID() {
  return guid;
}

#endif