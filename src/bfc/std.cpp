#include <precomp.h>
#include "std.h"
#include <wasabicfg.h>
#include <parse/pathparse.h>

#if !defined(WIN32) && !defined(LINUX)
#error port me!
#endif

#ifdef LINUX
#include <unistd.h>
#endif

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN

#include <shlobj.h>
#include <multimon.h>
#if !defined(HMONITOR_DECLARED) && (WINVER < 0x500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif

#include <objbase.h>
#include <shellapi.h>  // for ShellExecute
#include <math.h>
#include <mbctype.h>

#include <sys/stat.h>

#endif  // WIN32

#include <bfc/assert.h>

#include <bfc/ptrlist.h>

#if UTF8
#include <bfc/string/encodedstr.h>
#endif

#include <time.h>  // CUT if possible

static int rand_inited;
static double divider=0.;

namespace {
class StdInit {
public:
  StdInit() {
#ifdef WIN32
    LARGE_INTEGER ll;
    QueryPerformanceFrequency(&ll);
    divider = (double)ll.QuadPart;
#endif
  }
};
static StdInit feh;	// just make sure the constructor runs
};

int Std::keyDown(int code) {
#ifdef WIN32
  return !!(GetKeyState(code) & 0x8000);
#elif defined(LINUX) && !defined(WASABI_PLATFORM_DARWIN)
#ifdef WASABI_PLATFORM_XWINDOW
  if ( code == MK_RBUTTON || code == MK_LBUTTON || code == MK_MBUTTON) {
    Window t1, t2;
    int rx, ry, wx, wy;
    unsigned int buttons;

    XQueryPointer( XWindow::getDisplay(), XWindow::RootWin(), &t1, &t2,
       &rx, &ry, &wx, &wy, &buttons );

    if ( code == MK_MBUTTON )
      return buttons & Button2Mask;
    if ( code == MK_RBUTTON )
      return buttons & Button3Mask;
    //else
    return buttons & Button1Mask;
  }

  int code1 = XKeysymToKeycode( XWindow::getDisplay(), code & 0xFFFF );
  int code2 = XKeysymToKeycode( XWindow::getDisplay(), (code>>16) & 0xFFFF );

  char keys_return[32];
  XQueryKeymap( XWindow::getDisplay(), keys_return );

  if ( code1 && code2 )
    return (keys_return[ (code1 >> 3) & 31 ] & (1 << (code1 & 7))) ||
      (keys_return[ (code2 >> 3) & 31 ] & (1 << (code2 & 7)));

  return (keys_return[ (code1 >> 3) & 31 ] & (1 << (code1 & 7)));
#else //wnd
  DebugString("WARNING: Std::keyDown() requires WASABI_PLATFORM_XWINDOW in Linux\n");
  return FALSE;
#endif //wnd
#elif defined(WASABI_PLATFORM_DARWIN)
  DebugString("need to port Std::KeyDown for Darwin");
  return FALSE;
#else
#error port me!
#endif
}

int Std::pointInRect(RECT *r, int x, int y) {
  ASSERT(r != NULL);
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  if (x < r->left ||
      x >= r->right ||
      y < r->top ||
      y >= r->bottom) return 0;
  return 1;
#elif defined(XWINDOW)
  if(x < r->x ||
     x >= r->x + r->width ||
     y < r->y ||
     y >= r->y + r->width) return 0;
  return 1;
#else
#error port me!
#endif
}
int Std::pointInRect(RECT *r, POINT &p) {
  return pointInRect(r, p.x, p.y);
}

void Std::setRect(RECT *r, int left, int top, int right, int bottom) {
  ASSERT(r != NULL);
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r->left = left;
  r->top = top;
  r->right = right;
  r->bottom = bottom;
#elif defined(XWINDOW)
  r->x = left;
  r->y = top;
  r->width = right - left;
  r->height = bottom - top;
#else
#error port me!
#endif
}

void Std::setRectWH(RECT *r, int x, int y, unsigned int width, unsigned int height) {
  ASSERT(r != NULL);
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r->left = x;
  r->top = y;
  r->right = x + width;
  r->bottom = y + height;
#elif defined(XWINDOW)
  r->x = x;
  r->y = y;
  r->width = width;
  r->height = height;
#else
#error port me!
#endif
}

void Std::expandRect(RECT *r, int horizontal_delta, int vertical_delta) {
  ASSERT(r != NULL);
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r->left -= horizontal_delta;
  r->right += horizontal_delta;
  r->top -= vertical_delta;
  r->bottom += vertical_delta;
#elif defined(XWINDOW)
  r->x -= horizontal_delta;
  r->y -= vertical_delta;
  r->width += (horizontal_delta << 1);
  r->height += (vertical_delta << 1);
#else
#error port me!
#endif
}

void Std::maxRect(RECT *result, RECT *r1, RECT *r2) {
  if (!result || !r1 || !r2) return;
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  result->left = MIN(r1->left, r2->left);
  result->top = MIN(r1->top, r2->top);
  result->right = MAX(r1->right, r2->right);
  result->bottom = MAX(r1->bottom, r2->bottom);
#elif defined(XWINDOW)
  result->x = MIN(r1->x, r2->x);
  result->x = MIN(r1->y, r2->y);
  result->width = MAX(r1->x+r1->width, r2->x+r2->width) - result->x;
  result->height = MAX(r1->y+r1.height, r2->y+r2->height) - result->y;
#else
#error port me!
#endif
}

RECT Std::makeRect(int left, int top, int right, int bottom) {
  RECT r;
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r.left = left;
  r.top = top;
  r.right = right;
  r.bottom = bottom;
#elif defined(XWINDOW)
  r.x = left;
  r.y = top;
  r.width = right - left;
  r.height = bottom - top;
#else
#error port me!
#endif
  return r;
}

RECT Std::makeRectWH(int x, int y, unsigned int width, unsigned int height) {
  //ASSERT(r != NULL);
  RECT r;
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r.left = x;
  r.top = y;
  r.right = x + width;
  r.bottom = y + height;
#elif defined(XWINDOW)
  r.x = x;
  r.y = y;
  r.width = width;
  r.height = height;
#else
#error port me!
#endif
  return r;
}

