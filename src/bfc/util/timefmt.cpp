#include <precomp.h>
#include <bfc/std.h>
#include <bfc/string/string.h>
#include <time.h>
#include "timefmt.h"

void TimeFmt::printMinSec(int sec, char *buf, int buflen) {
  int minutes, seconds;
  int negative = sec < 0;

  if (buf == NULL) return;

  if (sec == -1) {
    *buf = 0;
    return;
  }

  seconds = sec % 60;
  sec /= 60;
  minutes = sec;

  StringPrintf sp("%s%d:%02d", (minutes == 0 && negative) ? "-" : "", minutes, ABS(seconds));
  STRNCPY(buf, sp, buflen);
}

void TimeFmt::printHourMinSec(int sec, char *buf, int buflen, int hoursonlyifneeded) {
  int hours, minutes, seconds;
  int negative = sec < 0;

  sec = ABS(sec);
  if (buf == NULL) return;

  if (sec == -1) {
    *buf = 0;
    return;
  }

  hours = sec / 3600;
  sec -= hours * 3600;
  seconds = sec % 60;
  sec /= 60;
  minutes = sec;

  String sp;
  if (hoursonlyifneeded && hours == 0)
    sp = StringPrintf("%s%d:%02d", (minutes == 0 && negative) ? "-" : "", minutes, seconds);
  else
    sp = StringPrintf("%s%d:%02d:%02d", (minutes == 0 && negative) ? "-" : "", hours, minutes, seconds);

  STRNCPY(buf, sp, buflen);
}

void TimeFmt::printTimeStamp(char *buf, int bufsize, int ts) {
  if (ts == 0) {
    STRNCPY(buf, "Never", bufsize);	// FUCKO: load from lang pack
    return;
  }

  struct tm *tm_now;
  tm_now = localtime((const time_t *)&ts);
  if (tm_now == NULL) {
    *buf = 0;
    return;
  }
  strftime(buf, bufsize, "%a %b %Y %d %I:%M:%S %p", tm_now);
}

void TimeFmt::printCustom(int sec, const char *buf, int bufsize, const char *str_sep, const char *str_seconds, const char *str_minutes, const char *str_hours, const char *str_days) {
  if (buf == NULL || bufsize == 0) return;
  char *p = (char *)buf;
  *p = 0;
  int days, hours, minutes;
  String s;

  if (str_days) {
    days = sec / (3600*24);
    sec -= days * (3600*24);
    if (days != 0) {
      s += StringPrintf("%d%s", days, str_days);
    }
  }
  if (str_hours) {
    hours = sec / 3600;
    sec -= hours * 3600;
    if (hours != 0) {
      if (!s.isempty()) s += str_sep;
      s += StringPrintf("%d%s", hours, str_hours);
    }
  }
  if (str_minutes) {
    minutes = sec / 60;
    sec -= minutes * 60;
    if (minutes != 0) {
      if (!s.isempty()) s += str_sep;
      s += StringPrintf("%d%s", minutes, str_minutes);
    }
  }
  if (str_seconds) {
    if (sec != 0) {
      if (!s.isempty()) s += str_sep;
      s += StringPrintf("%d%s", sec, str_seconds);
    }
  }
  STRNCPY(p, s.getValue(), bufsize);
  int l = s.len();
  if (l < bufsize) p[l] = 0;
}
