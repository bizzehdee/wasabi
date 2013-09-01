//PORTABLE
#ifndef _TIMEFMT_H
#define _TIMEFMT_H

/**
  Simple time formatting. Can format into a minutes:seconds style
  display based on count in seconds only. 
  
  Can also format a timestamp into human readable format.
  
  @author Nullsoft
  @ver 1.0
*/
class TimeFmt {
public:
  /**
    Formats a time value in seconds to minute:seconds.
    
    If the buffer is too small, the string will be
    truncated.
    
    @param seconds  Time value to convert.
    @param buf      Buffer to receive formatted string.
    @param buflen   Length of the buffer.
  */
  static void printMinSec(int seconds, char *buf, int buflen);
  static void printHourMinSec(int seconds, char *buf, int buflen, int hoursonlyifneeded=0);
  
  /**
    Formats a time value (from unix timestamp) to
    human readable format.

    If the buffer is too small, the string will be
    truncated.
    
    Example of formatted output:
      Tue Sep 10 18:34:42 PDT 2002
      
    @param buf      Buffer to receive the formatted string.
    @param bufsize  Length of the buffer.
    @param ts       The timestamp to use.
  */
  static void printTimeStamp(char *buf, int bufsize, int ts);

  static void printCustom(int sec, const char *buf, int bufsize, const char *str_sep=", ", const char *str_seconds="s", const char *str_minutes="m", const char *str_hours="h", const char *str_days="d");
};

#endif