POINT Std::makePoint(int x, int y) {
  POINT p = { x, y };
  return p;
}

void Std::offsetRect(RECT *r, int x, int y) {
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  r->left += x;
  r->right += x;
  r->top += y;
  r->bottom += y;
#elif defined(XWINDOW)
  r->x += x;
  r->y += y;
#else
#error port me!
#endif
}

void Std::multiplyRect(RECT *r, float v) {
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  //lengthen the math to suppress warnings and "do things properly" :S
  r->left = (LONG)(r->left * v);
  r->right = (LONG)(r->right * v);
  r->top = (LONG)(r->top * v);
  r->bottom = (LONG)(r->bottom * v);
#elif defined(XWINDOW)
  r->x *= v;
  r->y *= v;
  r->width *= v;
  r->height *= v;
#else
#error port me!
#endif
}

int Std::rectEqual(const RECT &a, const RECT &b) {
  return !MEMCMP(&a, &b, sizeof(RECT));
}

int Std::rectEqual(const RECT *a, const RECT *b) {
  return !MEMCMP(a, b, sizeof(RECT));
}

int Std::rectIntersect(const RECT &a, const RECT &b, RECT *intersection) {
  return rectIntersect(&a, &b, intersection);
}

int Std::rectIntersect(const RECT *i1, const RECT *i2, RECT *intersection) {
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  RECT out;
  int r = IntersectRect(&out, i1, i2);
  if (intersection) *intersection = out;
  return r;
#elif defined(XWINDOW)
  RECT out;
  out.x = MAX(i1->x, i2->y);
  out.y = MAX(i1->y, i2->y);
  out.width  = /*right*/MIN( i1->x + i1->width,  i2->x + i2->width)/**/  - out.x;
  out.height = /*bottom*/MIN(i1->y + i1->height, i2->y + i2->height)/**/ - out.y;

  if (!( out.x < out.x+out.width && out.top < out.y + out.height )) {
    out.left = out.top = out.width = out.height = 0;
    if (intersection != NULL) *intersection = out;
    return 0;
  } else {
    if (intersection != NULL) *intersection = out;
    return 1;
  }
#else
#error port me!
#endif
}

int Std::rectSubtract(const RECT &a, const RECT &b, RECT *a_minus_b) {
  return rectSubtract(&a, &b, a_minus_b);
}

int Std::rectSubtract(const RECT *a, const RECT *b, RECT *a_minus_b) {
#if defined(WIN32) || defined(WASABI_WIN32RECT)
  return SubtractRect(a_minus_b, a, b);
#elif defined(XWINDOW)
  // Copied and adapted from WINE sources 25 Jun 2005, liscensed under GNU LGPL 2.1.
  //  From wine/dlls/user/uitools.c (see http://source.winehq.org/source/dlls/user/uitools.c#L1547)
  //  Copyright 1997 Dimitrie O. Paun
  //  Copyright 1997 Bertho A. Stultiens
  RECT tmp;

  if (!a_minus_b) return FALSE;
  if (!a || a->width == 0 || a->height == 0 /*IsRectEmpty( a )*/)
  {
    /*SetRectEmpty( a_minus_b );*/
    a_minus_b->width = a_minus_b->height = a_minus_b->x = a_minus_b->y = 0;
    return FALSE;
  }
  *a_minus_b = *a;
  if (rectIntersect( a, b, &tmp ))
  {
    if (rectEqual( &tmp, a_minus_b ))
    {
      /*SetRectEmpty( a_minus_b );*/
      a_minus_b->width = a_minus_b->height = a_minus_b->x = a_minus_b->y = 0;
      return FALSE;
    }
    if ((tmp.y == a_minus_b->y) && (tmp.y + tmp.height == a_minus_b->y + a_minus_b->height))
    {
      if (tmp.x == a_minus_b->x) a_minus_b->x = tmp.x + tmp.width;
      else if (tmp.x + tmp.width == a_minus_b->x + a_minus_b->width) a_minus_b->x + a_minus_b->width = tmp.x;
    }
    else if ((tmp.x == a_minus_b->x) && (tmp.x + tmp.width == a_minus_b->x + a_minus_b->width))
    {
      if (tmp.y == a_minus_b->y) a_minus_b->y = tmp.y + tmp.height;
      else if (tmp.y + tmp.height == a_minus_b->y + a_minus_b->height) a_minus_b->y + a_minus_b->height = tmp.y;
    }
  }
  return TRUE;
#else
#error port me!
#endif
}

void Std::srandom(unsigned int key) {
  if (key == 0) key = (unsigned int)Std::getTimeStamp();
  ::srand(key);
}

int Std::random(int max) {
//  ASSERT(max <= RAND_MAX);
  if (!rand_inited++) srandom();
  int ret = ::rand();
  if (max != RAND_MAX+1) ret %= max;
  return ret;
}

unsigned int Std::random32(unsigned int max) {
  if (max <= 1) return 0;
  if (!rand_inited++) Std::srandom();
  unsigned int val=0;
  for (int i = 0; i < sizeof(unsigned int); i++) {
    val <<= 8;
    val |= ((::rand()>>2) & 0xff);
  }
  if (max != 0xffffffff) val %= max;
  return val;
}

void Std::sleep(int s) {
#if defined(WASABI_PLATFORM_POSIX)
  ::sleep(s);
#else // default
  Std::usleep(s * 1000);
#endif // platform
}

void Std::usleep(int ms) { // ... yah, Std::usleep() is a misnomer.
#if defined(WIN32)
  Sleep(ms);
#elif defined(WASABI_PLATFORM_POSIX)
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
#else
#error port me!
#endif // platform
}

stdtimeval Std::getTimeStamp() {
  return (stdtimeval)time(NULL);
}

stdtimevalf Std::getTimeStampMS() {
#ifdef WIN32
  LARGE_INTEGER ll;
  if (!QueryPerformanceCounter(&ll)) return getTickCount() / 1000.f;
  stdtimevalms ret = (stdtimevalms)ll.QuadPart;
  return ret /= divider;
#else
  return getTickCount() / 1000.f;
#endif
}

DWORD Std::getTickCount() {
  return GetTickCount();
//INLINE
}

