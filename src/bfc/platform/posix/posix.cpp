#include "wasabicfg.h"
#include <bfc/platform/platform.h>

#ifndef WASABI_PLATFORM_POSIX
#error this file is only for the POSIX Subplatform
#endif

#include <bfc/ptrlist.h>
#include <bfc/string/string.h>
#include <bfc/critsec.h>
#include <bfc/thread.h>
#include <api/application/ipcs.h>
#ifdef WASABI_PLATFORM_DARWIN
#include <kern/clock.h>
#endif
#ifdef WASABI_PLATFORM_XWINDOW
#include <wnd/rootwnd.h>
#endif

int MulDiv( int m1, int m2, int d ) {
#ifndef WASABI_PLATFORM_DARWIN
  __asm__ volatile (
        "mov %0, %%eax\n"
        "mov %1, %%ebx\n"
        "mov %2, %%ecx\n"
        "mul %%ebx\n"
        "div %%ecx\n"
        : : "m" (m1), "m" (m2), "m" (d)
        : "%eax", "%ebx", "%ecx", "%edx" );
#else
  __int64 m = m1*m2;
  return m/d;
#endif
}

void ExitProcess( int ret ) {
  exit( ret );
}

const char *CharPrev(const char *lpszStart, const char *lpszCurrent) {
  if (lpszCurrent-1 >= lpszStart) return lpszCurrent-1;
  return lpszStart;
}

void OutputDebugString( const char *s ) {
#ifdef _DEBUG
  fprintf( stderr, "%s", s );
#endif
  char *file = getenv( "WASABI_LOG_FILE" );
  if ( file ) {
    if ( !STRCMP( file, "-" ) ) {
      fprintf( stdout, "%s", s );
    } else {
      FILE *f = fopen( file, "a" );
      if ( f ) {
        fprintf( f, "%s", s );
        fclose( f );
      }
    }
  }
}

DWORD GetTickCount() {
  static int starttime = -1;

  if ( starttime == -1 )
    starttime = time( NULL );
/*CUT
//CUT ftime() is obsolete. gettimeofday() is better.
#ifndef DARWIN
  struct timeb tb;
  ftime( &tb );
  tb.time -= starttime;
  return tb.time * 1000 + tb.millitm;
#else
*/
  struct timeval tv;
  gettimeofday(&tv,NULL);
  tv.tv_sec -= starttime;
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  //struct mach_timespec ts = clock_get_system_value();
  //return ts.tv_sec*1000 + ts.tv_nsec/1000000;
  //OutputDebugString("port darwin GetTickCount\n");
//CUT #endif
}

void Sleep( int ms ) {
  if ( ms != 0 ) {
    struct timespec ts, rem;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    while (-1 == nanosleep( &ts, &rem)) {
      ts.tv_sec = rem.tv_sec;
      ts.tv_nsec = rem.tv_nsec;
    }
  } else
    sched_yield();
}

DWORD GetModuleFileName(void *pid, const char *filename, int bufsize) {
  char procbuffer[512];
  sprintf(procbuffer, "/proc/%ld/exe", (long)pid);
  return readlink(procbuffer, (char *)filename, bufsize);
}

//
// Win32-like Messages and timers.
//

#if defined(WASABI_COMPILE_TIMERS) || defined(WASABI_PLATFORM_XWINDOW)

struct TimerElem {
  OSWINDOWHANDLE win;
  int id;
  int nexttime;
  int delta;
  TIMERPROC tproc;

  TimerElem( OSWINDOWHANDLE _win, int _id, int ms, TIMERPROC _tproc ) {
    win = _win;
    id = _id;
    delta = ms;
    tproc = _tproc;
    nexttime = Std::getTickCount() + delta;
  }
};

int timer_id = 0;
CriticalSection timer_cs;
PtrList<TimerElem> timer_elems;

void KillTimer( OSWINDOWHANDLE win, int id ) {
  timer_cs.enter();
  for( int i = 0; i < timer_elems.getNumItems(); i++ )
    if ( timer_elems[i]->win == win && timer_elems[i]->id == id ) {
      delete timer_elems[i];
      timer_elems.delByPos( i );
      i--;
    }
  timer_cs.leave();
}

int SetTimer( OSWINDOWHANDLE win, int id, int ms, TIMERPROC tproc ) {
  KillTimer(win, id);

  if ( win == INVALIDOSWINDOWHANDLE ) {
    id = timer_id++;
  }

  TimerElem *te = new TimerElem( win, id, ms, tproc );
  timer_cs.enter();
  timer_elems.addItem( te, PTRLIST_POS_LAST );
  timer_cs.leave();

  return id;
}

CriticalSection send_cs;
MSG *send_msg;
int sending = 0;
int send_ret;
pthread_t message_thread = (pthread_t)-1;

