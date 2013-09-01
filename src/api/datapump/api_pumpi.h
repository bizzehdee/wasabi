#ifndef __API_DATAPUMP_IMPL_H
#define __API_DATAPUMP_IMPL_H

/*<?<autoheader/>*/
#include "api_pump.h"
#include "api_pumpx.h"
/*?>*/
/*[interface.header.h]
class PumpHandler;
class DataSink;
class DataSource;
typedef GUID PumpClass;
*/

#include <api/service/services.h>
class api_pumpI : public api_pumpX {
public:
  NODISPATCH api_pumpI();
  NODISPATCH virtual ~api_pumpI();
  
  static const char *getServiceName() { return "Data Pump API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  DISPATCH(100) int addPumpHandler(PumpHandler *handler);
  DISPATCH(110) PumpHandler *getPumpHandler(PumpClass eclass);

  DISPATCH(200) int registerSink(PumpClass eclass, DataSink *sink);
  DISPATCH(210) int registerSource(PumpClass eclass, DataSource *source);
  DISPATCH(220) int deregisterSink(DataSink *sink);
  DISPATCH(230) int deregisterSource(DataSource *source);
  
  DISPATCH(300) int pushData(GUID sink, GUID pusher, void *data, unsigned long sz);
  DISPATCH(350) int pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz);

};

/*[interface.footer.h]
// {0E572925-FAA0-F849-99B4-DB6A010600B9}
static const GUID pumpApiServiceGuid = 
{ 0x0e572925, 0xfaa0, 0xf849, { 0x99, 0xb4, 0xdb, 0x6a, 0x01, 0x06, 0x00, 0xb9 } };

extern api_pump *pumpApi;
*/

#endif // __API_DATAPUMP_IMPL_H