void Std::tolowerString(char *str) {
  while (*str++) *str = TOLOWER(*str);
}

#ifdef WASABI_COMPILE_WND

void Std::ensureVisible(RECT *r) {
  RECT sr;
  POINT p={(r->right+r->left)/2,(r->bottom+r->top)/2};
  StdWnd::getViewport(&sr,&p);
  int w = r->right-r->left;
  int h = r->bottom-r->top;
  if (r->bottom > sr.bottom) {
    r->bottom = sr.bottom;
    r->top = r->bottom-h;
  }
  if (r->right > sr.right) {
    r->right = sr.right;
    r->left = r->right-w;
  }
  if (r->left < sr.left) {
    r->left = sr.left;
    r->right = r->left+w;
  }
  if (r->top < sr.top) {
    r->top = sr.top;
    r->bottom = r->top+h;
  }
}

//deprecatedint Std::getScreenWidth() {
//deprecated#ifdef WIN32
//deprecated  RECT r;
//deprecated  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
//deprecated  return r.right-r.left;
//deprecated#else
//deprecated  return DisplayWidth(XWindow::getDisplay(), XWindow::getScreenNum());
//deprecated#endif
//deprecated}

//deprecatedint Std::getScreenHeight() {
//deprecated#ifdef WIN32
//deprecated  RECT r;
//deprecated  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
//deprecated  return r.bottom-r.top;
//deprecated#else
//deprecated  return DisplayHeight( XWindow::getDisplay(), XWindow::getScreenNum() );
//deprecated#endif
//deprecated}

#endif

int Std::dirChar() {
#ifdef WIN32
  return '\\';
#else
  return '/';
#endif
}

const char *Std::dirCharStr() {
#ifdef WIN32
  return "\\";
#else
  return "/";
#endif
};

int Std::isDirChar(int thechar, int allow_multiple_platforms) {
  if (thechar == DIRCHAR) return 1;
  if (allow_multiple_platforms) {
#ifdef WIN32
    if (thechar == '/') return 1;
#else
    if (thechar == '\\') return 1;
#endif
  }
  return 0;
}

const char *Std::matchAllFiles() {
#ifdef WIN32
  return "*.*";
#else
  return "*";
#endif
}

const char *Std::dotDir() {
  return ".";
}

const char *Std::dotDotDir() {
  return "..";
}

int Std::isRootPath(const char *path) {
  if (path == NULL || path[0] == '\0') return 0;
  if (isDirChar(path[0])) return 1;
#ifdef WIN32
  if (ISALPHA(path[0]) && path[1] == ':' && isDirChar(path[2])) return 1;
#endif
  return 0;
}

int Std::switchChar() {
#ifdef WIN32
  return '/';
#else
  return '-';
#endif
}

const char *Std::scanstr_back(const char *str, const char *toscan, const char *defval) {
  int strl = STRLEN(str);
  if (strl < 1) return defval;
  const char *s=str+strl-1;
  if (STRLEN(toscan) < 1) return defval;
  while (1) {
    const char *t=toscan;
    while (*t) if (*t++ == *s) return s;
    t=CharPrev(str,s);
    if (t==s) return defval;
    s=t;
  }
}

const char *Std::extension(const char *fn) {
  if (fn == NULL) return NULL;
  const char *s = scanstr_back(fn,".\\/",fn-1);
  if (s < fn) return "";
  if (*s == '\\' || *s == '/') return "";
  return (s+1);
}

const char *Std::filename(const char *fn) {
  const char *s=scanstr_back(fn,"\\/",0);
  if(!s) return fn;
  return (s+1);
}

BOOL Std::isMatchPattern(const char *p) {
  while (*p) {
    switch (*p++) {
      case '?':
      case '*':
      case '[':
      case '\\':
        return TRUE;
    }
  }
  return FALSE;
}

BOOL Std::isValidMatchPattern(const char *p, int *error_type) {
  /* init error_type */
  *error_type = PATTERN_VALID;
  /* loop through pattern to EOS */
  while (*p) {
    /* determine pattern type */
    switch (*p) {
      /* check literal escape, it cannot be at end of pattern */
      case '\\':
        if (!*++p) {
          *error_type = PATTERN_ESC;
          return FALSE;
        }
        p++;
        break;
      /* the [..] construct must be well formed */
      case '[':
        p++;
        /* if the next character is ']' then bad pattern */
        if (*p == ']') {
          *error_type = PATTERN_EMPTY;
          return FALSE;
        }
        /* if end of pattern here then bad pattern */
        if (!*p) {
          *error_type = PATTERN_CLOSE;
          return FALSE;
        }
        /* loop to end of [..] construct */
        while (*p != ']') {
          /* check for literal escape */
          if (*p == '\\') {
            p++;
            /* if end of pattern here then bad pattern */
            if (!*p++) {
              *error_type = PATTERN_ESC;
              return FALSE;
            }
          } else p++;
        /* if end of pattern here then bad pattern */
        if (!*p) {
          *error_type = PATTERN_CLOSE;
          return FALSE;
        }
        /* if this a range */
        if (*p == '-') {
          /* we must have an end of range */
          if (!*++p || *p == ']') {
            *error_type = PATTERN_RANGE;
            return FALSE;
          } else {
            /* check for literal escape */
            if (*p == '\\') p++;
            /* if end of pattern here then bad pattern */
            if (!*p++) {
              *error_type = PATTERN_ESC;
              return FALSE;
            }
          }
        }
      }
      break;
      /* all other characters are valid pattern elements */
      case '*':
      case '?':
      default:
        p++; /* "normal" character */
        break;
    }
  }
  return TRUE;
}