int _GetMessage( MSG *m, OSWINDOWHANDLE, UINT, UINT, int block=1) {
  MEMSET( m, 0, sizeof( MSG ) );

  message_thread = pthread_self();

#ifdef WASABI_PLATFORM_XWINDOW
  XEvent e;
#endif // wnd
  int curtime;
  int done = 0;
  int first = 1;
  static wa_msgbuf ipcm;
  static int qid = -1;
  int size;
#ifdef WASABI_API_LINUX
  if ( qid == -1 ) { qid = WASABI_API_LINUX->linux_getIPCId(); }
#endif
  if ( sending ) {
    *m = *send_msg;
    done = 1;
  }

  while( !done && (block || first)) {
    if ( qid != -1 ) {
#ifndef WASABI_PLATFORM_DARWIN
      if ( (size = msgrcv( qid, &ipcm, IPC_MSGMAX , 0, IPC_NOWAIT )) != -1 )
#else
        DebugString("port darwin message queue\n");
#endif
      {
        m->hwnd = None;
        m->message = WM_WA_IPC;
        m->wParam = (WPARAM)&ipcm;
        break;
      }
    }

    curtime = GetTickCount();

    timer_cs.enter();
    for( int i = 0; i < timer_elems.getNumItems(); i++ ) {
      if ( timer_elems[i]->nexttime < curtime ) {
        if (block)
          while( timer_elems[i]->nexttime < curtime )
            timer_elems[i]->nexttime += timer_elems[i]->delta;

        m->hwnd    = timer_elems[i]->win;
        m->message = WM_TIMER;
        m->wParam  = (WPARAM)timer_elems[i]->id;
        m->lParam  = (LPARAM)timer_elems[i]->tproc;

        done = 1;
      }
    }
    timer_cs.leave();

    if ( !done && ! first )
      Sleep( 1 );
    else
      first = 0;

#ifdef WASABI_PLATFORM_XWINDOW
    if ( !done && XPending( XWindow::getDisplay() ) ) {
      int n = XEventsQueued( XWindow::getDisplay(), QueuedAlready );

      for ( int i = 0; !done && i < n; i++ ) {
        XNextEvent( XWindow::getDisplay(), &e );
        if ( XWindow::convertEvent( m, &e ) )
          done = 1;
      }
      if ( done )
        break;
    }
#endif // wnd
  }

  return m->message != WM_QUIT;
}

int GetMessage( MSG *m, OSWINDOWHANDLE w, UINT f, UINT l) {
  return _GetMessage(m, w, f, l, 1);
}

// on linux, we don't really simply peek when PM_NOREMOVE is used,
// we just don't block, which is the only thing we want to accomplish here
int PeekMessage( MSG *m, OSWINDOWHANDLE w, UINT f, UINT l, UINT remove) {
  if (remove == PM_NOREMOVE) return _GetMessage(m, w, f, l, 0);
  else return _GetMessage(m, w, f, l, 1);
}


int DispatchMessage( MSG *m ) {
  if ( m->message == WM_TIMER && m->hwnd == None ) {
    TIMERPROC tproc = (TIMERPROC)m->lParam;
    tproc( m->hwnd, m->message, m->wParam, 0 );
    return 1;
  }

  int ret = 0;

#ifdef WASABI_PLATFORM_XWINDOW
  RootWnd *rootwnd = (RootWnd *)GetWindowLong( m->hwnd, GWL_USERDATA );

  if ( rootwnd ) {
    ret = rootwnd->wndProc( m->hwnd, m->message, m->wParam, m->lParam );
    rootwnd->performBatchProcesses();
  }
#endif // wnd

  if ( sending ) {
    send_ret = ret;
    sending = 0;
  }

  return ret;
}

int SendMessage( OSWINDOWHANDLE win, UINT msg, WPARAM wParam, LPARAM lParam ) {
  MSG m;
  m.hwnd = win;
  m.message = msg;
  m.wParam = wParam;
  m.lParam = lParam;

  int ret;

  if ( pthread_equal( message_thread, pthread_self() ) ) {
    return DispatchMessage( &m );

  } else {
    send_cs.enter();
    sending = 1;
    send_msg = &m;
    while( sending ) { Sleep( 1 ); }
    ret = send_ret;
    send_cs.leave();

    return ret;
  }
}

#ifndef WASABI_PLATFORM_XWINDOW
int TranslateMessage( MSG * ) {
  return 0;
}
#endif

#endif // timer | wnd

/* CUT WaI> Unused Win32-wannabees.
void CopyFile( const char *f1, const char *f2, BOOL b ) {
  COPYFILE( f1, f2 );
}
*/
// IntersectRect, SubtractRect(), EqualRect() are fairly specific to windowing, but
//   these implementations are non-XWindow (or anything else) specific
//   enough to warrant warrant putting them here.
int IntersectRect( RECT *out, const RECT *i1, const RECT *i2 ) {
  return Std::rectIntersect(i1, i2, out);
}

int SubtractRect( RECT *out, const RECT *in1, const RECT *in2 ) {
  int ret;
  if ( in1->left >= in2->left && in1->right <= in2->right ) {
    out->left = in1->left; out->right = in1->right;

    if ( in1->top >= in2->top && in2->bottom >= in2->top && in2->bottom <= in2->bottom ) {
      out->top = in1->bottom; out->bottom = in2->bottom;

      ret = 1;
    } else if ( in1->top <= in2->top && in1->bottom >= in2->top && in1->bottom <= in2->bottom ) {
      out->top = in1->top; out->bottom = in2->top;

      ret = 1;
    } else {
      ret = 0;
    }

  } else if ( in1->top >= in2->top && in1->bottom <= in2->bottom ) {
    out->top = in1->top; out->bottom = in1->bottom;

    if ( in1->left >= in2->left && in2->right >= in2->left && in2->right <= in2->right ) {
      out->left = in1->right; out->right = in2->right;

      ret = 1;
    } else if ( in1->left <= in2->left && in1->right >= in2->left && in1->right <= in2->right ) {
      out->left = in1->left; out->right = in2->left;

      ret = 1;
    } else {
      ret = 0;
    }


  } else {
    ret = 0;
  }
  return ret;
}

int EqualRect( RECT *a, RECT *b ) {
  return ( a->top == b->top && a->bottom == b->bottom &&
     a->left == b->left && a->right == b->right );
}


