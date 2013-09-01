#ifndef __TIMER_SVC_H
#define __TIMER_SVC_H

#include <api/timer/api_timer.h>

class ApiInit;

class TimerApi : public timer_apiI {
  friend class ApiInit;
  public:
    TimerApi();
    virtual ~TimerApi();
    virtual void timer_add(TimerClient *client, int id, int ms);
    virtual void timer_remove(TimerClient *client, int id = -1);

  protected:
};


#endif