int Std::matche(register const char *p, register const char *t) {
  register char range_start, range_end;  /* start and end in range */

  BOOLEAN invert;             /* is this [..] or [!..] */
  BOOLEAN member_match;       /* have I matched the [..] construct? */
  BOOLEAN loop;               /* should I terminate? */

  if(!p) p = "";
  if(!t) t = "";

  for ( ; *p; p++, t++) {
    /* if this is the end of the text then this is the end of the match */
    if (!*t) {
      return ( *p == '*' && *++p == '\0' ) ? MATCH_VALID : MATCH_ABORT;
    }
    /* determine and react to pattern type */
    switch (*p) {
      case '?': /* single any character match */
        break;
      case '*': /* multiple any character match */
        return matche_after_star (p, t);

      /* [..] construct, single member/exclusion character match */
      case '[': {
        /* move to beginning of range */
        p++;
        /* check if this is a member match or exclusion match */
        invert = FALSE;
        if (*p == '!' || *p == '^') {
          invert = TRUE;
          p++;
        }
        /* if closing bracket here or at range start then we have a malformed pattern */
        if (*p == ']') return MATCH_PATTERN;

        member_match = FALSE;
        loop = TRUE;
        while (loop) {
          /* if end of construct then loop is done */
          if (*p == ']') {
            loop = FALSE;
            continue;
          }
          /* matching a '!', '^', '-', '\' or a ']' */
          if (*p == '\\') range_start = range_end = *++p;
          else  range_start = range_end = *p;
          /* if end of pattern then bad pattern (Missing ']') */
          if (!*p) return MATCH_PATTERN;
          /* check for range bar */
          if (*++p == '-') {
            /* get the range end */
            range_end = *++p;
            /* if end of pattern or construct then bad pattern */
            if (range_end == '\0' || range_end == ']') return MATCH_PATTERN;
            /* special character range end */
            if (range_end == '\\') {
              range_end = *++p;
              /* if end of text then we have a bad pattern */
              if (!range_end) return MATCH_PATTERN;
            }
            /* move just beyond this range */
            p++;
          }
          /* if the text character is in range then match found.
             make sure the range letters have the proper
             relationship to one another before comparison */
          if (range_start < range_end) {
            if (*t >= range_start && *t <= range_end) {
              member_match = TRUE;
              loop = FALSE;
            }
          } else {
            if (*t >= range_end && *t <= range_start) {
              member_match = TRUE;
              loop = FALSE;
            }
          }
        }
        /* if there was a match in an exclusion set then no match */
        /* if there was no match in a member set then no match */
        if ((invert && member_match) || !(invert || member_match)) return MATCH_RANGE;
        /* if this is not an exclusion then skip the rest of the [...] construct that already matched. */
        if (member_match) {
          while (*p != ']') {
            /* bad pattern (Missing ']') */
            if (!*p) return MATCH_PATTERN;
            /* skip exact match */
            if (*p == '\\') {
              p++;
              /* if end of text then we have a bad pattern */
              if (!*p) return MATCH_PATTERN;
            }
            /* move to next pattern char */
            p++;
          }
        }
        break;
      }
      case '\\':  /* next character is quoted and must match exactly */
        /* move pattern pointer to quoted char and fall through */
        p++;
        /* if end of text then we have a bad pattern */
        if (!*p) return MATCH_PATTERN;
        /* must match this character exactly */
      default:
        if (toupper(*p) != toupper(*t)) return MATCH_LITERAL;
    }
  }
  /* if end of text not reached then the pattern fails */
  if (*t) return MATCH_END;
  else return MATCH_VALID;
}

int Std::matche_after_star(register const char *p, register const char *t) {
  register int match = 0;
  register char nextp;
  /* pass over existing ? and * in pattern */
  while ( *p == '?' || *p == '*' ) {
    /* take one char for each ? and + */
    if (*p == '?') {
      /* if end of text then no match */
      if (!*t++) return MATCH_ABORT;
    }
    /* move to next char in pattern */
    p++;
  }
  /* if end of pattern we have matched regardless of text left */
  if (!*p) return MATCH_VALID;
  /* get the next character to match which must be a literal or '[' */
  nextp = *p;
  if (nextp == '\\') {
    nextp = p[1];
    /* if end of text then we have a bad pattern */
    if (!nextp) return MATCH_PATTERN;
  }
  /* Continue until we run out of text or definite result seen */
  do {
    /* a precondition for matching is that the next character
       in the pattern match the next character in the text or that
       the next pattern char is the beginning of a range.  Increment
       text pointer as we go here */
    if (tolower(nextp) == tolower(*t) || nextp == '[') match = matche(p, t);
    /* if the end of text is reached then no match */
    if (!*t++) match = MATCH_ABORT;
  } while ( match != MATCH_VALID && match != MATCH_ABORT && match != MATCH_PATTERN);
  /* return result */
  return match;
}

BOOL Std::match(const char *p, const char *t) {
  int error_type;
  error_type = matche(p,t);
  return (error_type == MATCH_VALID ) ? TRUE : FALSE;
}

#ifndef _NOSTUDIO

int Std::getCurDir(char *str, int maxlen) {
  ASSERT(str != NULL);
#ifdef WIN32
  int retval = 0;
#if UTF8

  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    wchar_t buf16[WA_MAX_PATH];
    retval = GetCurrentDirectoryW(WA_MAX_PATH, buf16);
    if (retval) {
      String buf8;
      EncodedStr buf16enc(SvcStrCnv::UTF16, buf16, retval+1, 0); // retval is size without nullchar, we must give size WITH nullchar.
      retval = buf16enc.convertToUTF8(buf8);
      if (retval > 0) {
        STRNCPY(str, buf8, maxlen);
      } else if (retval == SvcStrCnv::ERROR_UNAVAILABLE) {
        retval = GetCurrentDirectory(maxlen, str);
      } else {
        retval = 0;
        str[0]=0;
      }
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    char bufOS[WA_MAX_PATH];
    retval = GetCurrentDirectory(WA_MAX_PATH, bufOS);
    if (retval) {
      String buf8;
      EncodedStr bufOSenc(SvcStrCnv::OSNATIVE, bufOS, retval+1, 0); // retval is size without nullchar, we must give size WITH nullchar.
      retval = bufOSenc.convertToUTF8(buf8);
      if (retval > 0) {
        STRNCPY(str, buf8, maxlen);
      } else if (retval == SvcStrCnv::ERROR_UNAVAILABLE) {
        retval = GetCurrentDirectory(maxlen, str);
      } else {
        retval = 0;
        str[0]=0;
      }
    }
  }

#else // The code that was there before.
  retval = GetCurrentDirectory(maxlen, str);
#endif

  return retval;

#else
  return getcwd( str, maxlen ) != NULL;
#endif
}

