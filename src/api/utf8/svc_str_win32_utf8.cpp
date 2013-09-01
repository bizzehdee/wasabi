#include "precomp.h"
#ifndef WIN32
#error "HELLO MORON!  This code is win32 only!@  Make your own damn SvcStrCnv::OSNATIVE version!"
#else // defined(WIN32)

#include <windows.h>
#include <api/utf8/svc_str_win32_utf8.h>
#include <api/service/svc_enum.h>

#ifndef _WASABIRUNTIME
BEGIN_SERVICES(Win32UTF8_Svc);
DECLARE_SERVICETSINGLE(svc_stringConverter, svc_str_win32_utf8);
END_SERVICES(Win32UTF8_Svc, _Win32UTF8_Svc);

#ifdef _X86_
extern "C" { int _link_Win32UTF8_Svc; }
#else
extern "C" { int __link_Win32UTF8_Svc; }
#endif

#endif

int svc_str_win32_utf8::canConvert(FOURCC encoding_type) {
  if (encoding_type == SvcStrCnv::OSNATIVE) {
    return 1;
  } else if (encoding_type == SvcStrCnv::UTF16) {
    // Check to see if the OS can handle UTF8 (win32 can always handle SvcStrCnv::UTF16).
    return (IsValidCodePage(CP_UTF8) != 0);
  }

  return 0;
}

int svc_str_win32_utf8::convertGetLastError() {
  int retval = SvcStrCnv::ERROR_FATAL; // unknown error.
  int lasterror = GetLastError();
  // Map the Windows error types to our error types.
  switch (lasterror) {
    case ERROR_INSUFFICIENT_BUFFER:
      retval = SvcStrCnv::ERROR_BUFFER_OVERRUN;
    break;
    case ERROR_INVALID_PARAMETER:
      retval = SvcStrCnv::ERROR_INPUT;
    break;
    case ERROR_INVALID_FLAGS:
    default:
      retval = SvcStrCnv::ERROR_FATAL;
    break;
  }
  return retval;
}

// "convertGetLastError" will be valid to call if this function returns error.
unsigned short * svc_str_win32_utf8::allocCodepageToUTF16(int codepage, const char *in_buffer, int size_in_bytes, int *size_out_bytes) {
  unsigned short *retval = NULL;
  int buff16Size = 0; // (IN BYTES!)
  int free16 = 0;

  // Preflight for MBCS->SvcStrCnv::UTF16
  buff16Size = sizeof(short) * MultiByteToWideChar(codepage, 0, in_buffer, size_in_bytes, NULL, 0);
  if (buff16Size > 0) {
    // Allocate
#ifdef WASABI_COMPILE_MEMMGR
    unsigned short *buffOut = reinterpret_cast<unsigned short *>(WASABI_API_MEMMGR->sysMalloc(buff16Size));
#else
    unsigned short *buffOut = reinterpret_cast<unsigned short *>(MALLOC(buff16Size));
#endif
    // Now convert for real.
    int written = sizeof(short) * MultiByteToWideChar(codepage, 0, in_buffer, size_in_bytes, (LPWSTR) buffOut, buff16Size / sizeof(short));
    if (written > 0) {
      // Assign the variables to our return params on success.
      retval = buffOut;
      *size_out_bytes = buff16Size;
    } else {
      // Otherwise, just free the ram.
      #ifdef WASABI_COMPILE_MEMMGR
      WASABI_API_MEMMGR->sysFree(buffOut);
      #else
      FREE(buffOut);
      #endif
    }
  }
  return retval;
}


// This is even more slow, because it's even more complicated.

// It's likely we could speed this code up by using intelligent pre-sized buffers to handle the most common
// sizes of string conversion which causes larger-than-average sized buffers to be translated even slower than
// this code.  Of course, that's about 2.5 times as much code for handling those forced error conditions
// and I just can't write all of that in one night.  Sorry.

// Whoever comes in behind me and wants to hate me for how slow this goes can try doing that, first.
// I'd like to say "that's why there's so many comments," but that's just my coding style.  Sue me.

