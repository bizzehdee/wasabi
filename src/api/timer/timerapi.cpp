#include <precomp.h>
#include <api/api.h>
#include <api/timer/timerapi.h>
#include <api/timer/tmultiplex.h>

timer_api *timerApi = NULL;

TimerApi::TimerApi() {
  mainmultiplex = new MainTimerMultiplexer();
/*  genwnd = new GenWnd();
  genwnd->setStartHidden(TRUE);
  genwnd->init(WASABI_API_APP->main_gethInstance(), INVALIDOSWINDOWHANDLE, TRUE);
  GenWnd::setTimerGenWnd(genwnd);*/
}

TimerApi::~TimerApi() {
  mainmultiplex->shutdown();
//  delete genwnd;
//  genwnd = NULL;
  delete mainmultiplex;
}

void TimerApi::timer_add(TimerClient *client, int id, int ms) {
  mainmultiplex->add(client, id, ms);
}

void TimerApi::timer_remove(TimerClient *client, int id) {
  mainmultiplex->remove(client, id);
}


