#ifndef _API_H
#define _API_H

#include <wasabicfg.h>

#ifndef WARNING
  #define WARNING_TOSTR(text) #text
  #define WARNING_TOSTR1(text) WARNING_TOSTR(text)
  #define WARNING(text) message(__FILE__ "(" WARNING_TOSTR1(__LINE__) ") : " text)
#endif

#include <bfc/dispatch.h>
#include <bfc/platform/platform.h>  // basic type defs
#ifdef WASABI_COMPILE_SCRIPT
#include <api/script/vcputypes.h>
#endif
#ifdef WASABI_COMPILE_CONSOLE
#include <api/console/console.h>
#endif
#define LATEST_API_VERSION  1

class CanvasBase;  // see ../common/canvas.h
class CfgItem;    // see ../attribs/cfgitem.h
class CoreCallback;  // see corecb.h
class EmbedWnd;    // see ../common/embedwnd.h
class ItemSequencer;  // see ../common/sequence.h
class RootWnd;    // see ../bfc/rootwnd.h
class SkinFilter;  // see skinfilter.h
class SysCallback;  // see syscb.h
class XmlReaderCallback; // see xmlreader.h
class waServiceFactory;  // see service.h
class MetricsCallback;  // see metricscb.h
class ScriptObjectController; // see objcontroller.h
class dbSvcScanner; // see ../studio/services/svc_db.h
class TimerClient; // see ../bfc/timerclient.h
class WindowHolder; // see ../bfc/wnds/wndholder.h
class PopupExitCallback; // see ../bfc/popexitcb.h
class GuiObject; // see ../common/script/guiobject.h
class Region; // see ../bfc/region.h
class RegionServer; // see ../bfc/region.h
class ColorThemeGroup; // see ../bfc/colortheme.h
class SkinItem; // see ../bfc/skinitem.h

typedef unsigned int CoreToken;
typedef unsigned long ARGB32;
class WaComponent;

#ifndef MODALWND_DEF
#define MODALWND_DEF
RootWnd *const MODALWND_NOWND = reinterpret_cast<RootWnd*>(-1);
#endif

#ifndef WASABINOMAINAPI

/**
  A collection of generalized methods that operate on various
  functional pieces of Wasabi. Examples: Filereading / writer, config file
  manipulation, services, windowing / skinning system, xml reading,
  callback registration, etc.

  The API or API Pointer is accessible in any component, except during
  construction and destruction. So, safe use of the API pointer is
  from the onCreate event to the onDestroy event of the WAComponentClient
  class.

  Any use of the API during contruction or destruction of your component
  will cause erradic behavior and often crashes.

  @see WAComponentClient
  @short Collection of generalized methods for Wasabi.
  @author Nullsoft
  @ver 1.0
*/
class ComponentAPI : public Dispatchable {
public:

  /**
    Get the GUID of the component that owns this api.

    @ret API version number.
  */
  GUID getThisGuid();

  /**
    Get the component that owns this api.

    @ret API version number.
  */
  WaComponent *getThisComponent();

  /**
    Get the version number of the API.

    @ret API version number.
  */
  int getVersion();  // the API version #

// studio.exe info

  // name of the application
  /**
    Get the application name.

    @ret Application name
  */
  const char *main_getAppName();

  // text string of version
  /**
    Get the main version string.

    @ret Main version string.
  */
  const char *main_getVersionString();

  // not-very-reliable auto-incremented build number... mostly just interesting
  /**
    Get the main auto-incremented build number.

    @ret Main build number.
  */
  unsigned int main_getBuildNumber();

  // the GUID of studio
  /**
    Get the main GUID.

    @ret Main GUID.
  */
  GUID main_getGUID();

  /**
    Returns OS handle to main thread of app. Threads can check this
    to detect app shutdown in a threadsafe manner. Must close this handle
    in OS-specific manner.
    @ret Handle of main thread. Caller must close handle when finished with it.
  */
  OSTHREADHANDLE main_getMainThreadHandle();

  // the HINSTANCE (try not to abuse :)
  /**
    Get a handle to the main instance.

    This is NOT PORTABLE and should NOT BE USED
    except in DIRE NEED.

    @ret Main HINSTANCE.
  */
  OSMODULEHANDLE main_gethInstance();

  // the icon
  /**
    Get a handle to the main icon.

    @ret Main OSICONHANDLE.
  */
  OSICONHANDLE main_getIcon(int _small);

  // the master window
  /**
    Get the main root window.

    @ret Main root window.
  */
  RootWnd *main_getRootWnd();

  // the command line
  /**
    Get the command line with which studio
    was launched.

    ie. studio.exe file.mp3

    @ret Main command line.
  */
  const char *main_getCommandLine();

  // the window title
  /**
    Set the main window title.

    @param title New window title.
  */
  void main_setWindowTitle(const char *title);

  /**
    Shuts down the wasabi app.
    @param deferred If FALSE, the app will shutdown immediately. If TRUE, the app will shutdown after the current event being processed is complete. Defaults to TRUE.
  */
  void main_shutdown(int deferred=TRUE);

  /**
    Cancel application shutdown. This function should be called as a response to runlevel callback ONBEFORESHUTDOWN if the app is not ready to quit.
  */
  void main_cancelShutdown();
  int main_isShuttingDown();

  /**
    Returns the status of the Internet connection.
    0: offline
    1: connected
  */
  int main_isInetAvailable();
  int app_getInitCount();

// paths
  /**
    Get the path to the main executable
    of the application.

    @ret Path to the main executable.
  */
  const char *path_getAppPath();  // the path to the main exe

  /**
    Get the path to a specific component by
    using it's GUID. If no GUID is specified,
    the GUID of the calling component will be used.

    @see GUID
    @ret Path of the component.
    @param g GUID of the component.
  */
  const char *path_getComponentPath(GUID g=INVALID_GUID);// the path to the wac. if you don't specify a guid uses the guid of calling component

  /**
    Get the component data path. This is
    where it is safe for you to write data.

    If you want to write user settings data,
    please use path_getUserSettingsPath().

    @see path_getUserSettingsPath()
    @ret Data path.
  */
  const char *path_getComponentDataPath(); // where you should write data

  /**
    Get the user settings path. This is where
    user specific settings data should be saved.

    If you want to write generic data,
    please use path_getComponentDataPath().

    @see path_getComponentDataPath()
    @ret User settings path.
  */
  const char *path_getUserSettingsPath();  // where settings type stuff should be stored

// config file stuff
  void setIntPrivate(const char *name, int val);
  int getIntPrivate(const char *name, int def_val);
  void setIntArrayPrivate(const char *name, const int *val, int nval);
  int getIntArrayPrivate(const char *name, int *val, int nval);
  void setStringPrivate(const char *name, const char *str);
  int getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str);
  int getStringPrivateLen(const char *name);
  void setIntPublic(const char *name, int val);
  int getIntPublic(const char *name, int def_val);
  void setStringPublic(const char *name, const char *str);
  int getStringPublic(const char *name, char *buf, int buf_len, const char *default_str);