int svc_str_win32_utf8::convertToUTF8(FOURCC encoding_type, const void *in_buffer, int size_in_bytes, char *out_buffer, int size_out_bytes) {
  int retval = 0;
// Okay, cheat if we can cheat.
  int utf8capable = IsValidCodePage(CP_UTF8);
  int ansiCodePage = GetACP();
  if (((ansiCodePage == 437) || (ansiCodePage == 20127)) && (encoding_type == SvcStrCnv::OSNATIVE)) {
    // magically, this codepage is 7bit ascii, so we're already utf8 compliant.
    // unfortunately, I don't think you get these codepages under 2k.  that's fine
    // because 2k can still give you funny filenames even in the english version, so
    // you CAN'T cheat and depend on the codepage in 2k anyhow.  tradeoffs, wot?
    int want_out_bytes = STRLEN(reinterpret_cast<const char *>(in_buffer)) + 1;
    if (want_out_bytes > size_out_bytes) {
      retval = SvcStrCnv::ERROR_BUFFER_OVERRUN;
    } else {
      retval = want_out_bytes;
      STRCPY(out_buffer, reinterpret_cast<const char *>(in_buffer));
    }
  } else {
    const unsigned short *buff16 = NULL;
    int buff16Size = 0; // (IN BYTES!)
    int free16 = 0;

    if (encoding_type == SvcStrCnv::OSNATIVE) {
      buff16 = allocCodepageToUTF16(CP_ACP, reinterpret_cast<const char *>(in_buffer), size_in_bytes, &buff16Size);
      if (buff16 == NULL) {
        retval = convertGetLastError();
      } else {
        free16 = 1;
      }
    } else if (encoding_type == SvcStrCnv::UTF16) {
      // Otherwise, if we have SvcStrCnv::UTF16, just copy the pointer
      buff16 = reinterpret_cast<const unsigned short *>(in_buffer);
      buff16Size = size_in_bytes;
    } else {
      // Return the error message that we do not support that encoding type.
      retval = SvcStrCnv::ERROR_INVALID;
    }


    if (buff16 != NULL) {
      // If our OS version is compatible, use the OS.
      if (utf8capable) {
        int buff8Size = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR) buff16, -1, NULL, 0, NULL, NULL);
        if (buff8Size == 0) {
          retval = convertGetLastError();
        } else if (buff8Size > size_out_bytes) {
          retval = SvcStrCnv::ERROR_BUFFER_OVERRUN;          
        } else {
          int written = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR) buff16, -1, out_buffer, size_out_bytes, NULL, NULL);
          if (written > 0) {
            retval = written;
          } else {
            retval = convertGetLastError();
          }
        }
      } else if (encoding_type == SvcStrCnv::UTF16) {
      // If our OS can't do it, and we were specifically ASKED to do it, that's an error (prevents recursion)
        // Return the error message that we do not support that encoding type.
        retval = SvcStrCnv::ERROR_INVALID;
      } else {
      // If our OS can't do it, see if there is another WAC installed which can.
        StringConverterEnum myServiceEnum(SvcStrCnv::UTF16);
        svc_stringConverter *myConv = myServiceEnum.getFirst();
        if (myConv != NULL) {
          // This means the user installed the ICU string conversion WAC on an OLD OS.
          retval = myConv->convertToUTF8(SvcStrCnv::UTF16, buff16, buff16Size, out_buffer, size_out_bytes);
          myServiceEnum.release(myConv);
        } else {
          // This means the user needs to install the ICU string conversion WAC on an OLD os.
          retval = SvcStrCnv::ERROR_INVALID;
        }
      }
    }
// Free the interim allocation if needbe.
    if (free16) {
      #ifdef WASABI_COMPILE_MEMMGR
      WASABI_API_MEMMGR->sysFree(const_cast<unsigned short *>(buff16));
      #else
      FREE(const_cast<unsigned short *>(buff16));
      #endif
    }
  }
  return retval;
}

