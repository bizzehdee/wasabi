#include "pumphandler.h"
#include "pumpobj.h"

int LocalPumpHandler::registerSink(DataSink *sink) {
  sinks.addItem(sink->getGuid(), sink);
  return 1;
}
int LocalPumpHandler::registerSource(DataSource *source) {
  sources.addItem(source->getGuid(), source);
  return 1;
}

int LocalPumpHandler::deregisterSink(DataSink *sink) {
  return sinks.delItem(sink->getGuid());
}
int LocalPumpHandler::deregisterSource(DataSource *source) {
  return sources.delItem(source->getGuid());
}

int LocalPumpHandler::pushData(GUID sink, GUID pusher, void *data, unsigned long sz) {
  DataSink *pSink = NULL;
  sinks.getItem(sink, &pSink);
  if(pSink) return pSink->onData(pusher, data, sz);
  return 0;
}

int LocalPumpHandler::pullData(GUID source, GUID puller, void *data, unsigned long sz, unsigned long *actual_sz) {
  DataSource *pSource = NULL;
  sources.getItem(source, &pSource);
  if(pSource) return pSource->getData(puller, data, sz, actual_sz);
  return 0;
}