// playback core
// those functions are DEPRECATED. use the CoreHandle class instead. THEY WILL GET AWAY SOON! :)
  /**
    Get the list of supported extensions
    from the core. This is a zero delimited list
    with double zero termination.

    @see core_getExtSupportedExtensions()
    @see core_registerExtension()
    @see core_unregisterExtension()
    @see core_getExtensionFamily()
    @ret List of supported extensions.
  */
  const char *core_getSupportedExtensions();//just the *.mp3 or whatever

  /**
    Get the extended list of supported extensions
    from the core. This will include the proper
    names of the extensions, for example:
    "MP3 Files (*.mp3)".

    This is returned as a zero delimited list with double
    zero termination.

    @see core_getSupportedExtensions()
    @see core_registerExtension()
    @see core_unregisterExtension()
    @see core_getExtensionFamily()
    @ret Extended list of supported extensions.
  */
  const char *core_getExtSupportedExtensions();// including names


  CoreToken core_create(); // returns a token
  int core_free(CoreToken core);

  /**
    Set the next file to be played by the core.

    @ret 1, success; 0, failure;
    @param core         Core token of the core.
    @param playstring   Playstring of the next file to be played.
  */
  int core_setNextFile(CoreToken core, const char *playstring);

  /**
    Get the playback status of the core.

    @ret -1, Paused; 0, Stopped; 1, Playing;
    @param core Core token of the core.
  */
  int core_getStatus(CoreToken core);  // returns -1 if paused, 0 if stopped and 1 if playing

  /**
    Get the playstring of the currently playing
    item.

    @ret Playstring of the currently playing item.
    @param core Core token of the core.
  */
  const char *core_getCurrent(CoreToken core);

  /**
    Get the index number of the currently
    playing item of the currently registered
    sequencer.

    @ret Index number (in the sequencer) of the item playing.
    @param core Core token of the core.
  */
  int core_getCurPlaybackNumber(CoreToken core);

  /**
    Get the playback position of the currently
    playing file.

    @see core_setPosition()
    @see core_getLength()
    @ret Position in the file (in milliseconds).
    @param core Core token of the core.
  */
  int core_getPosition(CoreToken core);

  /**
    Help?

    @see core_getPosition()
    @see core_getLength()
    @ret Current write position (in milliseconds).
  */
  int core_getWritePosition(CoreToken core);

  /**
    Seek to a specific position in the
    currently playing item.

    @see core_getPosition()
    @see core_getLength()
    @ret 1, Success; 0, Failure;
    @param core Core token of the core.
    @param ms Position in the file (in milliseconds, 0 being the beginning).
  */
  int core_setPosition(CoreToken core, int ms);

  /**
    Get the length of the currently
    playing item.

    @see core_getPosition()
    @see core_setPosition()
    @ret Length of the item (in milliseconds).
    @param core Core token of the core.
  */
  int core_getLength(CoreToken core);

  // this method queries the core plugins directly, bypassing the db
  int core_getPluginData(const char *playstring, const char *name,
    char *data, int data_len, int data_type=0);  // returns size of data

  /**
    Get the volume of the core.

    @see core_setVolume()
    @ret Volume (0 to 255).
    @param core Core token of the core.
  */
  unsigned int core_getVolume(CoreToken core);

  /**
    Set the volume of the core.

    @see core_getVolume()
    @param vol Volume (0 to 255).
    @param core Core token of the core.
  */
  void core_setVolume(CoreToken core, unsigned int vol);  // 0..255

  /**
    Get the panning value of the core.

    @see core_setPan()
    @ret Panning value (-127 [left] to 127 [right]).
    @param core Core token of the core.
  */
  int core_getPan(CoreToken core);            // -127..127

  /**
    Set the panning value of the core.

    @see core_getPan()
    @param core Core token of the core.
    @param bal Panning value (-127 [left] to 127 [right])
  */
  void core_setPan(CoreToken core, int bal);  // -127..127

  // register here for general callbacks in core status.
  /**
    Register a callback with the core to
    receive core status callbacks.

    @see delCallback()
    @param core Core token of the core.
    @param cb Core Callback to register.
  */
  void core_addCallback(CoreToken core, CoreCallback *cb);

  /**
    Unregister a callback with the core.

    @see addCallback()
    @param core Core token of the core.
    @param cb Core Callback to unregister.
  */
  void core_delCallback(CoreToken core, CoreCallback *cb);

  // get visualization data, returns 0 if you should blank out
  /**
    Get visualization data for the currently
    playing item.

    We suggest using a struct like this to read the vis
    data:

    typedef struct {
      enum {
        LEFT = 0,
        RIGHT = 1
      };
    unsigned char spectrumData[2][576];
    char waveformData[2][576];
    } VisData;

    A call using this struct would like so:

      getVisData(&myVisData, sizeof(VisData));

    @ret 0, If there is no VIS data; > 0, VIS data available;
    @param core Core token of the core.
    @param dataptr Buffer to receive VIS data.
    @param sizedataptr Size of the buffer.
  */
  int core_getVisData(CoreToken core, void *dataptr, int sizedataptr);

  /**
    Get the value of the left VU meter.

    @see core_getRightVuMeter()
    @ret Value of the left VU meter (0 to 255).
    @param core Core token of the core.
  */
  int core_getLeftVuMeter(CoreToken core);

  /**
    Get the value of the left VU meter.

    @see core_getLeftVuMeter()
    @ret Value of the right VU meter (0 to 255).
    @param core Core token of the core.
  */
  int core_getRightVuMeter(CoreToken core);

  /**
    Register an item sequencer with the core.
    The item sequencer feeds the playstrings
    of the next item to be played to the core.

    @see core_deregisterSequencer()
    @ret 1, Success; 0, Failure;
    @param core Core token of the core.
    @param seq Sequencer to register.
  */
  int core_registerSequencer(CoreToken core, ItemSequencer *seq);

  /**
    Unregister a sequencer with the core.

    @see registerSequencer()
    @ret 1, Success; 0, Failure;
    @param core Core token of the core.
    @param seq Sequencer to unregister.
  */
  int core_deregisterSequencer(CoreToken core, ItemSequencer *seq);

  void core_userButton(CoreToken core, int button);  // see buttons.h

  /**
    Get the EQ status.

    @see setEqStatus()
    @see getEqPreamp()
    @see setEqPreamp()
    @see getEqBand()
    @see setEqBand()
    @ret 1, On; 0, Off;
    @param core Core token of the core.
  */
  int core_getEqStatus(CoreToken core); // returns 1 if on, 0 if off

  /**
    Set the EQ state.

    @see getEqStatus()
    @see getEqPreamp()
    @see setEqPreamp()
    @see getEqBand()
    @see setEqBand()
    @param core Core token of the core.
    @param enable 1, On; 0, Off;
  */
  void core_setEqStatus(CoreToken core, int enable);  // 1=on, 0=off

  /**
    Get the pre-amp value of the EQ.

    @see setEqStatus()
    @see getEqStatus()
    @see setEqPreamp()
    @see getEqBand()
    @see setEqBand()
    @ret Pre-amp value (-127 [-20dB] to 127 [+20dB]).
    @param core Core token of the core.
  */
  int core_getEqPreamp(CoreToken core);            // -127 to 127 (-20db to +20db)

  /**
    Set the pre-amp value of the EQ.

    @see setEqStatus()
    @see getEqStatus()
    @see getEqPreamp()
    @see getEqBand()
    @see setEqBand()
    @param core Core token of the core.
    @param pre Pre-amp value (-127 [-20dB] to 127 [+20dB]).
  */
  void core_setEqPreamp(CoreToken core, int pre);  // -127 to 127 (-20db to +20db)

  /**
    Get the value of an EQ band. There
    are 10 bands available.

    Here is the list:

    0 - 60 Hz    1 - 170 Hz
    2 - 310 Hz   3 - 600 Hz
    4 - 1 kHz    5 - 3 kHz
    6 - 6 kHz    7 - 12 kHz
    8 - 14 kHz   9 - 16 kHz

    @see setEqStatus()
    @see getEqStatus()
    @see getEqPreamp()
    @see setEqBand()
    @ret EQ band value (-127 [-20dB] to 127 [+20dB]).
    @param core Core token of the core.
    @param band EQ band to read (0 to 9).
  */
  int core_getEqBand(CoreToken core, int band);            // band=0-9

  /**
    Set the value of an EQ band. There
    are 10 bands available.

    Here is the list:

    0 - 60 Hz    1 - 170 Hz
    2 - 310 Hz   3 - 600 Hz
    4 - 1 kHz    5 - 3 kHz
    6 - 6 kHz    7 - 12 kHz
    8 - 14 kHz   9 - 16 kHz

    @see setEqStatus()
    @see getEqStatus()
    @see getEqPreamp()
    @see setEqBand()
    @param core Core token of the core.
    @param band EQ band to set (0 to 9)
    @param val  EQ band value (-127 [-20dB] to 127 [+20dB]).
  */
  void core_setEqBand(CoreToken core, int band, int val);  // band=0-9,val=-127 to 127 (-20db to +20db)

  /**
    Get the automatic EQ preset loading state.

    @ret 1, On; 0, Off;
    @param core Core token of the core.
  */
  int core_getEqAuto(CoreToken core);     // returns 1 if on, 0 if off

  /**
    Set the automatic EQ preset loading.

    @param core Core token of the core.
    @param enable 1, On; 0, Off;
  */
  void core_setEqAuto(CoreToken core, int enable); // 1=on, 0=off

  void core_setCustomMsg(CoreToken core, const char *text);

  /**
    Register an extension with the core.

    @see core_getSupportedExtensions()
    @see core_getExtSupportedExtensions()
    @see core_unregisterExtension()
    @see core_getExtensionFamily()
    @param extensions Extension to register.
    @param extension_name Name of the extension.
  */
  void core_registerExtension(const char *extensions, const char *extension_name, const char *family=NULL);

  /**
    Get the family name to which the extension
    is associated with (Families are "Audio", "Video", etc.)

    @see core_getSupportedExtensions()
    @see core_getExtSupportedExtensions()
    @see core_registerExtension()
    @see core_unregisterExtension()
    @ret Family name of the extension.
    @param extension Extension to get family name of.
  */
  const char *core_getExtensionFamily(const char *extension);

  /**
    Unregister an extension with the core.

    @see core_getSupportedExtensions()
    @see core_getExtSupportedExtensions()
    @see core_registerExtension()
    @see core_getExtensionFamily()
    @param extensions Extension to unregister.
  */
  void core_unregisterExtension(const char *extensions);

  const char *core_getTitle(CoreToken core);