int svc_str_win32_utf8::preflightToUTF8(FOURCC encoding_type, const void *in_buffer, int size_in_bytes) {
  int retval = 0;
// Okay, cheat if we can cheat.
  int utf8capable = IsValidCodePage(CP_UTF8);
  int ansiCodePage = GetACP();
  if (((ansiCodePage == 437) || (ansiCodePage == 20127)) && (encoding_type == SvcStrCnv::OSNATIVE)) {
    // magically, this codepage is 7bit ascii, so we're already utf8 compliant.
    // unfortunately, I don't think you get these codepages under 2k.  that's fine
    // because 2k can still give you funny filenames even in the english version, so
    // you CAN'T cheat and depend on the codepage in 2k anyhow.  tradeoffs, wot?
    retval = STRLEN(reinterpret_cast<const char *>(in_buffer));
  } else {
    const unsigned short *buff16 = NULL;
    int buff16Size = 0; // (IN BYTES!)
    int free16 = 0;

// If we're SvcStrCnv::OSNATIVE, first convert to 16,
    // Ansi codepage for an MBCS encoding will have us first convert to u16, then fall
    // through into the u16 decoder in the next block of code.
    if (encoding_type == SvcStrCnv::OSNATIVE) {
      // Preflight for MBCS->SvcStrCnv::UTF16
      buff16 = allocCodepageToUTF16(CP_ACP, reinterpret_cast<const char *>(in_buffer), size_in_bytes, &buff16Size);
      if (buff16 == NULL) {
        retval = convertGetLastError();
      } else {
        free16 = 1;
      }
    } else if (encoding_type == SvcStrCnv::UTF16) {
    // Otherwise, if we have SvcStrCnv::UTF16, just copy the pointer
      buff16 = reinterpret_cast<const unsigned short *>(in_buffer);
      buff16Size = size_in_bytes;
    } else {
      // Return the error message that we do not support that encoding type.
      retval = SvcStrCnv::ERROR_INVALID;
    }
// Once we have a pointer to U16 (by whatever means), preflight it.
    if (buff16 != NULL) {
      // If our OS version is compatible, use the OS.
      if (utf8capable) {
        retval = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR) buff16, -1, NULL, 0, NULL, NULL);
        if (retval == 0) {
          retval = convertGetLastError();
        }
      } else if (encoding_type == SvcStrCnv::UTF16) {
      // If our OS can't do it, and we were specifically ASKED to do it, that's an error (prevents recursion)
        // Return the error message that we do not support that encoding type.
        retval = SvcStrCnv::ERROR_INVALID;
      } else {
      // If our OS can't do it, see if there is another WAC installed which can.
        StringConverterEnum myServiceEnum(SvcStrCnv::UTF16);
        svc_stringConverter *myConv = myServiceEnum.getFirst();
        if (myConv != NULL) {
          // This means the user installed the ICU string conversion WAC on an OLD OS.
          retval = myConv->preflightToUTF8(SvcStrCnv::UTF16, buff16, buff16Size);
          myServiceEnum.release(myConv);
        } else {
          // This means the user needs to install the ICU string conversion WAC on an OLD os.
          retval = SvcStrCnv::ERROR_INVALID;
        }
      }
    }
// Free the interim allocation if needbe.
    if (free16) {
      #ifdef WASABI_COMPILE_MEMMGR
      WASABI_API_MEMMGR->sysFree(const_cast<unsigned short *>(buff16));
      #else
      FREE(const_cast<unsigned short *>(buff16));
      #endif
    }
  }
  return retval;
}

