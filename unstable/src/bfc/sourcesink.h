#ifndef _SourceSink_H_Included
#define _SourceSink_H_Included

#include <bfc/ptrlist.h>
#include <bfc/dispatch.h>

class EventSource;
class EventSink;

// -------------------------------------------------------------------------------------------------------

class EventSinkX {
public:
  virtual int eventsink_eventDispatch(EventSource *item, GUID event_class, int msg, void **params=NULL, int nparam=0)=0;
};

// -------------------------------------------------------------------------------------------------------
// EventSource - the basic class for implementing event sources
// -------------------------------------------------------------------------------------------------------

// {8B1C0D94-C7F8-4a9e-BC67-A41752A4FB29}
static const GUID EventSource_GUID = 
{ 0x8b1c0d94, 0xc7f8, 0x4a9e, { 0xbc, 0x67, 0xa4, 0x17, 0x52, 0xa4, 0xfb, 0x29 } };

// -------------------------------------------------------------------------------------------------------
class EventSourceDispatch : public Dispatchable {
protected:
  EventSourceDispatch() {}

  enum {
    EVENTSOURCE_REGISTERSINK = 10,
    EVENTSOURCE_UNREGISTERSINK = 20,
  };

public:
  void eventsource_registerSink(EventSink *sink);
  void eventsource_unregisterSink(EventSink *sink);
};

inline void EventSourceDispatch::eventsource_registerSink(EventSink *sink) {
  _voidcall(EVENTSOURCE_REGISTERSINK, sink);
}

inline void EventSourceDispatch::eventsource_unregisterSink(EventSink *sink) {
  _voidcall(EVENTSOURCE_UNREGISTERSINK, sink);
}

// -------------------------------------------------------------------------------------------------------
class EventSourceX : public EventSourceDispatch {
protected:
  EventSourceX() {}
  RECVS_DISPATCH;
public:
  virtual void eventsource_registerSink(EventSink *sink)=0;
  virtual void eventsource_unregisterSink(EventSink *sink)=0;
};

// -------------------------------------------------------------------------------------------------------
class EventSource : public EventSourceX {
public:
  enum {
    ONDESTROY = 10,
  };

  EventSource();
  virtual ~EventSource();

  void eventsource_registerSink(EventSink *sink);
  void eventsource_unregisterSink(EventSink *sink);

  void trigger_onDestroy();

  int eventsource_onEvent(GUID eventclass, int msg) {
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, NULL, 0)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1) {
    void *params[1] = { &param1 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 1)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2) {
    void *params[2] = { &param1, &param2 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 2)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3) {
    void *params[3] = { &param1, &param2, &param3 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 3)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) {
    void *params[4] = { &param1, &param2, &param3, &param4 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 4)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5) {
    void *params[5] = { &param1, &param2, &param3, &param4, &param5 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 5)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6) {
    void *params[6] = { &param1, &param2, &param3, &param4, &param5, &param6 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 6)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7) {
    void *params[7] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 7)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8) {
    void *params[8] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 8)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9) {
    void *params[9] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 9)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10) {
    void *params[10] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 10)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14) {
    void *params[14] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 14)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14, PARAM15 param15, PARAM16 param16) {
    void *params[16] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 , &param15 , &param16 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 16)) return 1;
    endfor;
    return 0;
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16, class PARAM17>
  int eventsource_onEvent(GUID eventclass, int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14, PARAM15 param15, PARAM16 param16, PARAM17 param17) {
    void *params[17] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 , &param15 , &param16 , &param17 };
    foreach(sinks)
      if (sinks.getfor()->eventsink_eventDispatch(this, eventclass, msg, params, 17)) return 1;
    endfor;
    return 0;
  }

protected:
  PtrList<EventSinkX> sinks;
};


// -------------------------------------------------------------------------------------------------------
// EventSink - the basic class for receiving event callbacks
// -------------------------------------------------------------------------------------------------------

class EventSink : public EventSinkX {
public:
  EventSink();
  virtual ~EventSink();

  // This is where the onDestroy event is received, override this method
  virtual int onDestroy(EventSource *dep) { return 0; }

  virtual int eventsink_eventDispatch(EventSource *item, GUID event_class, int msg, void **params=NULL, int nparam=0);

  void eventsink_registerItem(EventSource *d);
  void eventsink_unregisterItem(EventSource *d);
  
private:
  PtrList<EventSource> items;
};

// -------------------------------------------------------------------------------------------------------
// utility classes
// -------------------------------------------------------------------------------------------------------

template<class T>
class AutoPtrList : public PtrList<T>, public EventSink {
  void copyFrom(const PtrList<T> *from) {
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    foreach(*from)
      eventsink_registerItem((*this)->getfor());
    endfor;
    PtrList<T>::copyFrom(from); 
  }
  void appendFrom(const PtrList<T> *from) { 
    foreach(*from)
      eventsink_registerItem((*from)->getfor());
    endfor;
    PtrList<T>::appendFrom(from); 
  }
  T* addItem (const T *item, int pos = PTRLIST_POS_LAST, int inc = DEF_PTRLIST_INCREMENT) {
    eventsink_registerItem(item);
    return PtrList<T>::addItem(item, pos, inc);
  }
  int removeItem(T *item) {
    eventsink_unregisterItem(item);
    return PtrList<T>::removeItem(item);
  }
  void removeEveryItem(const T *item) { 
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    PtrList<T>::removeEveryItem(item); 
  }
  void removeByPos(int pos) { 
    eventsink_unregisterItem(enumItem(pos));
    PtrList<T>::removeByPos(pos); 
  }
  T *setItem(const T *item, int pos) {
    eventsink_unregisterItem(enumItem(pos));
    return PtrList<T>::setItem(item, pos);
  }
  void removeLastItem() { 
    eventsink_unregisterItem(enumItem(getNumItems()-1));
    PtrList<T>::removeLastItem();
  }
  void removeAll() { 
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    PtrList<T>::removeAll(); 
  }
  void freeAll() { 
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    PtrList<T>::freeAll(); 
  }
  void deleteAll() {
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    PtrList<T>::deleteAll(); 
  }
  void deleteAllSafe() {
    foreach(*this)
      eventsink_unregisterItem((*this)->getfor());
    endfor;
    PtrList<T>::deleteAllSafe(); 
  }
  void deleteItem(int item) {
    if (item < getNumItems()) {
      eventsink_unregisterItem(enumItem(item));
      PtrList<T>::deleteItem(item); 
    }
  }
  void deleteItem(T *item) {
    eventsink_unregisterItem(item);
    PtrList<T>::deleteItem(item); 
  }
  virtual int onDelete(EventSource *object) {
    removeItem(static_cast<T*>(object));
  }
};




#endif // _SourceSink_H_Included
