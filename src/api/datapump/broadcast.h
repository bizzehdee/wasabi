#ifndef _WASABI_PUMP_BCAST_H
#define _WASABI_PUMP_BCAST_H

#include "pumpobj.h"
#include <bfc/tlist.h>

using namespace wasabi;

// Pump broadcast helpers. Use these to broadcast data to multiple sinks.

// A little trick for Listeners to connect/disconnect themselves.
//{32030EF1-9C9A-4df5-AFC8-86B5293942BE}
static const GUID RegisterListenerGUID =
{ 0x32030ef1, 0x9c9a, 0x4df5, { 0xaf, 0xc8, 0x86, 0xb5, 0x29, 0x39, 0x42, 0xbe } };
//{8a2fe374-64cd-4821-92e6-b383e2271073}
static const GUID DeregisterListenerGUID =
{ 0x8a2fe374, 0x64cd, 0x4821, { 0x92, 0xe6, 0xb3, 0x83, 0xe2, 0x27, 0x10, 0x73 } };

class DataBroadcast : public DataSink {
public:
  DataBroadcast(GUID me, const char *name="(unnamed DataBroadcast)", PumpClass pclass = PUMP_LOCAL)
      : DataSink(me, name, pclass) {};
  virtual ~DataBroadcast() {};

  virtual int onData(GUID from, void *data, unsigned long sz) { 
    if(from == RegisterListenerGUID) {
      ASSERT(sz == sizeof(GUID));
      GUID sink = *static_cast<GUID *>(data);
      addSink(sink);
      return 1;
    }
    if(from == DeregisterListenerGUID) {
      ASSERT(sz == sizeof(GUID));
      GUID sink = *static_cast<GUID *>(data);
      removeSink(sink);
      return 1;
    }
    
    for(int i = 0; i < sinks.getNumItems(); i++) { 
      WASABI_API_DATAPUMP->pushData( sinks[i], from, data, sz);
    }
    return 1;
  }
  
  int addSink(GUID sink) {
    if(sinks.haveItem(sink)) return 0;
    sinks.addItem(sink);
    return 1;
  }
  int removeSink(GUID sink) {
    return sinks.delItem(sink);
  }
private:
  TList<GUID> sinks;
};


class DataBroadcastListener : public DataSink {
public:
  DataBroadcastListener(GUID _listento, GUID me, const char *name = "(unnamed DataBroadcastListener)", PumpClass pclass = PUMP_LOCAL)
      : DataSink(me, name, pclass), listento(_listento)
  {
    WASABI_API_DATAPUMP->pushData(listento, RegisterListenerGUID, &me, sizeof(GUID));
  }
  virtual ~DataBroadcastListener() {
    disconnect();
  }
  
  virtual int onData(GUID from, void *data, unsigned long sz)=0;
  
  int disconnect() {
    WASABI_API_DATAPUMP->pushData(listento, DeregisterListenerGUID, &getGuid(), sizeof(GUID));    
  }
private:
  GUID listento;
};

#endif // _WASABI_PUMP_BCAST_H
