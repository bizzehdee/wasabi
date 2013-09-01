#ifndef _STD_H
#define _STD_H

// this is the base include file for all wasabi code
// contains wrappers for a lot of basic things needed by all platforms

#include <bfc/platform/platform.h>
//CUT #include <bfc/common.h>
#include <bfc/assert.h>

#ifdef __cplusplus
#include <wasabicfg.h>
#endif

//#define WANT_UTF8_WARNINGS

// to use this, do #pragma WARNING("your message") <- note the ABSENCE of ';'
#ifndef WARNING
  #define WARNING_TOSTR(text) #text
  #define WARNING_TOSTR1(text) WARNING_TOSTR(text)
  #define WARNING(text) message(__FILE__ "(" WARNING_TOSTR1(__LINE__) ") : " text)
#endif
// and for this one, do #pragma COMPILATION_CHAT("your_nick", "nick_you're_talking_to", "your message")
#define CHAT(from, to, text) message(__FILE__ "(" WARNING_TOSTR1(__LINE__) ") : <" from "> " to ": " text)
#define SELF(from, text) message(__FILE__ "(" WARNING_TOSTR1(__LINE__) ") : * " from"/#wasabi " text)

//#ifndef WASABIDLLEXPORT
//#error go define WASABIDLLEXPORT in your platform .h
//#endif

#ifndef NOVTABLE
#define NOVTABLE
#endif

#ifndef NOVTABLE
#error go define NOVTABLE in your platform .h
#endif

#include "std_cpu.h"
#include "std_mem.h"
#include "std_math.h"
#include "std_string.h"
#include "std_file.h"
#if defined(WASABI_PLATFORM_WIN32) || defined(WASABI_PLATFORM_XWINDOW)
#include "std_wnd.h"
#endif

#define WASABI_DEFAULT_FONTNAME "Arial"

// seconds since 1970
#ifdef _WIN64
  typedef unsigned long long stdtimeval;
#else
  typedef unsigned long stdtimeval;
#endif
// milliseconds since...??
typedef double stdtimevalf;
//CUT typedef double stdtimevalms;
#define stdtimevalms stdtimevalf	// back compat

#ifdef __cplusplus

#include <bfc/string/string.h>

template<class T> inline void SWAP(T& a, T& b) {
  T tmp = a; a = b; b = tmp;
}

namespace Std {
  int keyDown(int code);

  int pointInRect(RECT *r, int x, int y);
  int pointInRect(RECT *r, POINT &p);
  void setRect(RECT *r, int left, int top, int right, int bottom);
  void setRectWH(RECT *r, int x, int y, unsigned int width, unsigned int height);
  void expandRect(RECT *r, int horizontal_delta, int vertical_delta);
  void maxRect(RECT *result, RECT *r1, RECT *r2);
  RECT makeRect(int left=0, int top=0, int right=0, int bottom=0);
  RECT makeRectWH(int x=0, int y=0, unsigned int width=0, unsigned int height=0);
  POINT makePoint(int x, int y);
  void offsetRect(RECT *r, int x, int y);
  void multiplyRect(RECT *r, float v);
  int rectEqual(const RECT &a, const RECT &b);
  int rectEqual(const RECT *a, const RECT *b);
  int rectIntersect(const RECT &a, const RECT &b, RECT *intersection=NULL);
  int rectIntersect(const RECT *a, const RECT *b, RECT *intersection=NULL);
  int rectSubtract(const RECT &a, const RECT &b, RECT *a_minus_b=NULL);
  int rectSubtract(const RECT *a, const RECT *b, RECT *a_minus_b=NULL);

  // note: this random sucks. use the one in StdRand instead	//DEPRECATE
  int random(int);
  void srandom(unsigned int key=0);//if key==0, uses time()
  unsigned int random32(unsigned int max = 0xffffffff);  // full 32-bits of randomness

  // time functions
  void sleep(int s);
  void usleep(int ms);
  // get time in seconds since 1970
  stdtimeval getTimeStamp();
  // get time in seconds as double, but not since any specific time
  // useful for relative timestamps only
  stdtimevalf getTimeStampMS();
  // get milliseconds since system started. usefull for relative only
  DWORD getTickCount();

  void tolowerString(char *str);
  void ensureVisible(RECT *r);

//deprecated  int getScreenWidth();	//FUCKO: these should be subsumed by getViewport
//deprecated  int getScreenHeight();

// thread stuff moved to std_cpu.h

  String getLastErrorString(int force_errno=-1);

