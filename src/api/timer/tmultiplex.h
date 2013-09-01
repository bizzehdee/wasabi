#ifndef __MAIN_TIMERMULTIPLEXER_H
#define __MAIN_TIMERMULTIPLEXER_H

#include <api/timer/timermul.h>
#include <bfc/depend.h>
#include <bfc/string/string.h>
#include <api/timer/timerclient.h>

class VirtualTimer {
  public:
    VirtualTimer(TimerClient *_client, int _id, Dependent *depend);
    virtual ~VirtualTimer() { }

    TimerClient *client, *mclient;
    Dependent *dep;
    String name;
    int id;
};

class MainTimerMultiplexer : public TimerMultiplexer, public TimerMultiplexerClient, public DependentViewerI  {
  public:

    MainTimerMultiplexer();
    virtual ~MainTimerMultiplexer();
    
    virtual void add(TimerClient *client, int id, int ms);
    virtual void remove(TimerClient *client, int id);
  
    virtual void onMultiplexedTimer(void *data, int skip, int mssincelast);
    virtual void onServerTimer();

    virtual int viewer_onItemDeleted(Dependent *item);

  private:

    int isValidTimerClientPtr(TimerClient *tc, Dependent *dep);

    int haveClient(TimerClient *client);
    PtrList<VirtualTimer> timerclients;
    PtrListQuickSortedByPtrVal<TimerClient> masters;
  
};

extern MainTimerMultiplexer *mainmultiplex;

#endif
