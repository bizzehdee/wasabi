#ifndef _SERVICE_H
#define _SERVICE_H

// this file defines the wasabi service factory class. this isn't the actual
// service, just the interface to get the service pointer

#include <bfc/dispatch.h>
#include "services.h"
#include <api/api.h>

#include "waservicefactory.h"
#include "waservicefactorybase.h"
#include "waservicefactoryt.h"
#include "waservicefactorytsingle.h"

namespace SvcNotify {
  enum {
    ONREGISTERED=100,	// init yourself here -- not all other services are registered yet
    ONSTARTUP=200,	// everyone is initialized, safe to talk to other services
    ONAPPRUNNING=210,	// app is showing and processing events
    ONSHUTDOWN=300,	// studio is shutting down, release resources from other services
    ONDEREGISTERED=400,	// bye bye
    ONDBREADCOMPLETE=500,// after db is read in (happens asynchronously after ONSTARTUP)
    ONBEFORESHUTDOWN=600, // system is about to shutdown, call WASABI_API_APP->main_cancelShutdown() to cancel
  };
};

#endif
