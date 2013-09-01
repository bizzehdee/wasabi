#ifndef _WASABI_PUMPOBJ_H
#define _WASABI_PUMPOBJ_H

#include "pumphandler.h"
#include "pumpapi.h"
#include <bfc/guid.h>

// ===============================================================================================================
// Basic Pump objects:
// ===============================================================================================================

// Active data pusher
class DataPusher : public NamedGuid {
public:
  DataPusher(GUID sink_, GUID me, const char *name = "(unnamed DataPusher)")
      : NamedGuid(me, name), sink(sink_) { }
  virtual ~DataPusher() { }
  
  int pushData(void *data, unsigned long sz) {
    return WASABI_API_DATAPUMP->pushData(sink,getGuid(),data,sz);
  }

private:
  GUID sink;
};

// Passive data sink
class DataSink : public NamedGuid {
public:
  DataSink(GUID me, const char *name = "(unnamed DataSink)", PumpClass pclass = PUMP_LOCAL)
      : NamedGuid(me, name)
  { 
    WASABI_API_DATAPUMP->registerSink(pclass,this);
  }
  virtual ~DataSink() { WASABI_API_DATAPUMP->deregisterSink(this); }
  
  virtual int onData(GUID from, void *data, unsigned long sz) { return 0; }
};

// Passive data source
class DataSource : public NamedGuid {
public:
  DataSource(GUID me, const char *name = "(unnamed DataSource)", PumpClass pclass = PUMP_LOCAL)
      : NamedGuid(me, name)
  {
    WASABI_API_DATAPUMP->registerSource(pclass,this);
  }
  virtual ~DataSource() { WASABI_API_DATAPUMP->deregisterSource(this); }
  
  virtual int getData(GUID request_from, void *data, unsigned long sz, unsigned long *actual_sz) {
    // always be sure to cover this behavior:
    if (sz == 0 && actual_sz == NULL) return 0;
    // check request_from if necessary.
    // if sz == 0, return as much data as you like, and return actual_sz
    // otherwise, put up to sz bytes at data, return actual bytes to actual_sz.
    // return 0 on denied or error, return 1 otherwise.
    return 0;
  }

};

// Active data puller
class DataPuller : public NamedGuid {
public:
  DataPuller(GUID src, GUID me, const char *name = "(unnamed DataPuller)" )
      : NamedGuid(me, name), source(src) { }
  virtual ~DataPuller() { }
  
  int pullData(void *data, unsigned long sz, unsigned long *actual_sz) {
    return WASABI_API_DATAPUMP->pullData(source,getGuid(),data,sz,actual_sz);
  }

private:
  GUID source;
};

// ===============================================================================================================
// More Pump objects: (perhaps more useful)
// ===============================================================================================================

#endif // _WASABI_PUMPOBJ_H