// services
  /**
    Register a service. Service registration is done
    on creation of the WAC.

    @see WAComponentClient
    @ret 1, success; 0, failure.
    @param p Pointer to your service factory.
  */
  int service_register(waServiceFactory *);

  /**
    Deregister a service. Service deregistration is done
    on destruction of the WAC.

    @see WAComponentClient
    @ret 1, success; 0, failure.
    @param p Pointer to your service factory.
  */
  int service_deregister(waServiceFactory *);

  /**
    Get the number of services registered for a specific
    service type. This should only be called after
    Wasabi is fully started (after WAC creation).

    @see FOURCC
    @ret Number of services present in that service type.
    @param svc_type Service type.
  */
  int service_getNumServices(FOURCC svc_type);  // see common/svc_enum.h

  // enumerate by family
  /**
    Enumerate services by family. This should only
    be called after Wasabi is fully started (after WAC creation).

    @see FOURCC
    @ret Requested service.
    @param svc_type Service type.
    @param n        Number of the service.
  */
  waServiceFactory *service_enumService(FOURCC svc_type, int n);

  // fetch by GUID
  /**
    Get a service by it's GUID. This should only
    be called after Wasabi is fully started (after WAC creation).

    @ret Requested service.
    @param guid Service GUID.
  */
  waServiceFactory *service_getServiceByGuid(GUID guid);

  // service owner calls this when it issues a service *
  /**
    Lock a service. Service owner must call this when
    it issues a new service pointer to a client.

    @ret 1, success; 0, failure;
    @param owner Service owner.
    @param svcptr Service pointer returned to client.
  */
  int service_lock(waServiceFactory *owner, void *svcptr);
  int service_isvalid(FOURCC svctype, waServiceFactory *service);

  int service_unlock(void *svcptr);

  // service client calls this when it uses a service *
  /**
    ClientLock a service. Service client must call
    this before using the service.

    @ret 1, success; 0, failure;
    @param svcptr Service pointer.
  */
  int service_clientLock(void *svcptr);

  // service client calls this when done w/ service *
  /**
    Release a service. Service client must call this
    when he's finished using the service. If the service
    is NOT released it will cause improper shutdown of
    the service.

    @ret 1, success; 0, failure;
  */
  int service_release(void *svcptr);

  /**
    Get the pretty printed type name of a service type based
    on it's FOURCC.

    @see FOURCC
    @ret Service name (readable).
    @param svc_type Service type.
  */
  const char *service_getTypeName(FOURCC svc_type);

  /**
    Get the owning component of a service from
    a service pointer.

    @ret GUID of the owning component.
    @param svcptr Service pointer.
  */
  GUID service_getOwningComponent(void *svcptr);

  /**
    Get the locking component for a service from
    a service pointer.

    @ret GUID of the locking component.
    @param svcptr Service pointer.
  */
  GUID service_getLockingComponent(void *svcptr);

// sys callbacks
  /**
    Register a callback with the system (Wasabi).
    Please note that all callbacks are register thru
    this method.

    @see SysCallback
    @see RunlevelCallback
    @see CoreCallback
    @see SkinCallback
    @ret 1, success; 0, failure;
    @param cb Callback to register.
    @param param ?
  */
  int syscb_registerCallback(SysCallback *cb, void *param=NULL);

  /**
    Deregister a callback with the system (Wasabi).
    Please note that all callbacks are deregistered thru
    this method.

    @see SysCallback
    @see RunlevelCallback
    @see CoreCallback
    @see SkinCallback
    @ret 1, success; 0, failure;
    @param cb Callback to deregister.
  */
  int syscb_deregisterCallback(SysCallback *cb);
  int syscb_issueCallback(int eventtype, int msg, int p1=0, int p2=0);

// component commands
  /**
    Send a generic command to a specific component
    using it's GUID.

    @ret 1, success; 0, failure;
    @param guid     GUID of the component that needs to receive the command.
    @param command  Command string.
    @param param1   Generic parameter 1.
    @param param2   Generic paramater 2.
    @param ptr      Generic data ptr.
    @param ptrlen   Length of the data from ptr.
  */
  int cmd_sendCommand(GUID guid, char *command, int param1=0, int param2=0, void *ptr=NULL, int ptrlen=0);

  // this sends a command through the main wasabi thread
  /**
    Posts a generic command to a specific component
    using it's GUID. When this method is used the
    message will pass thru the main Wasabi thread.

    You can optionally wait for an answer from the component
    which will cause your component to BLOCK!

    @ret 1, success; 0, failure;
    @param guid           GUID of the component that needs to receive the command.
    @param command        Command string.
    @param param1         Generic parameter 1.
    @param param2         Generic paramater 2.
    @param ptr            Generic data ptr.
    @param ptrlen         Length of the data from ptr.
    @param waitforanswer  0, Do not wait for an answer; 1, Wait for an answer;
  */
  int cmd_postCommand(GUID guid, char *command, int param1=0, int param2=0, void *ptr=NULL, int ptrlen=0, int waitforanswer=0);

  /**
    Broadcasts a generic command to all components thru
    the main Wasabi thread.

    @ret 1, success; 0, failure.
    @param command Command string.
    @param param1         Generic parameter 1.
    @param param2         Generic paramater 2.
    @param ptr            Generic data ptr.
    @param ptrlen         Length of the data from/to ptr.
  */
  void cmd_broadcastCommand(char *command, int param1=0, int param2=0, void *ptr=NULL, int ptrlen=0);

  /**
    Sets the system tray icon tooltip for the application.

    @param text Tooltip to set.
  */
  void status_setTrayTipText(const char *text);

  /**
    Enables or Disables all user input to all windows.
    This includes child windows.

    @param enable 0, Disable; 1, Enable;
  */
  void main_enableInput(int enable);

  // language/locality support
  /**
    Get the translation of a string from the currently
    used locale.

    For example, if a french locale is used the string
    "Stop" could translate to the string "Arrêt".

    If no translation is found, the input string
    is returned.

    @ret Translated string.
    @param str String to translate.
  */
  const char *locales_getTranslation(const char *str); // if not found, returns the str paramer

  /**
    Add a string translation to the currently used
    locale.

    The changes made using this method are NOT saved
    on exit.

    @param from Input translation.
    @param to Output translation.
  */
  void locales_addTranslation(const char *from, const char *to);

  /**

  */
  const char *locales_getBindFromAction(int action);

  /**
    Get the number of string translations
    present in the currently used locale.

    @ret Number of string translations.
  */
  int locales_getNumEntries();

  /**
    Enumerate the string translation
    present in the currently used locale.

    @see locales_getNumEntries()
    @ret String translation.
    @param n Index number of the string translation.
  */
  const char *locales_enumEntry(int n);

  /**
    Register a new keyboard accelerators section for
    the currently active locale. The newly created section
    can then be filled with accelerator key definitions.

    The section is also defined only for the RootWnd specified
    unless the global flag is set to 1. If the global flag is
    set to 1, the main root window pointer is what should be
    used.

    @see main_getRootWnd()
    @param name Name of the accelerator section.
    @param wnd  Window for which the accelerator section applies.
    @param global 0, local section; 1, global section;
  */
  void locales_registerAcceleratorSection(const char *name, RootWnd *wnd, int global=0);

// configuration
  /**
    Register a configuration item.

    @param cfgitem Configuration item to register.
  */
  void config_registerCfgItem(CfgItem *cfgitem);

  /**
    Unregister a configuration item.

    @param Configuration item to unregister.
  */
  void config_deregisterCfgItem(CfgItem *cfgitem);

  /**
    Get the number of registered configuration
    items.
  */
  int config_getNumCfgItems();

  /**
    Enumerate the registered configuration
    items. Index starts at 0.

    @ret Requested configuration item or NULL if not available.
    @param n Index number of the configuration item.
  */
  CfgItem *config_enumCfgItem(int n);

  /**
    Get a configuration item using it's
    GUID. The GUID of the component
    that registered the configuration item must
    be used to find the item associated with it.

    @ret Requested configuration item or NULL if not available.
  */
  CfgItem *config_getCfgItemByGuid(GUID g);

