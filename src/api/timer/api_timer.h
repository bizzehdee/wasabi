#ifndef __TIMER_API_H
#define __TIMER_API_H

// Under linux, the Timer API requires the Linux API

#include <bfc/dispatch.h>
#include <bfc/platform/platform.h>

#include <api/service/services.h>

class TimerClient;

class timer_api : public Dispatchable {
  public:
    void timer_add(TimerClient *client, int id, int ms);
    void timer_remove(TimerClient *client, int id = -1);

  enum {
    TIMER_API_ADD = 0,
    TIMER_API_REMOVE = 10,
  };
};

inline void timer_api::timer_add(TimerClient *client, int id, int ms) {
  _voidcall(TIMER_API_ADD, client, id, ms);
}

inline void timer_api::timer_remove(TimerClient *client, int id) {
  _voidcall(TIMER_API_REMOVE, client, id);
}

class timer_apiI : public timer_api {
  public:
    static const char *getServiceName() { return "Timer API"; }
    static GUID getServiceType() { return WaSvc::UNIQUE; }
    
    virtual void timer_add(TimerClient *client, int id, int ms)=0;
    virtual void timer_remove(TimerClient *client, int id = -1)=0;

  protected:
    RECVS_DISPATCH;
};

// {3130D81C-AE1F-4954-9765-698473B627B0}
static const GUID timerApiServiceGuid = 
{ 0x3130d81c, 0xae1f, 0x4954, { 0x97, 0x65, 0x69, 0x84, 0x73, 0xb6, 0x27, 0xb0 } };

extern timer_api *timerApi;

#endif
