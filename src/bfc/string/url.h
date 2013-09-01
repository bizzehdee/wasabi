#ifndef _URL_H
#define _URL_H

#include <bfc/common.h>
#include <bfc/string/string.h>

#define URLENCODE_NOTHING                 1     // encode nothing (add some flags!), default is to start with everything
#define URLENCODE_EXCLUDEALPHANUM         2     // do not encode characters that are alphanumeric (a-z,0-1) or _ - and .
#define URLENCODE_EXCLUDESLASH		  4	// don't encode '/'
#define URLENCODE_EXCLUDEHTTPPREFIX		  8	// don't encode "http://"
#define URLENCODE_EXCLUDE_8BIT            16     // do not encode > 127
#define URLENCODE_EXCLUDE_7BIT_ABOVEEQ32  32     // do not encode >= 32 & <= 127
#define URLENCODE_ENCODESPACE             64    // force encoding space
#define URLENCODE_ENCODEXML               128    // force encoding '>' and '<' 

#define URLENCODE_DEFAULT                 URLENCODE_EXCLUDEALPHANUM
#define URLENCODE_FULLURL	(URLENCODE_DEFAULT|URLENCODE_EXCLUDEHTTPPREFIX|URLENCODE_EXCLUDESLASH|URLENCODE_EXCLUDE_7BIT_ABOVEEQ32|URLENCODE_ENCODESPACE)

namespace Url {
  enum {
    URLENCODE_STYLE_PERCENT = 0,   // %AB
    URLENCODE_STYLE_ANDPOUND = 1,  // &#171;
    URLENCODE_STYLE_ANDPOUNDX = 2, // &#xAB;
  };

  void encode(String &dest, int use_plus_for_space = FALSE, int encoding = URLENCODE_DEFAULT, int style=URLENCODE_STYLE_PERCENT);
  void decode(String &str, int use_plus_for_space = FALSE);
};

#endif