// message box
  /**
    Pop-open a skinned message box.

    The message box's style can be customized using
    the flags parameter. Style flags can be combined
    together using the OR bit operator. Here is the
    list of flags available.

    MSGBOX_OK         Adds an OK button.      (1)
    MSGBOX_CANCEL     Adds a CANCEL button.   (2)
    MSGBOX_YES        Adds a YES button.      (4)
    MSGBOX_NO         Adds a NO button.       (8)
    MSGBOX_ALL        Adds ALL buttons.       (16)
    MSGBOX_NEXT       Adds a NEXT button.     (32)
    MSGBOX_PREVIOUS   Adds a PREVIOUS button. (64)

    Please note that MSGBOX_ABORTED (0) is NOT a flag
    but only a return code.

    The automatic response identifier, if set, will
    cause the automatic return of the message box if
    the user has previously answered the question.

    @param txt    Text in the message box.
    @param title  Message box title.
    @param flags  Message box style flags.
    @param not_anymore_identifier Automatic response identifier.
    @param parenwnt Parent window.
  */
  int messageBox(const char *txt, const char *title, int flags, const char *not_anymore_identifier, RootWnd *parenwnt);

  /**
    Get the modal window on top of the
    modal window stack.

    @ret Window that's currently modal.
  */
  RootWnd *getModalWnd();

  /**
    Push a window onto the modal stack. If
    the window is on top of the stack, it's
    currently modal.

    @param w Window to push onto the modal stack.
  */
  void pushModalWnd(RootWnd *w=MODALWND_NOWND);

  /**
    Pop a window from the modal stack. If
    the window is on top of the stack, it
    will no longer be modal after this call.

    @param w Window to pop from the modal stack.
  */
  void popModalWnd(RootWnd *w=MODALWND_NOWND);

// size metrics
  void metrics_addCallback(MetricsCallback *mcb);
  void metrics_delCallback(MetricsCallback *mcb);
  int metrics_getDelta();
  int metrics_setDelta(int newdelta);

  /**
    Set the Winamp Browser Component URL.
    It will then automatically load the html
    content into itself.

    @param url URL to go to.
  */
  void main_navigateUrl(const char *url);

// database stuff
  /**
    Aus has no clue what this does.

    @ret 1, Success; 0, Failure;
    @param playstring Playstring to optimize.
  */
  int metadb_optimizePlayString(char *playstring);

  /**
    Add a new entry to the meta db.

    @ret 1, success; 0, failure;
    @param playstring Playstring to add.
    @param initial_name Initial name of the item (before media info reading).
  */
  int metadb_addNewItem(const char *playstring, const char *initial_name=NULL);

  /**
    Delete an entry from the meta db.

    @ret 1, success; 0, failure;
    @param playstring Playstring
  */
  int metadb_deleteItem(const char *playstring);

  /**
    Force writing of all changes to the metadb.
  */
  void metadb_sync();

  // called by class Playstring
  /**
    Adds a playstring to the playstring table.

    @ret Added playstring.
    @param playstring Playstring to add.
    @param tablenum
  */
  const char *metadb_dupPlaystring(const char *playstring, int tablenum=0);

  /**
    Removes a playstring from the playstring table.
  */
  void metadb_releasePlaystring(const char *playstring, int tablenum=0);

// metadata assistance from metaDB
  /**
    Get the media information for a specific playstring.

    This will return a formatted string containing all
    media specific information (bitrate, stereo/mono,
    sampling rate, for both audio and video)

    @param playstring Playstring to read.
    @param info Buffer for the media information.
    @param maxlen Maximum length of the buffer.
  */
  void metadb_getInfo(const char *playstring, char *info, int maxlen);

  /**
    Get the length (in milliseconds) of a piece
    of media using it's playstring.

    @ret Length (in milliseconds).
    @param playstring Playstring.
  */
  int metadb_getLength(const char *playstring);

  /**
    Get specific metadata linked to a playstring.
    You can read all fields using this method.

    @ret Length of the data.
    @param playstring Playstring to read.
    @param name       Field name.
    @param data       Data buffer.
    @param data_len   Maximum capicity of the data buffer.
    @param data_type  Type of the data you wish to read.
  */
  int metadb_getMetaData(const char *playstring, const char *name,
    char *data, int data_len, int data_type=0); // returns size of data

  /**
    Get the data type for a meta db field, using
    the field name.

    For a list of fields and data types, please
    look in common/metatags.h.

    @see metatags.h
    @ret Data type.
    @param name Field name.
  */
  int metadb_getMetaDataType(const char *name);

  /**
    Get the GUID of the table in which resides
    a specific field, using the field name.

    @see metatags.h
    @ret GUID of the table.
    @param name Field name.
  */
  GUID metadb_getMetaDataTable(const char *name);

  /**
    Set meta data for a playstring. Various fields
    can be set. For a list of fields, see common/metatags.h.

    @see metatags.h
    @ret Length of the data written.
    @param playstring Playstring to write to.
    @param name       Field name.
    @param data       Data buffer.
    @param data_len   Size of the data you wish to write.
    @param data_type  Type of the data you wish to write.
  */
  int metadb_setMetaData(const char *playstring, const char *name,
    const char *data, int data_len, int data_type);

  // attempts to render data (from db or whatever) in given rect
  // if you use this you can show new datatypes your code doesn't understand
  /**
    Attempts to render the data in a given RECT. It's possible to show
    datatypes that are unknown to your code using this.

    @ret
    @param cb Canvas to draw on.
    @param r  RECT to use.
    @param data Data to render.
    @param datatype Type of the data to render.
    @param centered TRUE, center data in canvas; FALSE, do not center data in canvas;
  */
  int metadb_renderData(CanvasBase *cb, RECT &r, void *data, int datatype, int centered=FALSE);

  // attempts to convert a datatype to a text representation
  /**
    Attempts to convert data to a text representation, based
    on it's datatype.

    @ret Length of the converted data.
    @param data     Data to convert.
    @param dataype  Type of the data to convert.
    @param buf      Result buffer.
    @param buflen   Result buffer maximum size.
  */
  int metadb_convertToText(void *data, int datatype, char *buf, int buflen);

  // delete metadata for this service and playitem
  /**
  */
  void metadb_deleteMetaData(GUID metadataservice, const char *playitem);

  // creates a scanner on a metadata service's table. not guaranteed to succeed (can be denied)
  /**
    Create a new database scanner on a specific
    table (by using the tables GUID).

    @ret Database scanner or NULL if scanner creation failed.
    @param metadataservice Meta DB table to attach to the scanner to.
  */
  dbSvcScanner *metadb_newScanner(GUID metadataservice);

  // deletes a scanner previously created by metadb_newScanner
  /**
    Delete a database scanner. This is for scanners
    created with metadb_newScanner().

    @see dbSvcScanner
    @see metadb_newScanner
    @see GUID
    @param scanner Scanner to delete.
  */
  void metadb_deleteScanner(dbSvcScanner *scanner);

#if 1//REIMPLEMENT
// completeness graph
  void startCompleted(int max);
  void incCompleted();
  void endCompleted();

#endif
// skinning
  COLORREF skin_getColorElement(char *type, const char **color_grp=NULL);
  COLORREF *skin_getColorElementRef(char *type, const char **color_grp=NULL);
  int *skin_getIterator();  // an int that cycles on skin reloading
  void setSkinPathOverride(const char *path); // UNSUPPORTED IN MONOLITHIC API
  void skin_setLockUI(int l);
  int skin_getLockUI();
  double skin_getVersion();
  COLORREF skin_getBitmapColor(const char *bitmapid);

#define RenderBaseTexture renderBaseTexture //CUT

  void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, const RECT &r, RootWnd *destWnd, int alpha=255);
  void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL);
  void skin_unregisterBaseTextureWindow(RootWnd *window);

  /**
    Switch to a specific skin using the skin
    name.

    @param skin_name Name of the skin to load.
  */
  void skin_switchSkin(char *skin_name);

  /**
    Get the name of the currently loaded
    skin.

    @ret Name of the currently loaded skin.
  */
  const char *getSkinName();

  /**
    Get the path of the currently loaded
    skin.

    @ret Path of the currently loaded skin.
  */
  const char *getSkinPath();

  /**
    Get the path containing all skins subdirectories

    @ret Skins root path
  */
  const char *getSkinsPath();


  /**
    Get the path to the default application
    skin.

    @ret Path to the default application skin.
  */
  const char *getDefaultSkinPath();

