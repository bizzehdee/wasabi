#ifndef _WASABI_POSIX_H
#define _WASABI_POSIX_H

#ifndef WASABI_PLATFORM_POSIX
#error this file is only for the POSIX Subplatform
#endif

#ifndef _WASABI_PLATFORM_H
#ifndef _WASABI_POSIX_H_ALLOW_INCLUDE // You must know what you are doing to include this file.
#error only include from bfc/platform/platform.h
#endif
#endif

//-------------------------------------------------------------------
// NOTE
// We take the term POSIX loosely here, not strictly the IEEE POSIX
// specification. E.g., This platform includes some Sys V and BSD
// headers. Most POSIX OS's will include this Subplatform.
//
// Feel free to #ifdef parts that need to be modified/excluded
// for your platform (such as is done with DARWIN below).
//
// I have tried to accurately note the standard each header belongs 
// to.
//-------------------------------------------------------------------


#include <stdio.h>   //ISO C99
#include <stdlib.h>  //ISO C99
#include <stdarg.h>  //ISO C
#include <ctype.h>   //ISO C99
#include <signal.h>  //ISO C99
#include <errno.h>   //ISO C99
#include <math.h>    //ISO C99
#ifndef WASABI_PLATFORM_DARWIN
#include <string.h>  //ISO C99
#else
// #include <string.h> grabs the wrong file on Darwin.
#include "/usr/include/string.h"
#endif

//CUT? #include <strings.h> //BSD

#ifndef WASABI_PLATFORM_DARWIN
#include <dlfcn.h>   //POSIX
#include <sys/msg.h> //Sys V
#endif

#include <sys/shm.h> //Sys V
#include <sys/ipc.h> //Sys V

#ifndef NOVTABLE
#define NOVTABLE
#endif

// http://gcc.gnu.org/onlinedocs/gcc/Thread_002dLocal.html#Thread_002dLocal
#ifndef WASABI_PLATFORM_DARWIN
#define THREADSTORAGE __thread
#else
#define THREADSTORAGE
#endif

#ifndef __USE_GNU
#define __USE_GNU
#include <pthread.h> //ISO C99
#undef __USE_GNU
#else
#include <pthread.h>
#endif

#include <unistd.h>    //POSIX - Symbolic Constants
#include <dirent.h>    //POSIX - Directory Operations
#include <sys/stat.h>  //POSIX - File Characteristics
#include <sys/types.h> //POSIX - Primitive System Data Types
//CUT #include <sys/timeb.h>   //POSIX - OBSOLETE
#include <sys/time.h>

typedef void* THREAD_RET;

// Come back here later
typedef struct {
  pthread_mutex_t mutex;
  pthread_t id;
  int count;
} CRITICAL_SECTION;
typedef pthread_t OSTHREADHANDLE;



typedef unsigned long COLORREF;
typedef struct {
  char rgbBlue;
  char rgbGreen;
  char rgbRed;
  char filler;
} RGBQUAD;
#define CLR_NONE    0

//CUT? #define min( a, b ) ((a>b)?b:a)
//CUT? #define max( a, b ) ((a>b)?a:b)

#ifdef __cplusplus
#ifndef XMD_H
typedef int BOOL; // It's int not bool because of some inheritance conflicts
#endif
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#else //__cplusplus
#ifndef XMD_H
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif //__cplusplus

#define __fastcall

typedef BOOL BOOLEAN;

//typedef int LRESULT;
typedef long LRESULT;
//typedef int LPARAM;
typedef long LPARAM;
//typedef int WPARAM;
typedef long WPARAM;
//typedef int RPARAM;
typedef long RPARAM;
typedef unsigned int TCHAR;
typedef long long __int64;
typedef long long LARGE_INTEGER;
typedef unsigned long long ULARGE_INTEGER;
#define OSPIPE int
#define OSPROCESSID int
#define LOWORD(a) ((a)&0xffff)
#define HIWORD(a) (((a)>>16)&0xffff)

#define MAX_PATH 8192

//CUT #define COMEXP
#define EXTC extern "C"
//CUT #define __cdecl
#define CALLBACK
//CUT #define WINAPI
#define HRESULT void*
//CUT #define WINUSERAPI
#define APIENTRY
//CUT? #define __declspec(a)
typedef char * LPSTR;
typedef unsigned long DWORD;
typedef short int WORD;
#ifndef XMD_H
typedef unsigned char BYTE;
#endif
typedef void* LPVOID;
typedef struct {
  long cx, cy;
} SIZE;
typedef long LONG;
#define VOID void

typedef char OLECHAR;

//#define NO_MMX
//CUT? #define NULLREGION    0

#define MAIN_MINX 0
#define MAIN_MINY 0

#ifdef __cplusplus

#define _TRY try
#define _EXCEPT(x) catch(...)

