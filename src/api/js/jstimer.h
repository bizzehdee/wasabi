#ifndef JSTIMER_H
#define JSTIMER_H

#include "js/JSJSTimer.h"
#include "sourcesink/JSTimerEventSource.h"
#include <api/timer/timerclient.h>

class JSTimer : public TimerClientDI,
                public JSScriptable,
                public JSTimerEventSource,
                public JSJSTimer {
  
  public:
  
    JSTimer() : m_delay(1) {}
    virtual ~JSTimer() { m_set = 0; }

    virtual void timerclient_timerCallback(int id) { onTimer(); };

    SCRIPT void setDelay(int delay);
    SCRIPT void start();
    SCRIPT void stop();
    SCRIPT EVENT void onTimer();

  private:
    int m_delay;
    int m_set;
};


#endif
