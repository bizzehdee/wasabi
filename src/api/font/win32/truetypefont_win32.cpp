#include "precomp.h"
// ============================================================================================================================================================
// Font abstract class + statics to install TT fonts and Bitmap fonts
// ============================================================================================================================================================
#define WIN32_LEAN_AND_MEAN
#include <fcntl.h>

#include "truetypefont_win32.h"

#include <draw/bitmap.h>
#include <wnd/bltcanvas.h>
#include <bfc/file/tmpnamestr.h>

#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in std.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif

// ============================================================================================================================================================
// TrueTypeFont_Win32 implementation. 
// ============================================================================================================================================================

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
TrueTypeFont_Win32::TrueTypeFont_Win32() {
  font = NULL;
  antialias_canvas = NULL;
  DColdstate = NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
TrueTypeFont_Win32::~TrueTypeFont_Win32() {
  ASSERT(fontstack.isempty());
#ifdef WIN32
  if (!tmpfilename.isempty()) {
    RemoveFontResource(tmpfilename);
    unlink(tmpfilename);
  }
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
int TrueTypeFont_Win32::isBitmap() {
  return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
int TrueTypeFont_Win32::addFontResource(FILE *in, const char *name){
  ASSERT(in != NULL);
#ifdef WIN32
  int len = FGETSIZE(in);
  FILE *out;
  char *m = (char *)MALLOC(len);
  ASSERT(m!=NULL);
  FREAD(m, len, 1, in);
  TmpNameStr tempfn;
  out = FOPEN(tempfn, "wb");
  ASSERT(out);
  FWRITE(m, len, 1, out);
  FCLOSE(out);
  AddFontResource(tempfn);
  FREE(m);
  tmpfilename = tempfn;
  setFontFace(filenameToFontFace(tempfn));
  return 1;
#else
  DebugString( "portme -- TrueTypeFont_Win32::addFontResource\n" );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::setFontFace(const char *face) {
  face_name = face;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
const char *TrueTypeFont_Win32::getFaceName() {
  return face_name;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::prepareCanvas(CanvasBase *c, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor) {

#ifdef WIN32

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#define CLEARTYPE_COMPAT_QUALITY 6
#endif

#ifndef WASABI_DISALLOW_CLEARTYPE
#define cleartype CLEARTYPE_QUALITY
#else
#define cleartype 0
#endif

  DColdstate = SaveDC(c->getHDC());

  fontslot *f = new fontslot;
  f->dcstate = DColdstate;

  f->font = font;

  int angle = c->getTextAngle();
  font = CreateFont(size, 0, angle, angle, bold ? FW_BOLD : FW_NORMAL,
                    italic, underline, FALSE, DEFAULT_CHARSET,
                    OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                    ANTIALIASED_QUALITY | cleartype,
                    VARIABLE_PITCH|FF_DONTCARE, face_name);

  f->prevfont = (HFONT)SelectObject(c->getHDC(), font);
  SetTextColor(c->getHDC(), color);
  SetBkColor(c->getHDC(), bkcolor);
  SetBkMode(c->getHDC(), opaque ? OPAQUE : TRANSPARENT);

  fontstack.push(f);
#else
  String fontname = StringPrintf( "-*-%s-%s-%s-*--%d-*-*-*-*-*-*-*",
				  (const char *)face_name, 
				  bold?"bold":"medium",
				  italic?"i":"r", size * 3/4 );
  font = XLoadQueryFont( Linux::getDisplay(), (const char *)fontname );
  if ( font == NULL ) {
    fontname = StringPrintf( "-*-arial-%s-%s-*--%d-*-*-*-*-*-*-*", 
			     bold?"bold":"medium",
			     italic?"i":"r", size * 3/4 );
    font = XLoadQueryFont( Linux::getDisplay(), (const char *)fontname );

    if ( font == NULL ) {
      fontname = StringPrintf( "-*-courier-%s-%s-*--%d-*-*-*-*-*-*-*", 
			       bold?"bold":"medium",
			       italic?"i":"r", size * 3/4 );
      font = XLoadQueryFont( Linux::getDisplay(), (const char *)fontname );
    } 
  } 
  ASSERTPR( font != NULL, fontname );
  XSetFont( Linux::getDisplay(), c->getHDC()->gc, font->fid );
  XSetForeground( Linux::getDisplay(), c->getHDC()->gc, color );
#endif

}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::restoreCanvas(CanvasBase *c) { 
#ifdef WIN32
  fontslot *f;
  fontstack.pop(&f);
  RestoreDC(c->getHDC(), DColdstate);
  SelectObject(c->getHDC(), f->prevfont);
  DeleteObject(font);
  font = f->font;
  DColdstate = f->dcstate;
  delete f;
#else
  if ( font != NULL ) {
    XFreeFont( Linux::getDisplay(), font );
    font = NULL;
  }
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
CanvasBase *TrueTypeFont_Win32::prepareAntialias(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int w, int h) {
  ASSERT(antialias_canvas == NULL);
  BaseCloneCanvas canvas(c);
  prepareCanvas(&canvas, size, bold, opaque, underline, italic, color, bkcolor);
  al_w = MAX(2,canvas.getTextWidth(txt) * 2 + xoffset*2);
  al_h = MAX(2,canvas.getTextHeight()*2 + yoffset*2);
  restoreCanvas(&canvas);
  if (w != -1) {
    al_w = w * 2;
    al_dw = w;
  } else al_dw = w;
  if (h != -1) {
    al_h = h * 2;
    al_dh = h;
  } else al_dh = h;
  al_mask=RGB(0,0,0);
  if (color == al_mask) al_mask=RGB(255,255,255);
  antialias_canvas = new BltCanvas(al_w, al_h);
  antialias_canvas->fillBits(0);
  prepareCanvas(antialias_canvas, size*2, bold, opaque, underline, italic, color, bkcolor);
  if (al_mask != 0) antialias_canvas->fillBits(al_mask);
  al_x = x; al_y = y; al_xo = xoffset; al_yo = yoffset;
  return antialias_canvas;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::completeAntialias(CanvasBase *c) {
  BaseCloneCanvas canvas(c);
  antialias_canvas->maskColor(al_mask, RGB(0,0,0));
  BltCanvas *ac = new BltCanvas(al_w/2, al_h/2);
  antialias_canvas->antiAliasTo(ac, al_w/2, al_h/2, 2);
//fixme  SkinBitmap *b = ac->getSkinBitmap();
  RECT src={0,0,al_w/2,al_h/2};
  RECT dst={al_x+al_xo,al_y+al_yo,al_x+al_xo+al_dw,al_y+al_yo+al_dh};
//fixme  b->blitToRect(&canvas, &src, &dst);
  delete ac;
  restoreCanvas(antialias_canvas);
  delete antialias_canvas;
  antialias_canvas = NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOut(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  if (antialiased) {
    CanvasBase *canvas = prepareAntialias(c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, -1, -1);

#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        TextOutW(canvas->getHDC(), x+xoffset, y+yoffset, txt16, WSTRLEN(txt16));
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        TextOut(canvas->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        TextOut(canvas->getHDC(), x+xoffset, y+yoffset, txtOS, STRLEN(txtOS));
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        TextOut(canvas->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
      }
    }

#else
#ifdef WIN32
    TextOut(canvas->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
#else
    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, x+xoffset, y+yoffset, txt, STRLEN(txt) );
#endif
#endif


    completeAntialias(c);
  } else {
    prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);

#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        TextOutW(c->getHDC(), x+xoffset, y+yoffset, txt16, WSTRLEN(txt16));
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        TextOut(c->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        TextOut(c->getHDC(), x+xoffset, y+yoffset, txtOS, STRLEN(txtOS));
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        TextOut(c->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
      }
    }

#else
#ifdef WIN32
    TextOut(c->getHDC(), x+xoffset, y+yoffset, txt, STRLEN(txt));
#else
    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );

    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, x+xoffset, y+yoffset+ascent, txt, STRLEN(txt) );
#endif
#endif


    restoreCanvas(c);
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOut2(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  if (antialiased) {
    CanvasBase *canvas = prepareAntialias(c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, w, h);
    RECT al_r={0,0,w*2,h*2};


#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(canvas->getHDC(), txt16, -1, &al_r, align | DT_NOPREFIX);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(canvas->getHDC(), txtOS, -1, &al_r, align | DT_NOPREFIX);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX);
      }
    }

#else
#ifdef WIN32
    DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX);
#else
    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, al_r.left, al_r.top, txt, STRLEN(txt) );
#endif
#endif


    completeAntialias(c);
  } else {
    RECT r;
    r.left = x+xoffset;
    r.top = y+yoffset;
    r.right = r.left + w;
    r.bottom = r.top + h;
    prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);


#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &r, align | DT_NOPREFIX);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &r, align | DT_NOPREFIX);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX);
#else
    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );
    int xstart = r.left;

    if ( align == DT_RIGHT ) {
      int width = XTextWidth( font, txt, STRLEN( txt ) );
      xstart = r.right - width;

    } else if ( align == DT_CENTER ) {
      int width = XTextWidth( font, txt, STRLEN( txt ) );
      xstart = (r.right + r.left - width) / 2;
    }

    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, xstart, r.top + ascent, txt, STRLEN(txt) );
#endif
#endif


    restoreCanvas(c);
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOutEllipsed(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  if (antialiased) {
    CanvasBase *canvas = prepareAntialias(c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, w, h);
    RECT al_r={0,0,w*2,h*2};

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(canvas->getHDC(), txt16, -1, &al_r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(canvas->getHDC(), txtOS, -1, &al_r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      }
    }

#else
#ifdef WIN32
    DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
#else
    DebugString( "portme -- TrueTypeFont_Win32::textoutEllipsed\n" );
#endif
#endif


    completeAntialias(c);
  } else {
    RECT r;
    r.left = x+xoffset;
    r.top = y+yoffset;
    r.right = r.left + w;
    r.bottom = r.top + h;
    prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);


#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_END_ELLIPSIS);
#else
    if ( txt == NULL )
      return;

    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );

    char *tmp = (char *)MALLOC( STRLEN( txt ) + 3 );
    STRCPY( tmp, txt );

    if ( XTextWidth( font, tmp, STRLEN( tmp ) ) > r.right - r.left ) {
      int len = STRLEN( tmp );
      char *p = tmp + len;
      int width = r.right - r.left - XTextWidth( font, "...", 3 );
      while( XTextWidth( font, tmp, len ) > width ) {
	*p-- = '\0';
	len--;
      }
     STRCPY( p, "..." );
    }

    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, r.left, r.top + ascent, tmp, STRLEN(tmp) );

    FREE( tmp );
#endif
#endif


    restoreCanvas(c);
  }
}