int Std::setCurDir(const char *str) {
  ASSERT(str != NULL);
#ifdef WIN32
  int retval = 0;
#if UTF8
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr dname16enc;
    int check = dname16enc.convertFromUTF8(SvcStrCnv::UTF16, String(str));
    if (check > 0) {
      wchar_t *dname16 = static_cast<wchar_t *>(dname16enc.getEncodedBuffer());
      retval = SetCurrentDirectoryW(dname16);
    } else if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
      retval = SetCurrentDirectory(str);
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr dnameOSenc;
    int check = dnameOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(str));
    if (check > 0) {
      char *dnameOS = static_cast<char *>(dnameOSenc.getEncodedBuffer());
      retval = SetCurrentDirectory(dnameOS);
    } else if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
      retval = SetCurrentDirectory(str);
    }
  }

#else // The code that was there before.
  retval = SetCurrentDirectory(str);
#endif

  return retval;

#else
  return chdir( str );
#endif
}

String Std::getLastErrorString(int _err) {
  if (_err == -1) {
#ifdef WIN32
    _err = GetLastError();
#else
//CUT #ifdef PLATFORM_WARNINGS
//CUT #warning Std::getLastErrorString not implemented on LINUX
//CUT #endif
   _err = errno;
#endif
  }
  String ret; // need this allocated later to avoid String construction changing errno.
#ifdef WIN32
  char *sysbuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, _err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&sysbuf, 0, NULL);
  ret = sysbuf;
  LocalFree(sysbuf);
#else
  ret = strerror(_err);
#endif
  return ret;
}

void Std::getTempPath(int bufsize, char *dst) {
  ASSERT(dst != NULL);
#ifdef WIN32
#if UTF8

  int failed = 0;

  String tempPath8;
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    wchar_t tempPath16[WA_MAX_PATH];
    GetTempPathW(WA_MAX_PATH, tempPath16);
    // Convert
    EncodedStr tempPath16enc(SvcStrCnv::UTF16, tempPath16, WSTRLEN(tempPath16) + 1, 0); // you must give the size PLUS the null term if any.
    failed = tempPath16enc.convertToUTF8(tempPath8);
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    char tempPathOS[WA_MAX_PATH];
    GetTempPath(WA_MAX_PATH, tempPathOS);
    // Convert
    EncodedStr tempPathOSenc(SvcStrCnv::OSNATIVE, tempPathOS, STRLEN(tempPathOS) + 1, 0); // you must give the size PLUS the null term if any.
    failed = tempPathOSenc.convertToUTF8(tempPath8);
  }

  if (failed == SvcStrCnv::ERROR_UNAVAILABLE) {
    GetTempPath(bufsize,dst);
  } else if (tempPath8.len()) {
    // Copy to the user buf.
    STRNCPY(dst, tempPath8, bufsize);
  } else {
    // ??? what do we do?  Null the string?  I guess.
    dst[0] = 0;
  }

#else // The code that was there before.
  GetTempPath(bufsize,dst);
#endif // UTF8

#elif defined(WASABI_PLATFORM_POSIX)
  char *tmpdir;
  struct stat st;

  if ( !(tmpdir = getenv( "TMPDIR" )) || stat( tmpdir, &st ) != 0 ) {
    tmpdir = P_tmpdir;
    if ( stat( tmpdir, &st ) != 0 )
      tmpdir = "/tmp";
  }

  STRNCPY( dst, tmpdir, bufsize-1 ); // BU -1 to account for DIRCHARSTR
  STRCAT( dst, DIRCHARSTR );
#endif // platform
}

// gets the system font path
void Std::getFontPath(int bufsize, char *dst) {
  ASSERT(dst != NULL);
#ifdef WIN32

  int (__stdcall *_SHGetSpecialFolderPath) (HWND hwndOwner, LPSTR lpszPath, int nFolder, int fCreate);

  HINSTANCE shell = LoadLibrary("shell32.dll");
  _SHGetSpecialFolderPath = (int (__stdcall *)(HWND, LPSTR, int, int))GetProcAddress(shell, "SHGetSpecialFolderPathA");
  if (!_SHGetSpecialFolderPath) _SHGetSpecialFolderPath = (int (__stdcall *)(HWND, LPSTR, int, int))GetProcAddress(shell, "SHGetSpecialFolderPath");

  int addfontdir = 0;

#if UTF8
  int (__stdcall *_SHGetSpecialFolderPathW) (HWND hwndOwner, LPWSTR lpszPath, int nFolder, int fCreate);
  _SHGetSpecialFolderPathW = (int (__stdcall *)(HWND, LPWSTR, int, int))GetProcAddress(shell, "SHGetSpecialFolderPathW");

#pragma CHAT("deadbeef", "all", "UTF getfontdir is borked on multiuser systems, needs updating to be like non-UTF mode")

  int failed = 0;

  String winPath8;
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    wchar_t winPath16[WA_MAX_PATH];
    if (_SHGetSpecialFolderPathW) _SHGetSpecialFolderPathW(NULL, winPath16, CSIDL_FONTS, FALSE);
    else {
      GetWindowsDirectoryW(winPath16, WA_MAX_PATH);
      addfontdir=1;
    }
    // Convert
    EncodedStr winPath16enc(SvcStrCnv::UTF16, winPath16, WSTRLEN(winPath16) + 1, 0); // you must give the size PLUS the null term if any.
    failed = winPath16enc.convertToUTF8(winPath8);
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    char winPathOS[WA_MAX_PATH];
    if (_SHGetSpecialFolderPath) _SHGetSpecialFolderPath(NULL, winPathOS, CSIDL_FONTS, FALSE);
    else {
      GetWindowsDirectory(winPathOS, WA_MAX_PATH);
      addfontdir=1;
    }
    // Convert
    EncodedStr winPathOSenc(SvcStrCnv::OSNATIVE, winPathOS, STRLEN(winPathOS) + 1, 0); // you must give the size PLUS the null term if any.
    failed = winPathOSenc.convertToUTF8(winPath8);
  }

  if (failed == SvcStrCnv::ERROR_UNAVAILABLE || winPath8.len() == 0) {
    char path[8192]="";
    if (_SHGetSpecialFolderPath) _SHGetSpecialFolderPath(NULL, path, CSIDL_FONTS, FALSE);
    else {
      GetWindowsDirectory(path, 8192);
      addfontdir=1;
    }
    STRNCPY(dst, path, bufsize);
  } else {
    // Copy to the user buf.
    STRNCPY(dst, winPath8, bufsize);
  }
