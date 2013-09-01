#include "precomp.h"

#include "jpgsave.h"
#include <studio/services/servicei.h>
#include "saver.h"

#ifndef _WASABIRUNTIME

BEGIN_SERVICES(JpgSaver_Svc)
DECLARE_SERVICETSINGLE(svc_imageWriter, JpgSave);
END_SERVICES(JpgSaver_Svc, _JpgSaver_Svc);

#ifdef _X86_
extern "C" { int _link_JpgSaveSvc; }
#else
extern "C" { int __link_JpgSaveSvc; }
#endif

#else

static WACNAME wac;
WAComponentClient *the = &wac;

// {243389F2-357B-49aa-A6C8-EA5AC74EB4DB}
static const GUID guid = 
{ 0x243389f2, 0x357b, 0x49aa, { 0xa6, 0xc8, 0xea, 0x5a, 0xc7, 0x4e, 0xb4, 0xdb } };

WACNAME::WACNAME() {
  registerService(new waServiceTSingle<svc_imageWriter, JpgSave>);
}

WACNAME::~WACNAME() {
}

GUID WACNAME::getGUID() {
  return guid;
}

#endif