#ifdef LINUX

const char *find_break( int (*width_func)(void *, const char *, int ), 
			void *f, const char *str, int width ) {
  const char *softret, *lastsoft, *hardret;

  if ( width_func( f, str, STRLEN( str ) ) <= width )
    return str + STRLEN( str );

  for( hardret = str; *hardret; hardret ++ )
    if ( *hardret == '\r' || *hardret == '\n' )
      break;

  if ( hardret && width_func( f, str, hardret - str ) <= width ) {
    return hardret;
  }
  for( softret = str; *softret && !isspace( *softret ); softret++ )
    ;
  
  if ( width_func( f, str, softret - str ) <= width ) {
    do {
      lastsoft = softret;

      for( softret = lastsoft+1; *softret && !isspace( *softret ); softret++ )
	;
      
    } while ( *lastsoft && width_func( f, str, softret - str ) <= width );

    softret = lastsoft;
  } else {
    for( softret = str; *softret; softret++ )
      if ( width_func( f, str, softret - str ) > width )
	break;

    softret--;
  }

  return softret;
}

int xlib_width( void *data, const char *str, int len ) {
  return XTextWidth( (XFontStruct *)data, str, len );
}

#endif

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOutWrapped(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  if (antialiased) {
    CanvasBase *canvas = prepareAntialias(c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, w, h);
    RECT al_r={0,0,w*2,h*2};

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(canvas->getHDC(), txt16, -1, &al_r, align | DT_NOPREFIX | DT_WORDBREAK);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_WORDBREAK);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(canvas->getHDC(), txtOS, -1, &al_r, align | DT_NOPREFIX | DT_WORDBREAK);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_WORDBREAK);
      }
    }

