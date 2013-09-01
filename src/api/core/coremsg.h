#ifndef _COREMSG_H
#define _COREMSG_H

typedef enum { 

/* WCM_ERROROCCURED
** An error occured in playback.
** data1 is severity
**   0: playback should stop (soundcard driver error, etc)
**   1: warning (something recoverable, like file not found, etc)
**   2: status update (nothing really wrong)
** on severity of 0, you will also receive WCM_STOPPED if previous state was playing.
** data2 is error string
*/
WCM_ERROROCCURED,


/* WCM_NEEDNEXTFILE
** Called by player when the next file is needed.
** data1 is id of previous file, or -1 if no previous file 
*/
WCM_NEEDNEXTFILE,

/* WCM_TITLECHANGE
** Called by player when the title changes (or the player goes to the next file)
** data1 is a (char *) to new title string
** data2 is new uniqueid
*/
WCM_TITLECHANGE,

/* WCM_INFOCHANGE
** Called by player when the info or length changes (or the player goes to the next file)
** data1 is a (char *) to new info string
** data2 is an integer that specifies, in ms, the length of the current track (or -1 if
** infinite or unknown)
*/
WCM_INFOCHANGE,

/* WCM_URLCHANGE
** Called by player when the url changes (or the player goes to the next file and that file has a url associated with it)
** data1 is a (char *) to new url string
*/
WCM_URLCHANGE,

/* WCM_NEXTFILE
** Called by player when the next file (the last one specified with SetNextFile()) starts
** playing.
** WCM_TITLECHANGE and WCM_INFOCHANGE should follow.
*/
WCM_NEXTFILE,

/* WCM_STOPPED
** Called by player when going from a playing state to a stopped state.
*/
WCM_STOPPED,

/* WCM_STARTED
** Called by player when going from a stopped state to a playing state.
*/
WCM_STARTED,

/* WCM_PAUSED
** Called by player when going from unpaused state to paused state
*/
WCM_PAUSED,

/* WCM_UNPAUSED
** Called by player when going from paused state to unpaused state
*/
WCM_UNPAUSED,

/* WCM_TITLE2CHANGE
** called by player if there is a secondary title to set (i.e. shoutcast)
** data1 is a (char *) to secondary title string
*/
WCM_TITLE2CHANGE,

/* WCM_ENDOFDECODE
** called by player to signal the current file has just finished its decoding
*/
WCM_ENDOFDECODE,

/* WCM_ABORTCURRENTSONG
** called by player if the song has been aborted
*/
WCM_ABORTCURRENTSONG,

WCM_LENGTHCHANGE,

WCM_CONVERTERSCHAINREBUILT,

WCM_MEDIAFAMILYCHANGE,

WCM_ENDOFMSGS

} WPlayerMsg; /* WPlayerMsg is sent via callback */

enum {
  USERMSG_NOP = WM_USER+2000,
  USERMSG_CORECALLBACK,

// this space taken by the wcore callbacks, offset by USERMSG_CORECALLBACK

  USERMSG_ENDCORECALLBACK = USERMSG_CORECALLBACK + WCM_ENDOFMSGS
};

#endif
