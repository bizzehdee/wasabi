#ifndef _WASABI_PUMPAPI_H
#define _WASABI_PUMPAPI_H
// ===============================================================================================================
// Pump API:
// ===============================================================================================================

#include <bfc/guid.h>

typedef GUID PumpClass;
class PumpHandler;
class DataSink;
class DataSource;

class PumpApi {
public:
  static void onShutdown();
  static int addPumpHandler(PumpHandler *handler);
  static PumpHandler *getPumpHandler(PumpClass eclass);

  static int registerSink(PumpClass eclass, DataSink *sink);
  static int registerSource(PumpClass eclass, DataSource *source);
  static int deregisterSink(DataSink *sink);
  static int deregisterSource(DataSource *source);
  
  static int pushData(GUID sink, GUID pusher, void *data, unsigned long sz);
  static int pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz);
  
private:
  static PumpHandler *handlerForSink(GUID sink);
  static PumpHandler *handlerForSource(GUID source);
};

#endif // _WASABI_PUMPAPI_H