#else
#ifdef WIN32
    DrawText(canvas->getHDC(), txt, -1, &al_r, align | DT_NOPREFIX | DT_WORDBREAK);
#else
    DebugString( "portme -- TrueTypeFont_Win32::textoutWrapped\n" );
#endif
#endif

    
    completeAntialias(c);
  } else {
    RECT r;
    r.left = x+xoffset;
    r.top = y+yoffset;
    r.right = r.left + w;
    r.bottom = r.top + h;
    prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &r, align | DT_NOPREFIX | DT_WORDBREAK);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_WORDBREAK);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &r, align | DT_NOPREFIX | DT_WORDBREAK);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_WORDBREAK);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), txt, -1, &r, align | DT_NOPREFIX | DT_WORDBREAK);
#else
    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );
    HDC hdc = c->getHDC();

    int yoff = r.top + ascent;
    const char *cur = txt, *next;
    int length = STRLEN( txt );
    for( int yoff = r.top + ascent; 
	 yoff < r.bottom - descent; 
	 yoff += ascent + descent ) {

      next = find_break( xlib_width, font, cur, r.right - r.left );

      XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, r.left, yoff, cur, next - cur );

      for ( cur = next; *cur && isspace( *cur ); cur++ )
	;
	
      if ( cur >= txt + length )
	break;
    }