#else // The code that was there before.
  char path[8192]="";
  if (_SHGetSpecialFolderPath) _SHGetSpecialFolderPath(NULL, path, CSIDL_FONTS, FALSE);
  else {
    int suc=0;
    HINSTANCE hKern=LoadLibrary("kernel32.dll"); // JF> on multiuser TS systems, we can't use GetWindowsDirectory for this
    if (hKern)
    {
      UINT (__stdcall *_gswd)(LPTSTR lpBuffer, UINT uSize);
      *(void **)&_gswd=(void*)GetProcAddress(hKern,"GetSystemWindowsDirectoryA");
      if (_gswd)
      {
        if (_gswd(path,8192))
        {
          suc=1;
        }
      }
      FreeLibrary(hKern);
    }

    if (!suc) GetWindowsDirectory(path, 8192);

    addfontdir=1;
  }
  STRNCPY(dst, path, bufsize);
#endif

  FreeLibrary(shell);

  if (addfontdir) {
    String tmp = dst;
    if (!isDirChar(tmp.lastChar())) tmp += "\\";
    tmp += "fonts";
    STRNCPY(dst, tmp, bufsize);
  }

#else
#ifdef PLATFORM_WARNINGS
#warning Std::getFontPath not implemented on LINUX
#endif
#endif

}

int default_font_scale = 100;
char default_font[256] =
#ifdef WIN32
WASABI_DEFAULT_FONTNAME ".ttf";
#else
  // even tho this isn't the way we'll port this, the style is fun.
"-*-arial-medium-r-*--10-*-*-*-*-*-*-*";
#endif

// gets the filename of a font file guaranteed to be in the system font path.
void Std::getDefaultFont(int bufsize, char *dst) {
  ASSERT(dst != NULL);
  const char *output = default_font;
  STRNCPY(dst, output, bufsize);
}

void Std::setDefaultFont(const char *newdefaultfont) {
  STRNCPY(default_font, newdefaultfont, 255); default_font[255] = 0;
}

int Std::getDefaultFontScale() {
  return default_font_scale;
}

void Std::setDefaultFontScale(int scale) {
  default_font_scale = scale;
}

int Std::createDirectory(const char *dirname) {
#ifdef WIN32
  if(!CreateDirectory(dirname,NULL))
#elif defined(WASABI_PLATFORM_POSIX)
  if(mkdir(dirname, 0755))
#else
#error port me!
#endif // platform
  {
    // create all the path
    PathParser pp(dirname);
    int l = pp.getNumStrings();
    for(int i=2;i<=l;i++) {
      String dir;
	  for(int j=0;j<i;j++) {
        dir += pp.enumString(j);
		dir += DIRCHARSTR;
	  }
#ifdef WIN32
      CreateDirectory(dir,NULL);
#elif defined(WASABI_PLATFORM_POSIX)
      mkdir(dir, 0755);
#else
#error port me!
#endif // platform
    }
  }
  return 1;
}

int Std::getFileInfos(const char *filename, fileInfoStruct *infos) {
  int retval = 0;
#ifdef WIN32

#if UTF8
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr path16enc;
    int retcode = path16enc.convertFromUTF8(SvcStrCnv::UTF16, String(filename));
    WCHAR *path16 = static_cast<WCHAR *>(path16enc.getEncodedBuffer());
    if (path16 != NULL) {
      struct _stati64 statbuf;
      if (_wstati64(path16, &statbuf) == -1) return 0;
      infos->lastWriteTime = (stdtimeval)statbuf.st_mtime;
      infos->fileSizeLow = (int)(statbuf.st_size);
      infos->fileSizeHigh = (int)(statbuf.st_size >> 32);
      infos->readonly = !(statbuf.st_mode & _S_IWRITE);
    } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      struct _stati64 statbuf;
      if (_stati64(filename, &statbuf) == -1) return 0;
      infos->lastWriteTime = (stdtimeval)statbuf.st_mtime;
      infos->fileSizeLow = (int)(statbuf.st_size);
      infos->fileSizeHigh = (int)(statbuf.st_size >> 32);
      infos->readonly = !(statbuf.st_mode & _S_IWRITE);
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr pathOSenc;
    int retcode = pathOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(filename));
    char *pathOS = static_cast<char *>(pathOSenc.getEncodedBuffer());
    if (pathOS != NULL) {
      struct _stati64 statbuf;
      if (_stati64(pathOS, &statbuf) == -1) return 0;
      infos->lastWriteTime = (stdtimeval)statbuf.st_mtime;
      infos->fileSizeLow = (int)(statbuf.st_size);
      infos->fileSizeHigh = (int)(statbuf.st_size >> 32);
      infos->readonly = !(statbuf.st_mode & _S_IWRITE);
    } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      struct _stati64 statbuf;
      if (_stati64(filename, &statbuf) == -1) return 0;
      infos->lastWriteTime = (stdtimeval)statbuf.st_mtime;
      infos->fileSizeLow = (int)(statbuf.st_size);
      infos->fileSizeHigh = (int)(statbuf.st_size >> 32);
      infos->readonly = !(statbuf.st_mode & _S_IWRITE);
    }
  }
#else
  HANDLE h;
  WIN32_FIND_DATA fd;
  if((h=FindFirstFile(filename, &fd))==INVALID_HANDLE_VALUE) return 0;

  infos->fileSizeHigh=fd.nFileSizeHigh;
  infos->fileSizeLow=fd.nFileSizeLow;
  struct _stati64 statbuf;
  if (_stati64(filename, &statbuf) == -1) return 0;
  infos->lastWriteTime = statbuf.st_mtime;
  infos->readonly = fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
  FindClose(h);
#endif // UTF8
  return 1;
#elif defined(WASABI_PLATFORM_POSIX)
  struct stat st;

  int ret = stat( filename, &st );
  if ( ret != 0 )
    return 0;

  infos->fileSizeHigh = 0;
  infos->fileSizeLow = st.st_size;
  infos->lastWriteTime = st.st_mtime;
  return 1;
