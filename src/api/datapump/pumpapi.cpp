#include <precomp.h>
#include "pumpapi.h"
#include "pumphandler.h"
#include "pumpobj.h"

#include <bfc/map.h>
#include <bfc/critsec.h>

CriticalSection cs;
Map<PumpClass, PumpHandler *> handlers;
Map<GUID, PumpClass> sinks;
Map<GUID, PumpClass> sources;

void PumpApi::onShutdown() {
  INCRITICALSECTION(cs);
  for (int i = 0; i < handlers.getNumItems(); i++) {
    PumpHandler *handler = handlers.enumItemByPos(i, NULL);
    delete handler;
  }
  handlers.deleteAll();
}

int PumpApi::addPumpHandler(PumpHandler *handler) {
  INCRITICALSECTION(cs);
  handlers.addItem(handler->getPumpClass(), handler);
  return 1;
}
PumpHandler *PumpApi::getPumpHandler(PumpClass pclass) {
  INCRITICALSECTION(cs);
  PumpHandler *ret = NULL;
  handlers.getItem(pclass, &ret);
  return ret;
}

int PumpApi::registerSink(PumpClass pclass, DataSink *sink) {
  INCRITICALSECTION(cs);
  PumpHandler *handler;
  if (handlers.getItem(pclass, &handler)) {
    handler->registerSink(sink);
    sinks.addItem(sink->getGuid(), pclass);
    return 1;
  }
  return 0;
}
int PumpApi::registerSource(PumpClass pclass, DataSource *source) {
  INCRITICALSECTION(cs);
  PumpHandler *handler;
  if (handlers.getItem(pclass, &handler)) {
    handler->registerSource(source);
    sources.addItem(source->getGuid(), pclass);
    return 1;
  }
  return 0;
}

int PumpApi::deregisterSink(DataSink *sink) {
  DebugString("PumpApi::deregisterSink(): \"%s\" ...", sink->toChar().vs());
  PumpHandler *handler = handlerForSink(sink->getGuid());
  if(!handler) return 0;
  if(!handler->deregisterSink(sink)) return 0;
  INCRITICALSECTION(cs);
  sinks.delItem(sink->getGuid());
  return 1;
}
int PumpApi::deregisterSource(DataSource *source) {
  DebugString("PumpApi::deregisterSource(): \"%s\" ...", source->toChar().vs());
  PumpHandler *handler = handlerForSource(source->getGuid());
  if(!handler) return 0;
  if(!handler->deregisterSource(source)) return 0;
  INCRITICALSECTION(cs);
  sources.delItem(source->getGuid());
  return 1;
}

int PumpApi::pushData(GUID sink, GUID pusher, void *data, unsigned long sz) {
  PumpHandler *handler = handlerForSink(sink);
  if(handler) return handler->pushData(sink, pusher, data, sz);
  return 0;
}
int PumpApi::pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz) {
  PumpHandler *handler = handlerForSource(source);
  if(handler) return handler->pullData(source, puller, data, sz, actual_sz);
  return 0;
}

PumpHandler *PumpApi::handlerForSink(GUID sink) {
  INCRITICALSECTION(cs);
  PumpHandler *ret = NULL;
  PumpClass pclass;
  sinks.getItem(sink, &pclass);
  handlers.getItem(pclass, &ret);
  return ret;
}
PumpHandler *PumpApi::handlerForSource(GUID source) {
  INCRITICALSECTION(cs);
  PumpHandler *ret = NULL;
  PumpClass pclass;
  sources.getItem(source, &pclass);
  handlers.getItem(pclass, &ret);
  return ret;
}