#endif
#endif

    
    restoreCanvas(c);
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOutWrappedPathed(CanvasBase *c, int x, int y, int w, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);
  RECT r;
  char *ptr, *d;
  const char *s;
  ptr = (char *)MALLOC(STRLEN(txt)+1+4);
  for (s = txt, d = ptr; *s; s++, d++) {
    if (*s == '/') *d = '\\';
    else *d = *s;
  }
  r.left = x+xoffset;
  r.top = y+yoffset;
  r.right = r.left + w;
  r.bottom = r.top + getTextHeight2(c, size, bold, underline, italic, antialiased);

  
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(ptr));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_MODIFYSTRING | DT_CALCRECT);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_MODIFYSTRING | DT_CALCRECT);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(ptr));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_MODIFYSTRING | DT_CALCRECT);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_MODIFYSTRING | DT_CALCRECT);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_MODIFYSTRING | DT_CALCRECT);
#else
    DebugString( "portme -- TrueTypeFont_Win32::textoutWrappedPathed\n" );
#endif
#endif

  
  for (d = ptr; *d; d++) {
    if (*d == '\\') *d = '/';
  }

  if (antialiased) {
    restoreCanvas(c);

    CanvasBase *canvas = prepareAntialias(c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, w, -1);
    RECT al_r={0,0,w*2,al_h};

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(ptr));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(canvas->getHDC(), txt16, -1, &al_r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), ptr, -1, &al_r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(ptr));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(canvas->getHDC(), txtOS, -1, &al_r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), ptr, -1, &al_r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      }
    }

#else
#ifdef WIN32
    DrawText(canvas->getHDC(), ptr, -1, &al_r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
#else
    DebugString( "portme -- TrueTypeFont_Win32::textoutWrappedPathed\n" );
#endif
#endif

    
    completeAntialias(c);
  } else {

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(ptr));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(ptr));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), ptr, -1, &r, align | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE);
#else
    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );

    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, r.left, r.top + ascent, txt, STRLEN(txt) );
    DebugString( "portme -- TrueTypeFont_Win32::textoutWrappedPathed\n" );