#else
#error port me!
#endif // platform
}

void Std::removeDirectory(const char *buf, int recurse) {
#ifdef WIN32
#if UTF8
  int failed = 0;
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr buf16enc;
    int retcode = buf16enc.convertFromUTF8(SvcStrCnv::UTF16, String(buf));
    WCHAR *buf16 = static_cast<WCHAR *>(buf16enc.getEncodedBuffer());
    if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      failed = 1;
    } else {
      if (recurse) {
        WIN32_FIND_DATAW fdW;
        char path[WA_MAX_PATH];
        STRCPY(path,buf);
        char *p=path;
        int c = 0;
        while (*p) {
          p++; c++;
        }
        STRCPY(p,"\\*.*");
        // Yah, whatever.  Okay, here we have path.  Convert it.
        EncodedStr path16encstr;
        path16encstr.convertFromUTF8(SvcStrCnv::UTF16, String(path));
        WCHAR *path16enc = static_cast<WCHAR *>(path16encstr.getEncodedBuffer());
        if (!path16enc) return;
        WCHAR path16[WA_MAX_PATH];
        memcpy(path16,path16enc,WSTRLEN(path16enc)+1);
        HANDLE h;
        h = FindFirstFileW(path16,&fdW);
        if (h != INVALID_HANDLE_VALUE) {
          do {
            if (fdW.cFileName[0] != '.' ||
                (fdW.cFileName[1] != '.' && fdW.cFileName[1])) {
              MEMCPY(path16+c+1, fdW.cFileName, (WSTRLEN(fdW.cFileName)+1)*2);
              if (fdW.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                SetFileAttributesW(path16,fdW.dwFileAttributes^FILE_ATTRIBUTE_READONLY);
              if (fdW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // now we have to go BACK to UTF8.  christ on a popsicle stick.
                EncodedStr next16enc(SvcStrCnv::UTF16, path16, (WSTRLEN(path16)+1)*2, 0/*no delete*/);
                String path8;
                next16enc.convertToUTF8(path8);
                removeDirectory(path8,recurse);
              }
              else DeleteFileW(path16);
            }
          } while (FindNextFileW(h,&fdW));
          FindClose(h);
        }
        p[0]=0; // fix buffer
      }
      RemoveDirectoryW(buf16);
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr bufOSenc;
    int retcode = bufOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(buf));
    char *bufOS = static_cast<char *>(bufOSenc.getEncodedBuffer());
    if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      failed = 1;
    } else {
      if (recurse) {
        WIN32_FIND_DATA fd;
        char path[WA_MAX_PATH];
        STRCPY(path,buf);
        char *p=path;
        int c = 0;
        while (*p) {
          p++; c++;
        }
        STRCPY(p,"\\*.*");
        // Yah, whatever.  Okay, here we have path.  Convert it.
        EncodedStr pathOSencstr;
        pathOSencstr.convertFromUTF8(SvcStrCnv::OSNATIVE, String(path));
        char *pathOSenc = static_cast<char *>(pathOSencstr.getEncodedBuffer());
        if (!pathOSenc) return;
        char pathOS[WA_MAX_PATH];
        strncpy(pathOS,pathOSenc,WA_MAX_PATH);
        pathOS[WA_MAX_PATH-1]=0;
        HANDLE h;
        h = FindFirstFile(pathOS,&fd);
        if (h != INVALID_HANDLE_VALUE) {
          do {
            if (fd.cFileName[0] != '.' ||
                (fd.cFileName[1] != '.' && fd.cFileName[1])) {
              STRCPY(pathOS+c+1, fd.cFileName);
              if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                SetFileAttributes(pathOS,fd.dwFileAttributes^FILE_ATTRIBUTE_READONLY);
              if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // now we have to go BACK to UTF8.  christ on a popsicle stick.
                EncodedStr nextOSenc(SvcStrCnv::OSNATIVE, pathOS, (STRLEN(pathOS)+1), 0/*no delete*/);
                String path8;
                nextOSenc.convertToUTF8(path8);
                removeDirectory(path8,recurse);
              }
              else DeleteFile(pathOS);
            }
          } while (FindNextFile(h,&fd));
          FindClose(h);
        }
        p[0]=0; // fix buffer
      }
      RemoveDirectory(bufOS);
    }
  }
  if (failed) {
    if (recurse) {
      WIN32_FIND_DATA fd;
      char path[WA_MAX_PATH];
      STRCPY(path,buf);
      char *p=path;
      while (*p) p++;
      STRCPY(p,"\\*.*");
      HANDLE h;
      h = FindFirstFile(path,&fd);
      if (h != INVALID_HANDLE_VALUE) {
        do {
          if (fd.cFileName[0] != '.' ||
              (fd.cFileName[1] != '.' && fd.cFileName[1])) {
            STRCPY(p+1,fd.cFileName);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
              SetFileAttributes(path,fd.dwFileAttributes^FILE_ATTRIBUTE_READONLY);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) removeDirectory(path,recurse);
            else DeleteFile(path);
          }
        } while (FindNextFile(h,&fd));
        FindClose(h);
      }
      p[0]=0; // fix buffer
    }
    RemoveDirectory(buf);
  }
#else
  if (recurse) {
    WIN32_FIND_DATA fd;
    char path[WA_MAX_PATH];
    STRCPY(path,buf);
    char *p=path;
    while (*p) p++;
    STRCPY(p,"\\*.*");
    HANDLE h;
    h = FindFirstFile(path,&fd);
    if (h != INVALID_HANDLE_VALUE) {
      do {
        if (fd.cFileName[0] != '.' ||
            (fd.cFileName[1] != '.' && fd.cFileName[1])) {
          STRCPY(p+1,fd.cFileName);
          if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
            SetFileAttributes(path,fd.dwFileAttributes^FILE_ATTRIBUTE_READONLY);
          if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) removeDirectory(path,recurse);
          else DeleteFile(path);
        }
      } while (FindNextFile(h,&fd));
      FindClose(h);
    }
    p[0]=0; // fix buffer
  }
  RemoveDirectory(buf);