int svc_str_win32_utf8::convertFromUTF8(FOURCC encoding_type, const char *in_buffer, int size_in_bytes, void *out_buffer, int size_out_bytes) {
  int retval = 0;
// Okay, cheat if we can cheat.
  int utf8capable = IsValidCodePage(CP_UTF8);
  int ansiCodePage = GetACP();
  if (((ansiCodePage == 437) || (ansiCodePage == 20127)) && (encoding_type == SvcStrCnv::OSNATIVE)) {
    // magically, this codepage is 7bit ascii, so we're already utf8 compliant.
    // unfortunately, I don't think you get these codepages under 2k.  that's fine
    // because 2k can still give you funny filenames even in the english version, so
    // you CAN'T cheat and depend on the codepage in 2k anyhow.  tradeoffs, wot?
    int want_out_bytes = STRLEN(reinterpret_cast<const char *>(in_buffer)) + 1;
    if (want_out_bytes > size_out_bytes) {
      retval = SvcStrCnv::ERROR_BUFFER_OVERRUN;
    } else {
      retval = want_out_bytes;
      STRCPY(reinterpret_cast<char *>(out_buffer), in_buffer);
    }
  } else {
    if (encoding_type == SvcStrCnv::UTF16) {
      // If we wanted to go to U16, we're done.
      if (utf8capable) {
        int written = sizeof(short) * MultiByteToWideChar(CP_UTF8, 0, in_buffer, size_in_bytes, 
                      (LPWSTR) reinterpret_cast<unsigned short *>(out_buffer), size_out_bytes / sizeof(short));
        if (written > 0) {
          retval = written;
        } else {
          retval = convertGetLastError();
        }
      } else {
        retval = SvcStrCnv::ERROR_INVALID;
      }
    } else if (encoding_type == SvcStrCnv::OSNATIVE) {
      const unsigned short *buff16 = NULL;
      int buff16Size = 0; // (IN BYTES!)
      int free16 = 0;

      // If we can go direct to utf16, do so.
      if (utf8capable) {
        buff16 = allocCodepageToUTF16(CP_UTF8, in_buffer, size_in_bytes, &buff16Size);
        if (buff16 == NULL) {
          retval = convertGetLastError();
        } else {
          free16 = 1;
        }
      } else {
        StringConverterEnum myServiceEnum(SvcStrCnv::UTF16);
        svc_stringConverter *myConv = myServiceEnum.getFirst();
        if (myConv != NULL) {
          // This means the user installed the ICU string conversion WAC on an OLD OS.
          buff16Size = myConv->preflightFromUTF8(SvcStrCnv::UTF16, in_buffer, size_in_bytes);
          if (buff16Size > 0) {
            #ifdef WASABI_COMPILE_MEMMGR
            unsigned short *buffOut = reinterpret_cast<unsigned short *>(api->sysMalloc(buff16Size));
            #else
            unsigned short *buffOut = reinterpret_cast<unsigned short *>(MALLOC(buff16Size));
            #endif
            int written = myConv->convertFromUTF8(SvcStrCnv::UTF16, in_buffer, size_in_bytes, buffOut, buff16Size);
            if (written > 0) {
              buff16 = buffOut;
              free16 = 1;
            } else {
              retval = written;
              #ifdef WASABI_API_MEMMGR
              api->sysFree(buffOut);
              #else
              FREE(buffOut);
              #endif
            }
          } else {
            retval = buff16Size;
          }
          myServiceEnum.release(myConv);
        } else {
          // This means the user needs to install the ICU string conversion WAC on an OLD os.
          retval = SvcStrCnv::ERROR_INVALID;
        }
      }  

      if (buff16 != 0) {
        // Now take it down to the converter
        int written = WideCharToMultiByte(CP_ACP, 0, (LPWSTR) buff16, -1, reinterpret_cast<char *>(out_buffer), size_out_bytes, NULL, NULL);
        if (written > 0) {
          retval = written;
        } else {
          retval = convertGetLastError();
        }
      }

      if (free16) {
        #ifdef WASABI_COMPILE_MEMMGR
        WASABI_API_MEMMGR->sysFree(const_cast<unsigned short *>(buff16));
        #else
        FREE(const_cast<unsigned short *>(buff16));
        #endif
      }

    } else {
      retval = SvcStrCnv::ERROR_INVALID;
    }

  }    
  return retval;
}