// generic malloc/free. Use these if you are returning data that other
// components will need to free (possibly after you have been unloaded)
  /**
    Allocate a block of memory specified in bytes from the system heap.

    NOTE: Please DO NOT just use this instead of malloc(). Only use it
    to allocate RAM that another component will free with sysFree().

    @see sysFree()
    @see sysRealloc()
    @see SysMemCallbackI
    @ret Pointer to the allocated memory block or NULL on failure.
    @param size Requested size of the memory block (in bytes).
  */
  void *sysMalloc(int size);

  /**
    Free a block of memory previously allocated from the system heap.

    NOTE: Please DO NOT just use this instead of free(). Only call it with
    memory allocated via sysMalloc().

    @see sysMalloc()
    @see sysRealloc()
    @see SysMemCallbackI
    @param ptr Pointer to the memory block to free.
  */
  void sysFree(void *ptr);

  /**
    Allocate supplementary memory to a previously allocated
    memory block. This will take care of copying the data to
    the newly allocated memory block.

    @see SysMemCallbackI
    @ret Pointer to the reallocated memory block or NULL on failure.
    @param ptr      Pointer to the memory block to resize.
    @param newsize  New size of the memory block (in bytes).
  */
  void *sysRealloc(void *ptr, int newsize);

  /**
    Notify the system that a sysMem block has changed. This will cause
    a SysMemCallback to be sent.
    @see SysMemCallbackI
    @see sysRealloc()
    @param ptr The pointer to the memory block whose contents has changed.
  */
  void sysMemChanged(void *ptr);

// image loading
  ARGB32 *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h);
  ARGB32 *imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL);
  ARGB32 *imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached);
  RegionServer *imgldr_requestSkinRegion(const char *id);
  void imgldr_cacheSkinRegion(const char *id, Region *r);
  void imgldr_releaseSkinBitmap(ARGB32 *bmpbits);

  COLORREF filterSkinColor(COLORREF, const char *elementid, const char *groupname);
  void reapplySkinFilters();

  /**
    Get the number of color themes available for
    the currently loaded skin.

    @ret Number of color themes.
  */
  int colortheme_getNumColorSets();

  /**
    Enumerate the names of the color themes
    available for the currently loaded skin.
    This is done using the color themes index
    number, the index start at 0.

    @ret Name of the color theme.
    @param n Index number of the color theme.
  */
  const char *colortheme_enumColorSet(int n);

  /**
    Get the number of color groups present in
    the color theme.

    @ret Number of color groups.
    @param colorset Name of the color theme.
  */
  int colortheme_getNumColorGroups(const char *colorset);

  /**
    Enumerate the names of the color groups
    present in the color theme. This is done using
    the color groups index number, the index starts
    at 0.

    @ret Name of the color group.
    @param colorset Name of the color theme.
    @param n Index number of the color group.
  */
  const char *colortheme_enumColorGroupName(const char *colorset, int n);

  /**
    Enumerate the color theme groups.

    @ret A ColorThemeGroup object.
    @param colorset the index of the color set.
    @param colorgroup the index of the clor group.
  */
  ColorThemeGroup *colortheme_enumColorGroup(int colorset, int colorgroup);
  ColorThemeGroup *colortheme_getColorGroup(const char *colorset, const char *colorgroup);

  /**
    Apply a color theme to the currently loaded
    skin.

    @param colorset Name of the color theme to use.
  */
  void colortheme_setColorSet(const char *colorset);

  /**
    Get the name of the currently applied
    color set.

    @ret Name of the currently applied color set.
  */
  const char *colortheme_getColorSet();
  void colortheme_newColorSet(const char *set);
  void colortheme_updateColorSet(const char *set);

  int paintset_present(int set);
  void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255);
  void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255);

// animated rects
  void drawAnimatedRects(const RECT *r1, const RECT *r2);

// Dde com
  /**
    Send a DDE (Dynamic Data Exchange) message to another
    application. This is Win32 specific and NOT PORTABLE.

    @param application  Name of the application.
    @param command      Command to send.
    @param minimumDelay Minimum delay between commands.
  */
  void sendDdeCommand(const char *application, const char *command, DWORD minimumDelay);

// XML Reader
  /**
    Register a callback with the main XML reader.

    It is also possible to match on a specific root tag.
    When the root tag is found, the parser will begin returning
    callbacks on what is found inside the root tag.

    The match pattern is a regular expression. There is one
    exception. The "/" character is used to indicate XML hierarchy.
    This means that if you use the match pattern "WinampXML/*", it
    will return all the elements found inside the WinampXML tag.

    For example, if you want to look for WinampXML the string
    you would use is "WinampXML/*".

    @param matchstr Match pattern to use for root tag.
    @param callback Pointer to the XML callback instance.
  */
  void xmlreader_registerCallback(const char *matchstr, XmlReaderCallback *callback);

  /**
    Unregister a callback with the main XML reader.

    @param callback Pointer to the XML callback instance.
  */
  void xmlreader_unregisterCallback(XmlReaderCallback *callback);

  /**
    Parse an XML file using the main XML parser.

    It is possible to specify a specific XML callback object
    when loading the file. This will cause only that callback
    object to receive callbacks while parsing the file.

    @param filename XML File to parse.
    @param only_this_object Pointer to the XML callback object to receive callbacks; NULL, Callbacks to all registered XML callback objects;
  */
  void xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object=NULL);


  /**
    Parse an XML file using specified chunk size

    @param filename XML file to parse
    @param only_this_object Pointer to the XML callback object to receive callbacks; NULL, Callbacks to all registered XML callback objects;
    @ret handle to use in xmlreader_loadChunk
  */
  int xmlreader_startXmlLoad(const char *filename, XmlReaderCallback *only_this_object=NULL);

  /**
    Load chunk from the handle specified from xmlreader_startXmlLoad

    @param handle Handle received from xmlreader_startXmlLoad
    @param chunksize Size of chunk to read from file
    @ret 1 if ok, 0 if done
  */
  int xmlreader_loadChunk(int handle, int chunksize);

  /**
    Close the handle opened from xmlreader_startXmlLoad

    @param handle Handle received from xmlreader_startXmlLoad
  */
  void xmlreader_close(int handle);


// Window Tracker
  void wndTrackAdd(RootWnd *wnd);
  void wndTrackRemove(RootWnd *wnd);
  BOOL wndTrackDock(RootWnd *wnd, RECT *r, int mask);
  BOOL wndTrackDock(RootWnd *wnd, RECT *r, RECT *orig_r, int mask);
  void wndTrackStartCooperative(RootWnd *wnd);
  void wndTrackEndCooperative(void);
  int wndTrackWasCooperative(void);
  void wndTrackInvalidateAll(void);