#endif
#elif defined(WASABI_PLATFORM_POSIX)
  rmdir( buf );
  if ( recurse ) {
    DebugString( StringPrintf( "portme -- Std::removeDirector( %s, recurse )\n", buf ) );
  }
#else
#error port me!
#endif // platform
}


int Std::fileExists(const char *filename) {
#ifdef WASABI_COMPILE_FILEREADER
  return WASABI_API_FILE->fileExists(filename);
#else
  FILE *f = fopen(filename, "r");
  if(!f) return 0;
  fclose(f);
  return 1;
#endif
}


int Std::isDirectory(const char *filename) {
#ifdef WIN32
#pragma CHAT("bizzeh", "everyone", "Std should be wasabi core, shouldnt depend on the API")
/*#ifdef WASABI_COMPILE_FILEREADER
  if (WASABI_API_FILE != NULL) {
#pragma CHAT("bas", "rOn", "might want to check for dir via readers here")
     return WASABI_API_FILE->fileIsDirectory(filename);
  }
#endif*/
  unsigned int r = GetFileAttributes(filename);
  if (r == 0xffffffff) return 0;
  return !!(r & FILE_ATTRIBUTE_DIRECTORY);
#else
	return StdFile::isDirectory(filename);
#endif
}


void Std::shellExec(const char *cmd, const char *params, bool hide) {
#ifdef WIN32
  ShellExecute(NULL, NULL, cmd, params, ".", hide ? SW_HIDE : SW_SHOWNORMAL);
#else
  system(StringPrintf("%s %s", cmd, params));
#endif
}

int Std::isLittleEndian() {
  return ((*((int*)"\1  "))&1);  // That one was justin's favourite.  I didn't test it.
}


int Std::encodingSupportedByOS(const FOURCC enc) {
#ifdef WIN32
  int retval = 0;
#if UTF8
  // For windows, this is it.
  switch (enc) {
    case SvcStrCnv::OSNATIVE:
      retval = 1; // duh.
    case SvcStrCnv::UTF16: {
      static int staticval = -1;
      if (staticval == -1) {
        // Initialize your capabilities just once, please
        CreateFileW(L"Hello, world",0,0,NULL,0,0,NULL);
        // Yes, we will fail, but why?
        if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
          // Not implemented means we're on 98 (or 98 style API's)
          staticval = 0;
        } else {
          staticval = 1;
        }

      }
      retval = staticval;
    }
    break;
    default:
      retval = 0;
    break;
  }
#endif
  return retval;
#else
//PORTME
  return 0;
#endif
}

#endif // nostudio

extern int stdmem_mmx_check_done;
extern int stdmem_mmx_available;

void MEMFILL32(void *lptr, unsigned long val, unsigned int n) {
  if (n == 0) return;
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64

  if (!stdmem_cpuid_check_done) StdMemCheckCPUID();
  if (n >= (EXTMMX_MEMFILLS_THRESHOLD >>2) && stdmem_sse_available) {
    unsigned int mmx_val[2]={val, val};
    int size_div_64 = n>>4;
    __asm {
        movq mm0, mmx_val
	      mov	edx, [lptr]
	      mov	eax, size_div_64
      l:
	      movntq	[edx], mm0
	      movntq	[edx+8], mm0
	      movntq	[edx+16], mm0
	      movntq	[edx+24], mm0
	      movntq	[edx+32], mm0
	      movntq	[edx+40], mm0
	      movntq	[edx+48], mm0
	      movntq	[edx+56], mm0
	      add	edx, 64
	      dec	eax
	      jnz	l
	      sfence
	      emms 
      };

    int offset = size_div_64<<4;
    n -= offset;
    if (n == 0) return;
    lptr = (unsigned long *)lptr + offset;
    // fallthru for the rest of the buffer
  } 

  __asm {
    mov eax, val
    mov edi, lptr
    mov ecx, n
    rep stosd
  };

#elif defined(GCC)
//http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html ;)
asm ("cld\n\t"
     "rep\n\t"
     "stosl"
     : /* no output registers */
     : "c" (n), "a" (val), "D" (lptr)
     : "%ecx", "%edi" );
#else
#ifdef PLATFORM_WARNINGS
#error port me!
#endif
#endif
}

void MEMFILL(unsigned short *ptr, unsigned short val, unsigned int n) {
  if (n == 0) return;
  unsigned long v = (unsigned long)val | ((unsigned long)val << 16);
  int r = n & 1;
  MEMFILL32(ptr, v, n/2);
  if (r) ptr[n-1] = val;
}

void MEMCPY32(void *dest, void *src, unsigned int nwords) {
  ASSERT(nwords >= 0);
  if (nwords == 0) return;
  ASSERT(dest != NULL);
  ASSERT(src != NULL);

  MEMCPY(dest, src, nwords*4);	// hardcode *4 since it's 32 bit words
}

#ifndef UNLEN
#define UNLEN 256
#endif

void Std::getUserName(char *name, int bufsize) {
#ifdef WIN32
  char buf[UNLEN+1]="";
  unsigned long l = UNLEN;
  GetUserName(buf, &l);
  buf[UNLEN] = 0;
  STRNCPY(name, buf, bufsize);
  name[bufsize-1] = 0;
#elif defined(WASABI_PLATFORM_POSIX)
  getlogin_r(name, bufsize);
  name[bufsize-1] = 0;
#else
  ASSERTPR(0, "port getusername!");
#endif
}

int Std::isDebuggerPresent() {
  int ret = 0;
#ifdef WASABI_PLATFORM_WIN32
  HINSTANCE kernel32 = LoadLibrary("kernel32.dll");
  if (kernel32 == NULL) return 0;
  BOOL (WINAPI *IsDebuggerPresent)() = NULL;
  IsDebuggerPresent = (BOOL (WINAPI *)())GetProcAddress(kernel32, "IsDebuggerPresent");
  if (IsDebuggerPresent != NULL) ret = (*IsDebuggerPresent)();
  FreeLibrary(kernel32);
#else
//PORTME
#endif
  return ret;
}

#ifdef WASABI_PLATFORM_WIN32
int Std::isWinNT() {
	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
  if(osvi.dwPlatformId == 2) {
    return 1;
  }
  return 0;
}
#endif