int svc_str_win32_utf8::preflightFromUTF8(FOURCC encoding_type, const char *in_buffer, int size_in_bytes) {
  int retval = 0;
// Okay, cheat if we can cheat.
  int utf8capable = IsValidCodePage(CP_UTF8);
  int ansiCodePage = GetACP();
  if (((ansiCodePage == 437) || (ansiCodePage == 20127)) && (encoding_type == SvcStrCnv::OSNATIVE)) {
    // magically, this codepage is 7bit ascii, so we're already utf8 compliant.
    // unfortunately, I don't think you get these codepages under 2k.  that's fine
    // because 2k can still give you funny filenames even in the english version, so
    // you CAN'T cheat and depend on the codepage in 2k anyhow.  tradeoffs, wot?
    retval = STRLEN(reinterpret_cast<const char *>(in_buffer)) + 1;
  } else {
    if (encoding_type == SvcStrCnv::UTF16) {
      // If we wanted to go to U16, we're done.
      if (utf8capable) {
        int written = sizeof(short) * MultiByteToWideChar(CP_UTF8, 0, in_buffer, size_in_bytes, NULL, 0);
        if (written > 0) {
          retval = written;
        } else {
          retval = convertGetLastError();
        }
      } else {
        retval = SvcStrCnv::ERROR_INVALID;
      }
    } else if (encoding_type == SvcStrCnv::OSNATIVE) {
      const unsigned short *buff16 = NULL;
      int buff16Size = 0; // (IN BYTES!)
      int free16 = 0;

      // If we can go direct to utf16, do so.
      if (utf8capable) {
        buff16 = allocCodepageToUTF16(CP_UTF8, in_buffer, size_in_bytes, &buff16Size);
        if (buff16 == NULL) {
          retval = convertGetLastError();
        } else {
          free16 = 1;
        }
      } else {
        StringConverterEnum myServiceEnum(SvcStrCnv::UTF16);
        svc_stringConverter *myConv = myServiceEnum.getFirst();
        if (myConv != NULL) {
          // This means the user installed the ICU string conversion WAC on an OLD OS.
          buff16Size = myConv->preflightFromUTF8(SvcStrCnv::UTF16, in_buffer, size_in_bytes);
          if (buff16Size > 0) {
            #ifdef WASABI_COMPILE_MEMMGR
            unsigned short *buffOut = reinterpret_cast<unsigned short *>(api->sysMalloc(buff16Size));
            #else
            unsigned short *buffOut = reinterpret_cast<unsigned short *>(MALLOC(buff16Size));
            #endif
            int written = myConv->convertFromUTF8(SvcStrCnv::UTF16, in_buffer, size_in_bytes, buffOut, buff16Size);
            if (written > 0) {
              buff16 = buffOut;
              free16 = 1;
            } else {
              retval = written;
              #ifdef WASABI_COMPILE_MEMMGR
              WASABI_API_MEMMGR->sysFree(buffOut);
              #else
              FREE(buffOut);
              #endif
            }
          } else {
            retval = buff16Size;
          }
          myServiceEnum.release(myConv);
        } else {
          // This means the user needs to install the ICU string conversion WAC on an OLD os.
          retval = SvcStrCnv::ERROR_INVALID;
        }
      }  

      if (buff16 != 0) {
        // Now take it down to the converter
        int written = WideCharToMultiByte(CP_ACP, 0, (LPWSTR) buff16, -1, NULL, 0, NULL, NULL);
        if (written > 0) {
          retval = written;
        } else {
          retval = convertGetLastError();
        }
      }

      if (free16) {
        #ifdef WASABI_COMPILE_MEMMGR
        WASABI_API_MEMMGR->sysFree(const_cast<unsigned short *>(buff16));
        #else
        FREE(const_cast<unsigned short *>(buff16));
        #endif
      }

    } else {
      retval = SvcStrCnv::ERROR_INVALID;
    }

  }    
  return retval;
}



#endif // defined(WIN32)