// Components
  /**
    Get the number of loaded components.

    @ret Number of loaded components.
  */
  int getNumComponents();

  /**
    Get the GUID of a component based on it's
    loading index number.

    The loading index number is determined simply
    by the order in which the components were loaded.
    Index starts at 0.

    @see GUID
    @ret GUID of the requested component or 0 if the index is out of bounds.
    @param c Index number of the component.
  */
  GUID getComponentGUID(int c);

  /**
    Get the name of the component using it's
    GUID.

    @see GUID
    @ret Name of the component.
    @param componentGuid GUID of the component.
  */
  const char *getComponentName(GUID componentGuid);

  /**
    Get the configuration item registered
    by the component using the component's
    GUID.

    @see GUID
    @ret Configuration item registered by the component.
    @param componentGuid GUID of the component.
  */
  CfgItem *compon_getCfgInterface(GUID componentGuid);

  /**
  */
  int comp_notifyScripts(const char *s, int i1, int i2);

  /**
  */
  int autopopup_registerGuid(GUID g, const char *desc, const char *prefered_container=NULL, int container_flag=0);

  /**
  */
  int autopopup_registerGroupId(const char *groupid, const char *desc, const char *prefered_container=NULL, int container_flag=0);

  /**
  */
  void autopopup_unregister(int id);

  int skinwnd_toggleByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  int skinwnd_toggleByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  RootWnd *skinwnd_createByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  RootWnd *skinwnd_createByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  void skinwnd_destroy(RootWnd *w, RECT *destanimrect=NULL);
  int skinwnd_getNumByGuid(GUID g);
  RootWnd *skinwnd_enumByGuid(GUID g, int n);
  int skinwnd_getNumByGroupId(const char *groupid);
  RootWnd *skinwnd_enumByGroupId(const char *groupid, int n);
  void skinwnd_attachToSkin(RootWnd *w, int side, int size);

  ScriptObject *skin_getContainer(const char *container_name);
  ScriptObject *skin_getLayout(ScriptObject *container, const char *layout_name);

  void wndholder_register(WindowHolder *wh);
  void wndholder_unregister(WindowHolder *wh);

  // allows a rootWndFromPoint that won't get other instances RootWnds
  // also, this will find VirtualWnds while the WIN32 version won't
  // and... this is portable :)
  RootWnd *rootWndFromPoint(POINT *pt);
  void registerRootWnd(RootWnd *wnd);
  void unregisterRootWnd(RootWnd *wnd);
  int rootwndIsValid(RootWnd *wnd);
  int forwardOnMouseWheel(int l, int a);

  //keyboard support

  void hookKeyboard(RootWnd *hooker);
  void unhookKeyboard(RootWnd *hooker);
  int interceptOnChar(unsigned int c);
  int interceptOnKeyDown(int k);
  int interceptOnKeyUp(int k);
  int interceptOnSysKeyDown(int k, int kd);
  int interceptOnSysKeyUp(int k, int kd);
  int pushKeyboardLock();
  int popKeyboardLock();
  int isKeyboardLocked();
  void kbdReset();

  int forwardOnChar(RootWnd *from, unsigned int c, int kd);
  int forwardOnKeyDown(RootWnd *from, int k, int kd);
  int forwardOnKeyUp(RootWnd *from, int k, int kd);
  int forwardOnSysKeyDown(RootWnd *from, int k, int kd);
  int forwardOnSysKeyUp(RootWnd *from, int k, int kd);
  int forwardOnKillFocus();


  // file readers support
  /**
    Open a file using specified mode flags.

     Flags include read (r), write (w),
     append (a) as well as optional flags
     text (t) for opening text files, binary (b)
     for opening data files and plus (+) for opening
     in read and write modes.

     Append differs from the other modes as it sets
     the filehandle at the end of the file.

     For example, to open a file in read and write
     binary mode, you would use this string: "w+b".

     @see FOPEN()
     @ret Filehandle.
     @param filename  File to open.
     @param mode      Mode used to open the file ("a+b" for example).
  */
  void *fileOpen(const char *filename, const char *mode);

  /**
    Close a file using.

    @see FCLOSE()
    @param fileHandle Filehandle of the file to close.
  */
  void fileClose(void *fileHandle);

  /**
    Read from a previously open file.

    @see FREAD()
    @see FGETS()
    @ret Number of bytes read.
    @param buffer Buffer for data read.
    @param size   Number of bytes to read.
    @param fileHandle Filehandle of the file to read from.
  */
  int fileRead(void *buffer, int size, void *fileHandle);

  /**
    Write to a previously open file.

    @see FWRITE()
    @see FPRINTF()
    @ret Number of bytes written.
    @param buffer Buffer containing data to be written.
    @param size   Number of bytes to be written.
    @param fileHandle Filehandle of the file to be written to.
  */
  int fileWrite(const void *buffer, int size, void *fileHandle);

  /**
    Seek in a previously open file.

    @see FSEEK()
    @ret Position in the file (byte position, start of file is 0).
    @param offset Offset to go to (in bytes).
    @param origin Start point in file (byte position, start of file is 0).
    @param fileHandle Filehandle of the file to seek in.
  */
  int fileSeek(int offset, int origin, void *fileHandle);

  /**
    Get the position of the data pointer in
    a previously open file.

    @see FTELL()
    @ret Position of the data pointer in the file (byte position, start of file is 0).
    @param fileHandle Filehandle of the file.
  */
  int fileTell(void *fileHandle);

  /**
    Get the size of a previously open file.

    @see FGETSIZE()
    @ret Size of the file in bytes.
    @param fileHandle Filehandle of the file.
  */
  int fileGetFileSize(void *fileHandle);

  /**
    Verify if a file exists using it's
    full path.

    @see FEXISTS()
    @ret 1, File exists; 0, File not found; -1, Unknown;
    @param filename Full path of the file.
  */
  int fileExists(const char *filename);

  /**
    Delete a file using it's full path.

    @see UNLINK()
    @see FDELETE()
    @ret 1, Success; 0, Failure;
    @param Full path of the file.
  */
  int fileRemove(const char *filename);

  int fileRemoveUndoable(const char *filename);

  // can't move directories between volumes on win32 :P
  /**
    Move a file (or directory) using it's full origin and
    destination path.

    Please note that under Win32, it's impossible
    to move directories between volumes using this
    method.

    @ret 1, Success; 0, Failure;
    @param filename     Full origin path of the file (or directory).
    @param destfilename Full destination path of the file (or directory).
  */
  int fileMove(const char *filename, const char *destfilename);

// text output functions. don't call these directly, use the Canvas stuff
  void font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt);
  int font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h);

// call this after deallocating a lot of RAM
  void hint_garbageCollect();

// outputs a message to the console, severity 0 to 9 (critical, before terminating operations)
  /**
    Output a message to the console. It's possible
    to specify the level of severity of the message.

    @ret
    @param severity Severity of the message (0 to 9, 9 being critical).
    @param string   Message to output.
  */
  int console_outputString(int severity, const char *string);

// loads a skin file, but does not switch main skin.
// use this to load a custom xml skin/script/definition file or package (using includes)
// either before (if you want to allow the content to be overriden) or after
// skin loading
// return value is what you need to give to unloadSkinPart() to clean up your mess
  int loadSkinFile(const char *xmlfile);
  void unloadSkinPart(int skinpart);

  void maki_pushObject(void *o);
  void maki_pushInt(int v);
  void maki_pushBoolean(int b);
  void maki_pushFloat(float f);
  void maki_pushDouble(double d);
  void maki_pushString(const char *s);
  void maki_pushVoid();
  void maki_pushAny(scriptVar v);
  void *maki_popObject();
  int maki_popInt();
  int maki_popBoolean();
  float maki_popFloat();
  double maki_popDouble();
  const char *maki_popString();
  scriptVar maki_popAny();
  void maki_popDiscard();
  const char *maki_getFunction(int dlfid, int *n, ScriptObjectController **p);
  int maki_addDlfRef(ScriptObjectController *o, const char *function_name, void *host);
  void maki_addDlfClassRef(ScriptObjectController *o, void *host);
  void maki_remDlfRef(void *host);
  scriptVar maki_callFunction(ScriptObject *o, int dlfid, scriptVar **params);
  scriptVar maki_triggerEvent(ScriptObject *o, int dlfid, int np, int scriptid=-1);
  int maki_getScriptInt(scriptVar v);
  int maki_getScriptBoolean(scriptVar v);
  float maki_getScriptFloat(scriptVar v);
  double maki_getScriptDouble(scriptVar v);
  const char *maki_getScriptString(scriptVar v);
  ScriptObject *maki_getScriptObject(scriptVar v);
  scriptVar maki_updateDlf(maki_cmd *cmd, int *dlfid, int *linkcount);
  ScriptObject *maki_instantiate(GUID classguid);
  void maki_destroy(ScriptObject *o);
  void *maki_encapsulate(GUID classguid, ScriptObject *o);
  void maki_deencapsulate(GUID classguid, void *o);
  ScriptObjectController *maki_getController(GUID scriptclass);
  int maki_createOrphan(int type);
  void maki_killOrphan(int id);
  void maki_setObjectAtom(const char *atomname, ScriptObject *object);
  ScriptObject *maki_getObjectAtom(const char *atomname);
  ScriptObject *maki_findObject(const char *name);
  void vcpu_addScriptObject(ScriptObject *o);
  void vcpu_removeScriptObject(ScriptObject *o);
  int vcpu_getCacheCount();
  int vcpu_isValidScriptId(int id);
  int vcpu_mapVarId(int varid, int scriptid);
  int vcpu_getUserAncestorId(int varid, int scriptid);
  int vcpu_getNumEvents();
  int vcpu_getEvent(int event, int *dlf, int *script, int *var);
  int vcpu_getComplete();
  void vcpu_setComplete();
  void vcpu_resetComplete();
  const char *vcpu_getClassName(int vcpuid, int localclassid);

// groups creation / destruction

  RootWnd *group_create(const char *groupid, int scripts_enabled=1);
  RootWnd *group_createBySkinItem(SkinItem *item, int scripts_enabled=1);
  RootWnd *group_create_layout(const char *groupid, int scripts_enabled=1);
  RootWnd *group_create_cfg(const char *groupid, CfgItem *cfgitem, const char *attributename, int scripts_enabled=1);
  int group_exists(const char *id);
  RootWnd *retired_group_create(const char *groupid);
  RootWnd *retired_group_create_layout(const char *groupid);
  RootWnd *retired_group_create_cfg(const char *groupid, CfgItem *cfgitem, const char *attributename);
  int group_destroy(RootWnd *group);

// URL Launching Capabilities
  void navigateUrl(const char *url);