#endif
#endif

    
    restoreCanvas(c);
  }
  
  FREE(ptr);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::textOutCentered(CanvasBase *c, RECT *r, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialiased) {
  ASSERT(r != NULL);
  ASSERT(txt != NULL);
  RECT rr=*r;
  rr.left += xoffset;
  rr.right += xoffset;
  rr.top += yoffset;
  rr.bottom += yoffset;

  if (antialiased) {
    CanvasBase *canvas = prepareAntialias(c, r->left, r->top, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, r->right-r->left, r->bottom-r->top);
    RECT al_r={0,0,(r->right-r->left)*2,(r->bottom-r->top)*2};

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(canvas->getHDC(), txt16, -1, &al_r, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(canvas->getHDC(), txtOS, -1, &al_r, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(canvas->getHDC(), txt, -1, &al_r, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      }
    }

#else
#ifdef WIN32
    DrawText(canvas->getHDC(), txt, -1, &al_r, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
#else
    DebugString( "portme -- TrueTypeFont_Win32::textoutCentered\n" );
#endif

#endif

    
    completeAntialias(c);
  } else { 
    prepareCanvas(c, size, bold, opaque, underline, italic, color, bkcolor);

    
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        DrawTextW(c->getHDC(), txt16, -1, &rr, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &rr, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        DrawText(c->getHDC(), txtOS, -1, &rr, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        DrawText(c->getHDC(), txt, -1, &rr, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
      }
    }

#else
#ifdef WIN32
    DrawText(c->getHDC(), txt, -1, &rr, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK | DT_SINGLELINE);
#else
    int dir, ascent, descent, width;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );
    width = XTextWidth( font, txt, STRLEN( txt ) );

    HDC hdc = c->getHDC();
    XDrawString( Linux::getDisplay(), hdc->d, hdc->gc, (rr.right + rr.left - width) / 2, rr.top + ascent, txt, STRLEN(txt) );
#endif
#endif

    
    restoreCanvas(c);
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
int TrueTypeFont_Win32::getTextWidth(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialiased) {
  if (!text || !*text) return 0;
  int w;
  getTextExtent(c, text, &w, NULL, size, bold, underline, italic, antialiased);
  return w;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
int TrueTypeFont_Win32::getTextHeight(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialiased) {
  if (!text || !*text) return 0;
  int h;
  getTextExtent(c, text, NULL, &h, size, bold, underline, italic, antialiased);
  {
    // calcul for multiline text
    const char *p=text;
    int n=0;
    while(*p!=0) if(*p++=='\n') n++;
    if(n) h*=(n+1);
  }
  return h;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void TrueTypeFont_Win32::getTextExtent(CanvasBase *c, const char *txt, int *w, int *h, int size, int bold, int underline, int italic, int antialiased) {
  SIZE rsize={0,0};
//CUT  ASSERT(txt != NULL);
  if (txt == NULL || *txt == 0) {
    if (w != NULL) *w = 0;
    if (h != NULL) *h = 0;
    return;
  }

  if (antialiased) 
    prepareCanvas(c, size*2, bold, 0, underline, italic, 0, 0);
  else
    prepareCanvas(c, size, bold, 0, underline, italic, 0, 0);


  
#if UTF8
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      EncodedStr txt16enc;
      int retcode = txt16enc.convertFromUTF8(SvcStrCnv::UTF16, String(txt));
      WCHAR *txt16 = static_cast<WCHAR *>(txt16enc.getEncodedBuffer());
      if (txt16 != NULL) {
        GetTextExtentPoint32W(c->getHDC(), txt16, WSTRLEN(txt16), &rsize);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        GetTextExtentPoint32(c->getHDC(), txt, STRLEN(txt), &rsize);
      }
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      EncodedStr txtOSenc;
      int retcode = txtOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(txt));
      char *txtOS = reinterpret_cast<char *>(txtOSenc.getEncodedBuffer());
      if (txtOS != NULL) {
        GetTextExtentPoint32(c->getHDC(), txtOS, STRLEN(txtOS), &rsize);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        // If we fail conversion, the service might not be available yet.
        // so we'll try opening the file and pray that it's ascii7.
        GetTextExtentPoint32(c->getHDC(), txt, STRLEN(txt), &rsize);
      }
    }

#else
#ifdef WIN32
    GetTextExtentPoint32(c->getHDC(), txt, STRLEN(txt), &rsize);
#else
    int dir, ascent, descent;
    XCharStruct overall;
    XTextExtents( font, txt, STRLEN( txt ), &dir, &ascent, &descent, &overall );
    rsize.cy = ascent + descent;
    rsize.cx = XTextWidth( font, txt, STRLEN( txt ) );
#endif
#endif

  
  
  if (w != NULL) *w = rsize.cx;
  if (h != NULL) *h = rsize.cy;

  if (antialiased) {
    if (w != NULL) *w /= 2;
    if (h != NULL) *h /= 2;
  }

  restoreCanvas(c);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
int TrueTypeFont_Win32::getTextHeight2(CanvasBase *c, int size, int bold, int underline, int italic, int antialiased) {
  return getTextHeight(c, "My", size, bold, underline, italic, antialiased);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// code from ftp.microsoft.com/Softlib/MSLFILES/FONTINST.EXE
// retrieves the friendly font name from its filename
char *TrueTypeFont_Win32::filenameToFontFace(const char *pszFile) {
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "TrueType fonts MUST have Ascii7 filenames, for now.")
#endif
  static char					lpszLongName[256];
  unsigned            i;
  char								namebuf[255];
  int									fp;
  unsigned short      numNames;
  long								curseek;
  unsigned            cTables;
  sfnt_OffsetTable    OffsetTable;
  sfnt_DirectoryEntry Table;
  sfnt_NamingTable    NamingTable;
  sfnt_NameRecord			NameRecord;

#ifdef WIN32 
  lpszLongName[0] = '\0';
  if ((fp = _open(pszFile, O_RDONLY | O_BINARY)) == -1)
    return NULL;
 
  /* First off, read the initial directory header on the TTF.  We're only   * interested in the "numOffsets" variable to tell us how many tables   * are present in this file.
  *
  * Remember to always convert from Motorola format (Big Endian to   * Little Endian).
  */
  _read (fp, &OffsetTable, sizeof (OffsetTable) - sizeof
(sfnt_DirectoryEntry));
  cTables = (int) SWAPW (OffsetTable.numOffsets);
 
  for ( i = 0; i < cTables && i < 40; i++)
  {
    if ((_read (fp, &Table, sizeof (Table))) != sizeof(Table))       return NULL;
    if (Table.tag == tag_NamingTable) /* defined in sfnt_en.h */     {
  /* Now that we've found the entry for the name table, seek to that   * position in the file and read in the initial header for this   * particular table.  See "True Type Font Files" for information   * on this record layout.
    */
  lseek (fp, SWAPL (Table.offset), SEEK_SET);
  read (fp, &NamingTable, sizeof (NamingTable));
  numNames = SWAPW(NamingTable.count);
  while (numNames--)
  {
    _read (fp, &NameRecord, sizeof (NameRecord));
    curseek = _tell(fp);
    if (SWAPW(NameRecord.platformID) == 1 &&
      SWAPW(NameRecord.nameID) == 4) {
    lseek (fp, SWAPW (NameRecord.offset) +
      SWAPW(NamingTable.stringOffset) +
      SWAPL(Table.offset), SEEK_SET);
    read (fp, &namebuf, MIN(255,(int)SWAPW(NameRecord.length)));
    namebuf[MIN(255,(int)SWAPW(NameRecord.length))] = '\0';
    lstrcpy(lpszLongName,namebuf);
    lseek (fp, curseek, SEEK_SET);
    }
  }
    close (fp);
    return lpszLongName;
    }
  }
  close (fp);
#else
    DebugString( "portme -- TrueTypeFont::filenameToFontFace\n" );
#endif
  return NULL;
}

