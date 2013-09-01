#if defined(LINUX) || defined(DARWIN)
#include <precomp.h>
#include "linuxapi.h"
#include <bfc/ptrlist.h>
#include <bfc/string/string.h>

#ifdef XWINDOW
static Display *display;
static XContext context;
static PtrList<XShmSegmentInfo> shmsegs;

static XineramaScreenInfo *screens;
static int num_screens;

extern int (*_XErrorFunction)( Display *, XErrorEvent *);
extern int (*_XIOErrorFunction)( Display * );
int (*orig_error)( Display *, XErrorEvent *);
int (*orig_ioerror)( Display * );

#endif

static DWORD starttime;

#ifdef XWINDOW
void clean_segfault( int sig ) {
  // Make sure even in a crash we clean up the ipc stuff
  int ipcid = LinuxAPI::getIPCId();
  if ( ipcid != -1 ) {
    #ifndef WASABI_PLATFORM_DARWIN
    msgctl( ipcid, IPC_RMID, 0 );
    #else
    DebugString("port darwin msg queues!\n");
    #endif
  }

  for( int i = 0; i < shmsegs.getNumItems(); i++ ) {
    if ( shmsegs[i]->shmaddr )
      shmdt( shmsegs[i]->shmaddr );

    if ( shmsegs[i]->shmid >= 0 )
      shmctl( shmsegs[i]->shmid, IPC_RMID, NULL );
  }

  kill( getpid(), SIGTERM );
}

int LinuxIOErrorHandler( Display *d ) {
  orig_ioerror( d );

  clean_segfault( SIGSEGV );

  return 1;
}

int LinuxErrorHandler( Display *d, XErrorEvent *e ) {
  char tmp[8196];

  orig_error( d, e );

  clean_segfault( SIGSEGV );

  return 1;
}
#endif

int LinuxAPI::init() {
  starttime = time( NULL );

#ifdef XWINDOW
  XInitThreads();

  signal( SIGSEGV, clean_segfault );
  signal( SIGINT, clean_segfault );
#endif

#ifdef XWINDOW
  if ( (display = XOpenDisplay( NULL ))  == NULL ) {
    return 0;
  }

  context = XUniqueContext();

  // Since all Win32 events happen this way we probably should too
  // This doesn't seam to cause to much detrimental performance on Default skin
  // I think that XSync after XShmAttach and XCopyArea would fix it though
  XSynchronize( display, True );
#endif

#ifdef XWINDOW
#ifdef _DEBUG
  orig_error = _XErrorFunction;
  orig_ioerror = _XIOErrorFunction;

  XSetErrorHandler( LinuxErrorHandler );
  XSetIOErrorHandler( LinuxIOErrorHandler );
#endif
#endif

  return 1;
}

void LinuxAPI::kill() {
#ifdef XWINODOW
  XFree(screens);
  XCloseDisplay( display );
#endif

  int ipcid = getIPCId();
  if ( ipcid != -1 ) {
    #ifndef WASABI_PLATFORM_DARWIN
    msgctl( ipcid, IPC_RMID, 0 );
    #else
    DebugString("port darwin msg queues!\n");
    #endif
  }
}

#ifdef XWINDOW
Display *LinuxAPI::getDisplay() {
  return display;
}

XContext LinuxAPI::getContext() {
  return context;
}
#endif

DWORD LinuxAPI::getStarttime() {
  return starttime;
}

static int ipc_id = -1;

void LinuxAPI::setIPCId( int id ) { ipc_id = id; }
int LinuxAPI::getIPCId( void ) { return ipc_id; }

#ifdef XWINDOW
XShmSegmentInfo *LinuxAPI::createXShm( int nbytes ) {
  XShmSegmentInfo *ret;

  ret = (XShmSegmentInfo *)MALLOC(sizeof(XShmSegmentInfo));
  if(!ret) return NULL;
  ret->shmid = shmget( IPC_PRIVATE, nbytes, IPC_CREAT|0777);
  ASSERT( ret->shmid >= 0 );
  ret->shmaddr = (char *)shmat( ret->shmid, 0, 0);
  ret->readOnly = False;

  XShmAttach( getDisplay(), ret );

  shmsegs.addItem( ret );

  return ret;
}

void LinuxAPI::destroyXShm( XShmSegmentInfo *shm ) {
  XShmDetach( getDisplay(), shm );

  if ( shm->shmaddr )
    shmdt( shm->shmaddr );
  else
    DebugString( "Invalid shmaddr\n" );
  if ( shm->shmid >= 0 )
    shmctl( shm->shmid, IPC_RMID, NULL );
  else
    DebugString( "Invalid shmid\n" );

  shmsegs.removeItem( shm );

  FREE( shm );
}

const XineramaScreenInfo *LinuxAPI::getXineramaInfo( int *num ) {
  if(screens) {
    *num = num_screens;
    return screens;
  }
  screens = XineramaQueryScreens(getDisplay(), num);
  return screens;
}
#endif

#endif