// translates @vars@
  const char *varmgr_translate(const char *str);

  int parse(const char *str, const char *how);

  void timer_add(TimerClient *client, int id, int ms);
  void timer_remove(TimerClient *client, int id=-1);

  void popupexit_deregister(PopupExitCallback *cb);
  void popupexit_register(PopupExitCallback *cb, RootWnd *watched);
  int popupexit_check(RootWnd *w);
  void popupexit_signal();

  void preferences_registerGroup(const char *group_id, const char *desc, GUID pref_guid, GUID pref_guid_parent=INVALID_GUID);
  int preferences_getNumGroups();
  const char *preferences_enumGroup(int n, const char **desc, GUID *this_guid, GUID *parent_guid);

  void appdeactivation_push_disallow(RootWnd *w);
  void appdeactivation_pop_disallow(RootWnd *w);
  int appdeactivation_isallowed(RootWnd *w);
  void appdeactivation_setbypass(int i);

  GuiObject *xui_new(const char *classname);
  void xui_delete(GuiObject *o);

  OSCURSORHANDLE cursor_request(const char *id);


#define hintGarbageCollect hint_garbageCollect  //FUCKO CUT

  // Dispatchable codes
  enum {
    GETTHISGUID   = 90,
    GETTHISCOMP   = 95,
    GETVERSION    = 100,
    GETVERSIONSTRING  = 101,
    GETBUILDNUMBER  = 102,
    GETGUID    = 103,
    GETHINSTANCE  = 104,
    GETICON    = 105,
    GETROOTWND    = 106,
    GETMAINTHREADHANDLE  = 107,
    GETAPPNAME    = 108,
    SETINTPRIVATE  = 200,
    GETINTPRIVATE  = 201,
    SETINTARRAYPRIVATE  = 202,
    GETINTARRAYPRIVATE  = 203,
    SETSTRINGPRIVATE  = 204,
    GETSTRINGPRIVATE  = 205,
    SETSTRINGPUBLIC  = 206,
    GETSTRINGPUBLIC  = 207,
    GETSTRINGPRIVATELEN  = 208,

    COREGETSUPPORTEDEXTENSIONS    = 300,
    COREGETEXTSUPPORTEDEXTENSIONS  = 301,
    CORECREATE        = 302,
    COREFREE        = 303,
    CORESETNEXTFILE      = 306,
    COREGETSTATUS      = 307,
    COREGETCURRENT      = 308,
    COREGETPOSITION      = 309,
    CORESETPOSITION      = 310,
    COREGETLENGTH      = 311,
    COREGETPLUGINDATA      = 312,
    COREGETVOLUME      = 313,
    CORESETVOLUME      = 314,
    COREADDCALLBACK      = 315,
    COREDELCALLBACK      = 316,
    COREGETVISDATA      = 317,
    COREGETLEFTVUMETER      = 318,
    COREGETRIGHTVUMETER      = 319,
    COREREGISTERSEQUENCER    = 320,
    COREDEREGISTERSEQUENCER    = 321,
    COREUSERBUTTON      = 322,
    COREGETEQSTATUS      = 323,
    CORESETEQSTATUS      = 324,
    COREGETEQPREAMP      = 325,
    CORESETEQPREAMP      = 326,
    COREGETEQBAND      = 327,
    CORESETEQBAND      = 328,
    COREREGISTEREXTENSION = 7030,  // 329 retired
    COREGETEXTENSIONFAMILY = 7040,
    COREUNREGISTEREXTENSION = 330,
    COREGETEQAUTO     = 331,
    CORESETEQAUTO     = 332,
    COREGETPAN        = 333,
    CORESETPAN        = 334,
    CORESETCUSTOMMSG  = 335,
    COREGETCURPLAYBACKNUMBER = 336,
    COREGETTITLE   = 337,
    COREGETINFO    = 338,

    SERVICEREGISTER      = 400,
    SERVICEDEREGISTER      = 401,
    SERVICEGETNUMSERVICES    = 402,
    SERVICEENUMSERVICE      = 403,
    SERVICELOCK        = 404,
    SERVICERELEASE      = 405,
    SERVICEGETTYPENAME      = 406,
    SERVICEGETOWNINGCOMPONENT    = 407,
    SERVICEGETLOCKINGCOMPONENT    = 408,
    SERVICECLIENTLOCK      = 409,
    SERVICEGETSERVICEBYGUID    = 410,
    SERVICEUNLOCK      = 411,
    SERVICEISVALID = 412,

    PUSHOBJECT = 1100,
    PUSHINT = 1101,
    PUSHBOOLEAN = 1102,
    PUSHFLOAT = 1103,
    PUSHDOUBLE = 1104,
    PUSHSTRING = 1105,
    PUSHVOID = 1106,
    PUSHANY = 1107,

    POPOBJECT = 1120,
    POPINT = 1121,
    POPBOOLEAN = 1122,
    POPFLOAT = 1123,
    POPDOUBLE = 1124,
    POPSTRING = 1125,
    POPDISCARD = 1126,
    POPANY = 1127,

    GETMAKIFUNCTION = 1128,
    ADDDLFREF = 1129,
    REMDLFREF = 1130,
    CALLFUNCTION = 1131,
    ADDDLFCLASSREF = 1132,

    TRIGGEREVENT = 1140,
    UPDATEDLF = 1141,

    GETSCRIPTOBJECT = 1160,
    GETSCRIPTINT = 1161,
    GETSCRIPTBOOLEAN = 1162,
    GETSCRIPTFLOAT = 1163,
    GETSCRIPTDOUBLE = 1164,
    GETSCRIPTSTRING = 1165,

    GETCACHECOUNT = 1166,
    ISVALIDSCRIPTID = 1167,
    MAPVARID = 1168,
    GETUSERANCESTORID = 1169,
    GETNUMEVENTS = 1170,
    GETEVENT = 1171,

    ADDSCRIPTOBJECT = 1172,
    REMOVESCRIPTOBJECT = 1173,

    MAKIINSTANTIATE = 1174,
    MAKIDESTROY = 1175,

    CREATEORPHAN = 1176,
    KILLORPHAN = 1177,
    MAKIGETCONTROLLER=1178,

    MAKIENCAPSULATE = 1179,
    MAKIDEENCAPSULATE = 1180,

    MAKIGETOBJATOM = 1181,
    MAKISETOBJATOM = 1182,

    MAKIFINDOBJECT = 1185,

    GETCOMPLETE = 1190,
    VCPUGETCLASSNAME = 1191,
    SETCOMPLETE = 1192,
    RESETCOMPLETE = 1193,

    // retired
    RETIRED_GROUPCREATE = 1200,
    RETIRED_GROUPCREATECFG = 1202,
    RETIRED_GROUPCREATELAYOUT = 1203,

    GROUPCREATE = 1205,
    GROUPDESTROY = 1201,
    GROUPCREATECFG = 1207,
    GROUPCREATELAYOUT = 1208,
    GROUPCREATEBYITEM = 1204,
    GROUPEXISTS = 1206,

    AUTOPOPUP_REGGUID = 1310,
    AUTOPOPUP_REGGROUP = 1311,
    AUTOPOPUP_UNREG = 1312,

    SYSCB_REGISTERCALLBACK=1400,
    SYSCB_DEREGISTERCALLBACK=1410,
    SYSCB_ISSUECALLBACK=1420,
    CMD_SENDCOMMAND=1600,
    CMD_BROADCASTCOMMAND=1610,
    CMD_POSTCOMMAND=1620,
    STATUS_SETTRAYTIPTEXT=1830,
    MAIN_ENABLEINPUT=1900,
    LOCALES_GETTRANSLATION=2000,
    LOCALES_ADDTRANSLATION=2010,
    LOCALES_GETBINDFROMACTION=2020,
    LOCALES_GETNUMENTRIES=2030,
    LOCALES_ENUMENTRY=2040,
    LOCALES_REGISTERACCELERATORSECTION=2050,
//retired    TIMEFMT_PRINTMINSEC=2100,
//retired    TIMEFMT_PRINTTIMESTAMP=2110,
    MESSAGEBOX=2300,
    GETMODALWND=2400,
    PUSHMODALWND=2410,
    POPMODALWND=2420,
    METRICS_ADDCALLBACK=2500,
    METRICS_DELCALLBACK=2510,
    METRICS_GETDELTA=2520,
    METRICS_SETDELTA=2530,
    MAIN_NAVIGATEURL=2620,
    METADB_OPTIMIZEPLAYSTRING=2700,
    METADB_ADDNEWITEM=2710,
    METADB_DELETEITEM=2715,
    METADB_SYNC=2720,
    METADB_DUPPLAYSTRING=2730,
    METADB_RELEASEPLAYSTRING=2740,
    METADB_GETINFO=2750,
    METADB_GETLENGTH=2760,
    METADB_GETMETADATA=2770,
    METADB_GETMETADATATYPE=2780,
    METADB_GETMETADATATABLE=2781,
    METADB_SETMETADATA=2790,
    METADB_RENDERDATA=2800,
    METADB_CONVERTTOTEXT=2810,
    METADB_DELETEMETADATA=2820,
    METADB_NEWSCANNER=2830,
    METADB_DELETESCANNER=2840,
    STARTCOMPLETED=3200,
    INCCOMPLETED=3210,
    ENDCOMPLETED=3220,
//    SKIN_GETELEMENTCOLOR=3300, // retired
//    SKIN_GETELEMENTREF=3310, // retired
    SKIN_GETELEMENTCOLOR2=3300,
    SKIN_GETELEMENTREF2=3310,
    SKIN_GETBITMAPCOLOR=3315,
    SKIN_GETITERATOR=3320,
    SKIN_SETSKINPATHOVERRIDE=3325,
    SKIN_RENDERBASETEXTURE=3330,
    SKIN_REGISTERBASETEXTUREWINDOW=3340,
    SKIN_UNREGISTERBASETEXTUREWINDOW=3350,
    SKIN_SWITCHSKIN=3360,
    SKIN_GETVERSION=3365,
    SETLOCKUI=3370,
    GETLOCKUI=3380,
    GETSKINNAME=3400,
    GETSKINPATH=3410,
    GETSKINSPATH=3415,
    GETDEFAULTSKINPATH=3420,
    PATH_GETAPPPATH=3430,
    IMGLDR_MAKEBMP=3500,
    OLDIMGLDR_MAKEBMP2=3510, // DEPRECATED, CUT
    IMGLDR_MAKEBMP2=3511,
    IMGLDR_REQUESTSKINBITMAP=3520,
    IMGLDR_RELEASESKINBITMAP=3530,
    IMGLDR_REQUESTSKINREGION=3540,
    IMGLDR_CACHESKINREGION=3550,
    REGISTERSKINFILTER=3600,
    UNREGISTERSKINFILTER=3610,
    REAPPLYSKINFILTERS=3620,
    FILTERSKINCOLOR=3630,
    GETNUMCOLORSETS=3650,
    ENUMCOLORSET=3660,
    GETNUMCOLORGROUPS=3670,
    ENUMCOLORGROUP=3680,
    ENUMCOLORGROUP2=3681,
    GETCOLORGROUP=3682,
    SETCOLORSET=3690,
    GETCOLORSET=3695,
    NEWCOLORSET=3696,
    UPDATECOLORSET=3697,
    PAINTSET_RENDER=3700,
    PAINTSET_RENDERTITLE=3710,
    PAINTSET_PRESENT=3720,
    DRAWANIMATEDRECTS=3800,
    SENDDDECOMMAND=3900,

    XMLREADER_REGISTERCALLBACK=4000,
    XMLREADER_UNREGISTERCALLBACK=4010,
    XMLREADER_LOADFILE=4020,
    XMLREADER_STARTXMLLOAD=4030,
    XMLREADER_LOADCHUNK=4040,
    XMLREADER_CLOSE=4050,

    WNDTRACKADD=4100,
    WNDTRACKREMOVE=4110,
    WNDTRACKDOCK=4120,
    WNDTRACKDOCK2=4130,
    WNDTRACKSTARTCOOPERATIVE=4140,
    WNDTRACKENDCOOPERATIVE=4150,
    WNDTRACKWASCOOPERATIVE=4160,
    WNDTRACKINVALIDATEALL=4170,
    GETNUMCOMPONENTS=4200,
    GETCOMPONENTGUID=4210,
    GETCOMPONENTNAME=4220,
/*retired    GETCOMPONENTINFO=4230, */
    COMPON_GETCFGINTERFACE=4240,
    SKINWND_CREATEBYGUID=4250,
    SKINWND_CREATEBYGROUPID=4251,
    SKINWND_TOGGLEBYGUID=4252,
    SKINWND_TOGGLEBYGROUPID=4253,
    SKINWND_DESTROY=4254,
    SKINWND_GETNUMBYGUID=4255,
    SKINWND_ENUMBYGUID=4256,
    SKINWND_GETNUMBYGROUPID=4257,
    SKINWND_ENUMBYGROUPID=4258,
    SKINWND_ATTACHTOSKIN=4259,
    WNDHOLDER_REGISTER=4270,
    WNDHOLDER_UNREGISTER=4271,
/*retired    COMP_RENDERELEMENT=4300, */
    COMP_NOTIFYSCRIPTS=4310,
    SKIN_GETCONTAINER=4500,
    SKIN_GETLAYOUT=4510,
    ROOTWNDFROMPOINT=5000,
    REGISTERROOTWND=5100,
    UNREGISTERROOTWND=5200,
    ROOTWNDISVALID=5300,
    FORWARDONMOUSEWHEEL=5310,
    FORWARDONCHAR=5400,
    FORWARDONKEYDOWN=5410,
    FORWARDONKEYUP=5420,
    FORWARDONSYSKEYDOWN=5421,
    FORWARDONSYSKEYUP=5422,
    FORWARDONKILLFOCUS=5430,
    INTERCEPTONCHAR=5450,
    INTERCEPTONKEYDOWN=5460,
    INTERCEPTONKEYUP=5470,
    INTERCEPTONSYSKEYDOWN=5480,
    INTERCEPTONSYSKEYUP=5490,
    HOOKKEYBOARD=5491,
    UNHOOKKEYBOARD=5492,
    PUSHLOCK=5493,
    POPLOCK=5494,
    ISKBDLOCKED=5495,
    KBDRESET=5495,
    FILEOPEN=5500,
    FILECLOSE=5510,
    FILEREAD=5520,
    FILEWRITE=5530,
    FILESEEK=5540,
    FILETELL=5550,
    FILEGETFILESIZE=5560,
    FILEEXISTS=5570,
    FILEREMOVE=5580,
    FILEMOVE=5585,
    FILEREMOVEUNDOABLE=5590,
    FONT_TEXTOUT=5600,
    FONT_GETINFO=5610,
    HINT_GARBAGECOLLECT=5700,
    CONSOLE_OUTPUTSTRING=5800,
    LOADSKINFILE=5901,
    UNLOADSKINPART=5902,
    NAVIGATEURL=6000,
    ___RETIREDMETHOD=6010,
    GETINTPUBLIC=6100,
    SETINTPUBLIC=6110,
    COREGETWRITEPOSITION=6200,
    PARSE=6301,
    SYSMALLOC=6400,
    SYSFREE=6410,
    SYSREALLOC=6420,
    SYSMEMCHANGED=6430,
    VARMGRTRANSLATE = 6500,
    TIMERADD = 6600,
    TIMERREMOVE = 6610,
    MAIN_GETCOMMANDLINE = 6620,
    MAIN_SETWINDOWTITLE = 6630,
    MAIN_SHUTDOWN = 6640,
    MAIN_CANCELSHUTDOWN = 6641,
    MAIN_ISSHUTTINGDOWN = 6642,
    MAIN_ISINETAVAILABLE = 6650,
    APP_GETINITCOUNT = 6660,

    CONFIG_REGISTERCFGITEM = 6700,
    CONFIG_DEREGISTERCFGITEM = 6710,
    CONFIG_GETNUMCFGITEMS = 6720,
    CONFIG_ENUMCFGITEM = 6730,
    CONFIG_GETCFGITEMBYGUID = 6740,

    POPUPEXIT_CHECK = 6800,
    POPUPEXIT_SIGNAL = 6810,
    POPUPEXIT_REGISTER= 6820,
    POPUPEXIT_DEREGISTER= 6830,

    PREFERENCES_REGISTERGROUP = 6900,
    PREFERENCES_GETNUMGROUPS = 6910,
    PREFERENCES_ENUMGROUP = 6920,

    // 7030 = COREREGISTEREXTENSION
    // 7040 = COREGETEXTENSIONFAMILY

    XUI_NEW = 7100,
    XUI_DELETE = 7110,

    APPNODEACTIVATE_PUSH = 7200,
    APPNODEACTIVATE_POP = 7210,
    APPNODEACTIVATE_CHECK = 7220,
    APPNODEACTIVATE_BYPASS = 7230,

    // 7300?



    PATH_GETCOMPONENTPATH=8000,
    PATH_GETCOMPONENTDATAPATH=8010,
    PATH_GETUSERSETTINGSPATH=8020,

    CURSOR_REQUEST=8100,

    //
    END_OF_DISPATCH
  };
};

// these are a bunch of helper implementations of the above methods
#include "apihelp.h"

extern ComponentAPI *api;

// DEPRECATED
#define getAppPath path_getAppPath

#endif

#endif