  int dirChar();  // \ for win32, / for all else
#define DIRCHAR (Std::dirChar())
  const char *dirCharStr(); // "\\" for win32, "/" for all else
#define DIRCHARSTR (Std::dirCharStr())
  int isDirChar(int thechar, int allow_multiple_platforms = TRUE);
  const char *matchAllFiles(); // "*.*" on win32, "*" on else
#define MATCHALLFILES (Std::matchAllFiles())
  const char *dotDir();  // usually "."
#define DOTDIR (Std::dotDir())
  const char *dotDotDir();  // usually ".."
#define DOTDOTDIR (Std::dotDotDir())

  int isRootPath(const char *path); // "c:\" or "\" on win32, "/" on linux

  int switchChar();   // '/' on win32, '-' for all else
#define SWITCHCHAR (Std::switchChar())

  // viewport stuff moved to StdWnd

  // like a STRCHR, but from the end towards the front, and with a string of
  // characters searched for instead of one. include a defval
  const char *scanstr_back(const char *match, const char *str, const char *defval);

  // retrieves extension of a given filename
  const char *extension(const char *fn);

  // retrieves filename from a given path+filename
  const char *filename(const char *fn);

  int getCurDir(char *str, int maxlen);
  int setCurDir(const char *str);

  // regexp match functions

  // A match means the entire string TEXT is used up in matching.
  // In the pattern string:
  //      `*' matches any sequence of characters (zero or more)
  //      `?' matches any character
  //      [SET] matches any character in the specified set,
  //      [!SET] or [^SET] matches any character not in the specified set.

  // A set is composed of characters or ranges; a range looks like
  // character hyphen character (as in 0-9 or A-Z).  [0-9a-zA-Z_] is the
  // minimal set of characters allowed in the [..] pattern construct.
  // Other characters are allowed (ie. 8 bit characters) if your system
  // will support them.

  // To suppress the special syntactic significance of any of `[]*?!^-\',
  // and match the character exactly, precede it with a `\'.

  enum {
    MATCH_VALID=1,  /* valid match */
    MATCH_END,      /* premature end of pattern string */
    MATCH_ABORT,    /* premature end of text string */
    MATCH_RANGE,    /* match failure on [..] construct */
    MATCH_LITERAL,  /* match failure on literal match */
    MATCH_PATTERN,  /* bad pattern */
  };

  enum {
    PATTERN_VALID=0,  /* valid pattern */
    PATTERN_ESC=-1,   /* literal escape at end of pattern */
    PATTERN_RANGE=-2, /* malformed range in [..] construct */
    PATTERN_CLOSE=-3, /* no end bracket in [..] construct */
    PATTERN_EMPTY=-4, /* [..] contstruct is empty */
  };

  // return TRUE if PATTERN has any special wildcard characters
  BOOL isMatchPattern(const char *p);

  // return TRUE if PATTERN has is a well formed regular expression
  BOOL isValidMatchPattern(const char *p, int *error_type);

  // return MATCH_VALID if pattern matches, or an errorcode otherwise
  int matche(register const char *p, register const char *t);
  int matche_after_star(register const char *p, register const char *t);

  // return TRUE if pattern matches, FALSE otherwise.
  BOOL match(const char *p, const char *t);

//FUCKO: these need to move to StdFile
  // gets the system temporary path.
  void getTempPath(int bufsize, char *dst);

  // gets the system font path.
  void getFontPath(int bufsize, char *dst);

  // gets the filename of a font file guaranteed to be in the system font path.
  void getDefaultFont(int bufsize, char *dst);

  // sets a new default font, ie "blah.ttf"
  void setDefaultFont(const char *newdefaultfont);

  // gets the default font scale
  int getDefaultFontScale();

  // sets the new default font scale
  void setDefaultFontScale(int scale);

  // creates a directory. returns 0 on error, nonzero on success
  int createDirectory(const char *dirname);

  // gets informations about a given filename. returns 0 if file not found
  typedef struct {
  unsigned int fileSizeHigh;
  unsigned int fileSizeLow;
  stdtimeval lastWriteTime;
  int readonly;
  } fileInfoStruct;

  int getFileInfos(const char *filename, fileInfoStruct *infos);

  // removes a directory
  void removeDirectory(const char *buf, int recurse);

  // checks if a file exists
  int fileExists(const char *filename);

  // checks if file is a directory
  int isDirectory(const char *filename);

  void shellExec(const char *cmd, const char *params=NULL, bool hide=false);

  int isLittleEndian();

  // unicode & internationalization support.
  int encodingSupportedByOS(const FOURCC enc);

  void getUserName(char *name, int bufsize);

  int isDebuggerPresent();

#ifdef WASABI_PLATFORM_WIN32
  int isWinNT();
#endif
};

#endif

#endif
