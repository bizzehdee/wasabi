#include <precomp.h>
#include "sourcesink.h"

#ifdef CBCLASS
#undef CBCLASS
#endif

#define CBCLASS EventSourceX
START_DISPATCH;
  VCB(EVENTSOURCE_REGISTERSINK, eventsource_registerSink);
  VCB(EVENTSOURCE_UNREGISTERSINK, eventsource_unregisterSink);
END_DISPATCH;
#undef CBCLASS

// -----------------------------------------------------------------------

EventSource::EventSource() {
}

// When this object dies, notify all sinks by triggering the onDestroy event
EventSource::~EventSource() { 
  trigger_onDestroy(); 
}


// An event sink is now watching us, remember its pointer so we can send it our callbacks
void EventSource::eventsource_registerSink(EventSink *sink) { 
  sinks.addItem(sink); 
}

void EventSource::eventsource_unregisterSink(EventSink *sink) { 
  sinks.removeItem(sink); 
}

// This is the trigger function for the onDestroy event
void EventSource::trigger_onDestroy() {
  // send the event to all sinks, we do this one ourselves rather than 
  // calling eventsource_onEvent(EventSource_GUID, ONDESTROY); so as to
  // prevent anybody from aborting the event by returning 1 : all sinks 
  // are guaranteed to receive all their objects' onDestroy
  foreach(sinks)
    sinks.getfor()->eventsink_eventDispatch(this, EventSource_GUID, ONDESTROY, NULL, 0);
  endfor;
}

// -----------------------------------------------------------------------

EventSink::EventSink() {
}

// When the sink dies, we remove our pointers from the item's sink lists
EventSink::~EventSink() {
  foreach(items)
    items.getfor()->eventsource_unregisterSink(this);
  endfor;
}

// Dispatch event messages received from an event source
int EventSink::eventsink_eventDispatch(EventSource *item, GUID event_class, int msg, void **params, int nparam) {
  // Handle our custom events
  if (event_class == EventSource_GUID) {
    switch (msg) {
      case EventSource::ONDESTROY: 
        int dummy = onDestroy(item); 
        eventsink_unregisterItem(item);
        return 0;
    }
  } 
  // We have no ancestry as an event sink, if we did, we should have been calling our ancestor's eventsink_eventDispatch 
  // here and we should have been returning its return value
  return 0;
}


// Begin watching an item's events
void EventSink::eventsink_registerItem(EventSource *d) { 
  items.addItem(d); 
  d->eventsource_registerSink(this);
}

// Stop watching an item's events
void EventSink::eventsink_unregisterItem(EventSource *d) {
  d->eventsource_unregisterSink(this);
  items.removeItem(d); 
}
