#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_pumpi.h"
//?>

#include "pumpapi.h"

api_pump *pumpApi = NULL;

api_pumpI::api_pumpI() { }
api_pumpI::~api_pumpI() { PumpApi::onShutdown(); }

int api_pumpI::addPumpHandler(PumpHandler *handler) {
  return PumpApi::addPumpHandler(handler);
}
PumpHandler *api_pumpI::getPumpHandler(PumpClass eclass) {
  return PumpApi::getPumpHandler(eclass);
}

int api_pumpI::registerSink(PumpClass eclass, DataSink *sink) {
  return PumpApi::registerSink(eclass, sink);
}
int api_pumpI::registerSource(PumpClass eclass, DataSource *source) {
  return PumpApi::registerSource(eclass, source);
}
int api_pumpI::deregisterSink(DataSink *sink) {
  return PumpApi::deregisterSink(sink);
}
int api_pumpI::deregisterSource(DataSource *source) {
  return PumpApi::deregisterSource(source);
}
  
int api_pumpI::pushData(GUID sink, GUID pusher, void *data, unsigned long sz) {
  return PumpApi::pushData(sink, pusher, data, sz);
}
int api_pumpI::pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz) { 
  return PumpApi::pullData(source, puller, data, sz, actual_sz);
}