#include <new>

// some of these are used even without wnd support

enum {
  WM_CREATE,    // NULL
  WM_CLOSE,     // NULL
  WM_PAINT,     // NULL
  WM_NCPAINT,   // NULL
  WM_SYNCPAINT, // NULL
  WM_SETCURSOR, // NULL
  WM_TIMER,     // timerid
  WM_SETFOCUS,  // NULL
  WM_KILLFOCUS, // NULL
  WM_LBUTTONDOWN, // xPos | yPos << 16
  WM_RBUTTONDOWN, // "
  WM_MBUTTONDOWN, // "
  WM_MOUSEMOVE,   // "
  WM_LBUTTONUP,   // "
  WM_RBUTTONUP,   // "
  WM_MBUTTONUP,   // "
  WM_CONTEXTMENU, // "
  WM_ERASEBKGND,  // NULL
  WM_MOUSEWHEEL,  // a << 16 | t (l=a/120)
  WM_CHAR,    // char
  WM_KEYDOWN, // keypress
  WM_KEYUP,   // "
  WM_SYSKEYDOWN,  // look at OnSysKeyDown
  WM_SYSKEYUP,    // "
  WM_SYSCOMMAND,    // Hunh?
  WM_MOUSEACTIVATE, // Hunh?
  WM_ACTIVATEAPP,   // Hunh?
  WM_ACTIVATE,    // WA_ACTIVE || WA_CLICKACTIVE
  WM_NCACTIVATE,  // NULL
  WM_WINDOWPOSCHANGED,  // NULL, WINDOWPOS *
  WM_DROPFILES,         // HDROP
  WM_CAPTURECHANGED,    // NULL
  WM_COMMAND,     // Commands?..
  WM_SETTINGCHANGE,
  WM_QUIT,
  WM_DESTROY,
  WM_USER = 1000, // wParam, lParam -> make sure this is last
};

#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002 // ignored

#endif //__cplusplus

typedef void* OSMODULEHANDLE; //HINSTANCE; // Useless, just a placeholder
//CUT? typedef void* HMONITOR;
//CUT? typedef void* WIN32_FIND_DATA;
//CUT? typedef void* WIN32_FIND_DATAW;
typedef void* BLENDFUNCTION;
typedef void* ATOM;
//CUT? typedef void* HGLOBAL;
//CUT? typedef void* HKEY;
typedef char* LPTSTR;
typedef char* LPCTSTR;
typedef DWORD* LPDWORD;

int MulDiv( int m1, int m2, int d );
void ExitProcess( int ret );
const char *CharPrev(const char *lpszStart, const char *lpszCurrent);
void OutputDebugString( const char *s );
DWORD GetTickCount();
void Sleep( int ms );
DWORD GetModuleFileName(void *pid, const char *filename, int bufsize);

#ifndef WASABI_PLATFORM_XWINDOW
#define WASABI_WIN32RECT
typedef struct { int left, top, right, bottom; } RECT;
typedef struct { int x, y; } POINT; 
#endif

//CUT? #if defined(WASABI_COMPILE_TIMERS) || defined(WASABI_PLATFORM_XWINDOW)

#ifdef WASABI_PLATFORM_XWINDOW
#include <bfc/platform/xwindow/types.h>
#else
typedef void * OSWINDOWHANDLE;
typedef void * OSREGIONHANDLE;
typedef void * OSFONTHANDLE;
typedef void * OSCURSORHANDLE;
typedef void * OSICONHANDLE;
#ifndef None
#define None (OSWINDOWHANDLE)0
#endif

typedef void (*TIMERPROC)(OSWINDOWHANDLE, UINT, UINT, DWORD);

typedef struct {
  OSWINDOWHANDLE hwnd;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD time;
  POINT pt;
} MSG;

#endif // !X

int SetTimer( OSWINDOWHANDLE, int id, int ms, TIMERPROC );
void KillTimer( OSWINDOWHANDLE, int id );
int GetMessage( MSG *, OSWINDOWHANDLE, UINT, UINT );
int PeekMessage( MSG *, OSWINDOWHANDLE, UINT, UINT, UINT );
int DispatchMessage( MSG * );
#ifndef WASABI_PLATFORM_XWINDOW
int TranslateMessage( MSG * );
#endif
int SendMessage( OSWINDOWHANDLE, UINT, WPARAM, LPARAM );

//CUT? #endif

//CUT WaI> Unused Win32-wannabees.
int IntersectRect(RECT *, const RECT *, const RECT *);
int SubtractRect( RECT *out, const RECT *in1, const RECT *in2 );
int EqualRect( RECT *a, RECT *b );
//CUT void CopyFile( const char *f1, const char *f2, BOOL b );






#endif//_WASABI_POSIX_H
