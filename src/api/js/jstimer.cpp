#include <precomp.h>
#include "jstimer.h"

void JSTimer::setDelay(int delay) {
  m_set = 0;
  m_delay = delay;
}

void JSTimer::start() {
  m_set = 1;
  timerclient_setTimer(0, m_delay);
}

void JSTimer::stop() {
  m_set = 0;
  timerclient_killTimer(0);
}

void JSTimer::onTimer() {
  eventSource_onTimer();
  for (int i=0;m_set && i<timerclient_getSkipped();i++)
    eventSource_onTimer();
}

