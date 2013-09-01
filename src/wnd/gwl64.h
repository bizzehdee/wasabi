#ifndef GWL64_H
#define GWL64_H

/*

x64 replaces SetWindowLong and GetWindowLong with SetWindowLongPtr and GetWindowLongPtr. Also, the GWL_* constants are now GWLP_*.

To make code that compiles and works on both x86 and x64, use :

- SETWINDOWLONG in place of SetWindowLong
- GETWINDOWLONG in place of GetWindowLong
- _GWL_* in place of GWL_*

And, VERY IMPORTANT, in every previous call to SetWindowLong, where you would normally cast your final parameter to a (long) or a (LONG), cast it instead to (_GWL_LONGTYPE)

*/

#ifdef _WIN64
#undef SetWindowLong
#undef GetWindowLong

#define SetWindowLong use_SETWINDOWLONG_instead_and_read_gwl64_dot_h
#define GetWindowLong use_GETWINDOWLONG_instead_and_read_gwl64_dot_h

#define GETWINDOWLONG GetWindowLongPtr
#define SETWINDOWLONG SetWindowLongPtr
#define GETWINDOWLONGW GetWindowLongPtrW
#define SETWINDOWLONGW SetWindowLongPtrW
#define GETWINDOWLONGA GetWindowLongPtrA
#define SETWINDOWLONGA SetWindowLongPtrA
#define _GWL_WNDPROC GWLP_WNDPROC
#define _GWL_USERDATA GWLP_USERDATA
#define _GWL_HINSTANCE GWLP_HINSTANCE
#define _GWL_HWNDPARENT GWLP_HWNDPARENT
#define _GWL_STYLE GWLP_STYLE
#define _GWL_EXSTYLE GWLP_EXSTYLE
#define _GWL_USERDATA GWLP_USERDATA
#define _GWL_LONGTYPE LONG_PTR
#else
#define GETWINDOWLONG GetWindowLong
#define SETWINDOWLONG SetWindowLong
#define GETWINDOWLONGW GetWindowLongW
#define SETWINDOWLONGW SetWindowLongW
#define GETWINDOWLONGA GetWindowLongA
#define SETWINDOWLONGA SetWindowLongA
#define _GWL_WNDPROC GWL_WNDPROC
#define _GWL_USERDATA GWL_USERDATA
#define _GWL_HINSTANCE GWL_HINSTANCE
#define _GWL_HWNDPARENT GWL_HWNDPARENT
#define _GWL_STYLE GWL_STYLE
#define _GWL_EXSTYLE GWL_EXSTYLE
#define _GWL_USERDATA GWL_USERDATA

#define _GWL_LONGTYPE LONG
#endif

#endif // GWL64_H
