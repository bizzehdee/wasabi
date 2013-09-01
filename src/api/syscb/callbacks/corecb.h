#ifndef _CORECB_H
#define _CORECB_H

#include <bfc/std.h>
#include <bfc/dispatch.h>
#include <bfc/reentryfilter.h>

// don't derive from this
class NOVTABLE COMEXP CoreCallback : public Dispatchable {
protected:
  CoreCallback() {}

public:
  int ccb_notify(int msg, int param1=0, int param2=0) {
    return _call(CCB_NOTIFY, 0, msg, param1, param2);
  }

  // class Dispatchable codes
  enum {
    CCB_NOTIFY = 100,
  };

  // various ccb_notify notifications. these are *not* the Dispatchable codes
  enum {
    REGISTER        = 100,
    DEREGISTER      = 200,
    NEXTFILE        = 300,

    STARTED         = 500,
    STOPPED         = 600,
    PAUSED          = 700,
    UNPAUSED        = 800,
    SEEKED          = 900,

    VOLCHANGE       = 2000,
    EQSTATUSCHANGE  = 2100,
    EQPREAMPCHANGE  = 2200,
    EQBANDCHANGE    = 2300,
    EQAUTOCHANGE    = 2400,
    PANCHANGE       = 2500,
    
    STATUSMSG       = 3000,
    WARNINGMSG      = 3100,
    ERRORMSG        = 3200,
    ERROROCCURED    = 3300,

    TITLECHANGE     = 4000,
    TITLE2CHANGE    = 4100,
    INFOCHANGE      = 4200,
    SAMPLERATECHANGE = 4210,
    BITRATECHANGE   = 4220,
    CHANNELSCHANGE  = 4230,
    URLCHANGE       = 4300, 
    LENGTHCHANGE    = 4400,

    NEEDNEXTFILE    = 5100,
    SETNEXTFILE     = 5200,

    ABORTCURRENTSONG= 6000,
    
    ENDOFDECODE     = 7000,

    ONFILECOMPLETE     = 8000,

    CONVERTERSCHAINREBUILT = 9000,

    MEDIAFAMILYCHANGE = 10000,
  };
};

// this class does NOT automatically deregister itself when you destruct it
// that is up to you, and obviously, if you fail to do so, you'll crash the
// whole app. and they'll all laugh at you. see class CoreHandle
class NOVTABLE CoreCallbackI : public CoreCallback {
protected:
  CoreCallbackI() { } // no instantiating this on its own

public:
  virtual int corecb_onStarted() { return 0; }
  virtual int corecb_onStopped() { return 0; }
  virtual int corecb_onPaused() { return 0; }
  virtual int corecb_onUnpaused() { return 0; }
  virtual int corecb_onSeeked(int newpos) { return 0; }

  virtual int corecb_onVolumeChange(int newvol) { return 0; }
  virtual int corecb_onPanChange(int newpan) { return 0; }

  virtual int corecb_onEQStatusChange(int newval) { return 0; }
  virtual int corecb_onEQPreampChange(int newval) { return 0; }
  virtual int corecb_onEQBandChange(int band, int newval) { return 0; }
  virtual int corecb_onEQAutoChange(int newval) { return 0; }

  virtual int corecb_onStatusMsg(const char *text) { return 0; }
  virtual int corecb_onWarningMsg(const char *text) { return 0; }
  virtual int corecb_onErrorMsg(const char *text) { return 0; }

  virtual int corecb_onTitleChange(const char *title) { return 0; }
  virtual int corecb_onTitle2Change(const char *title2) { return 0; }
  virtual int corecb_onInfoChange(const char *info) { return 0; }
  virtual int corecb_onBitrateChange(int khz) { return 0; }
  virtual int corecb_onSampleRateChange(int kbps) { return 0; }
  virtual int corecb_onChannelsChange(int nch) { return 0; }
  virtual int corecb_onUrlChange(const char *url) { return 0; }
  virtual int corecb_onLengthChange(int newlength) { return 0; }

  virtual int corecb_onNextFile() { return 0; }
  virtual int corecb_onNeedNextFile(int fileid) { return 0; }
  virtual int corecb_onSetNextFile(const char *playstring) { return 0; }

  virtual int corecb_onErrorOccured(int severity, const char *text) { return 0; }

  // return 1 in this callback to make the current callback chain to abort
  virtual int corecb_onAbortCurrentSong() { return 0; }

  virtual int corecb_onEndOfDecode() { return 0; }

  virtual int corecb_onFileComplete(const char *playstring) { return 0; }

  virtual int corecb_onConvertersChainRebuilt() { return 0; }

  virtual int corecb_onMediaFamilyChange(const char *newfamily) { return 0; }

  virtual int ccb_notify(int msg, int param1=0, int param2=0);
protected:
  RECVS_DISPATCH;
  ReentryFilterObject filter;
};

#endif
