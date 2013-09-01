#ifndef _WASABI_PUMPHANDLER_H
#define _WASABI_PUMPHANDLER_H
// ===============================================================================================================
// Pump Handlers:
// ===============================================================================================================

#include <bfc/guid.h>
#include <bfc/map.h>
#include <bfc/thread.h>

typedef GUID PumpClass;
class DataSource;
class DataSink;

const PumpClass PUMP_NOHANDLER = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
class PumpHandler {
public:
  PumpHandler() {}
  virtual ~PumpHandler() {}
  virtual PumpClass getPumpClass() { return PUMP_NOHANDLER; }
  virtual int pushData(GUID sink, GUID pusher, void *data, unsigned long sz)=0;
  virtual int pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz)=0;
  
  virtual int registerSink(DataSink *sink)=0;
  virtual int registerSource(DataSource *source)=0;
  
  virtual int deregisterSink(DataSink *sink)=0;
  virtual int deregisterSource(DataSource *source)=0;
};

const PumpClass PUMP_LOCAL = { 0x7c1192c9, 0x5c5a, 0x9344, { 0xb0, 0xb3, 0x83, 0x71, 0x91, 0x59, 0x18, 0xb4 } };
class LocalPumpHandler : public PumpHandler {
public:
  LocalPumpHandler() {}
  virtual ~LocalPumpHandler() {}
  virtual PumpClass getPumpClass() { return PUMP_LOCAL; }
  virtual int pushData(GUID sink, GUID pusher, void *data, unsigned long sz);
  virtual int pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz);
  
  virtual int registerSink(DataSink *sink);
  virtual int registerSource(DataSource *source);
  
  virtual int deregisterSink(DataSink *sink);
  virtual int deregisterSource(DataSource *source);

  private:
  Map<GUID, DataSink *> sinks;
  Map<GUID, DataSource *> sources;
};

// I have no idea how this class would actually work.. but here's a stub.
const PumpClass PUMP_TCP = { 0x2452b77e, 0x1d77, 0x0942, { 0x95, 0xff, 0x80, 0xd1, 0x7d, 0x83, 0x5d, 0x4f } };
class TCPPumpHandler : public PumpHandler, public Thread {
public:
  TCPPumpHandler() {}
  virtual ~TCPPumpHandler() {}
  virtual PumpClass getPumpClass() { return PUMP_TCP; }
  virtual int pushData(GUID sink, GUID pusher, void *data, unsigned long sz);
  virtual int pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz);
  
  // TCP/IP listening proc ?
  virtual int threadProc();

};

#endif // _WASABI_PUMPHANDLER_H
