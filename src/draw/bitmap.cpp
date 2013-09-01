#include <precomp.h>

#include "bitmap.h"
#include <api/service/svcs/svc_imgload.h>

#if !defined(WIN32) && !defined(LINUX)
#error port me!
#endif

//CUT: ???
#define ERRORBMP "wasabi.bitmapnotfound"
#define HARDERRORBMP "wacs/wasabi.system/ui/guiobjects/xml/wasabi/window/error.png" 

// do not define NO_MMX in this file. :)
#ifndef NO_MMX

#ifdef WIN32
#define MMX_CONST const
#else
#define MMX_CONST
#endif

unsigned int MMX_CONST Bitmap_mmx_revn2[2]={0x01000100,0x01000100};
unsigned int MMX_CONST Bitmap_mmx_zero[2]={0,0};
unsigned int MMX_CONST Bitmap_mmx_one[2]={1,0};
#define HAS_MMX Blenders::MMX_AVAILABLE()

#else

//NO_MMX defined
#define HAS_MMX 0

#endif

#define INVALID_W 1
#define INVALID_H 1
#define INVALID_COLOR 0xffff00ff
static ARGB32 invalid_bits = INVALID_COLOR;

Bitmap::Bitmap() {
  init();
  makeInvalid();
}

#ifndef _NOSTUDIO

#ifdef WASABI_COMPILE_IMGLDR
Bitmap::Bitmap(HINSTANCE hInstance, int id, const char *forcegroup) {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  bitmapname = "";
  subimage_w=-1;
  subimage_h=-1;
  x_offset=-1;
  y_offset=-1;
  fullimage_w=fullimage_h=0;
  has_alpha = 0;
  ASSERT(hInstance != NULL);
  allocmethod = ALLOC_IMGLDRAPI;
  bits=WASABI_API_IMGLDR->imgldr_makeBmp(hInstance, id,&has_alpha,&fullimage_w,&fullimage_h, forcegroup);
  if (bits == NULL) {
    last_failed = 1;
//    bits = WASABI_API_IMGLDR->imgldr_makeBmp(HARDERRORBMP, &has_alpha, &fullimage_w, &fullimage_h);
makeInvalid();
  } else {
    last_failed = 0;
  }
}
#endif
#endif

//BU> doesn't seem to work?
void Bitmap::bmpToBits(HBITMAP hbmp, HDC defaultDC) {
#ifdef WIN32
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  if (hbmp && !bits) 
  {
    BITMAPINFO srcbmi={0,};
    HDC hMemDC, hMemDC2;
    HBITMAP hprev,hprev2;
    HBITMAP hsrcdib;
    void *srcdib;
    BITMAP bm;
    int r = GetObject(hbmp, sizeof(BITMAP), &bm);
    ASSERT(r != 0);

    fullimage_w=bm.bmWidth;
    fullimage_h=ABS(bm.bmHeight);

    int bmw=getWidth();
    int bmh=getHeight();
    int xo=getX();
    int yo=getY();

    srcbmi.bmiHeader.biSize=sizeof(srcbmi.bmiHeader);
    srcbmi.bmiHeader.biWidth=bmw;
    srcbmi.bmiHeader.biHeight=-bmh;
    srcbmi.bmiHeader.biPlanes=1;
    srcbmi.bmiHeader.biBitCount=32;
    srcbmi.bmiHeader.biCompression=BI_RGB;
    hMemDC = CreateCompatibleDC(NULL);
    hsrcdib=CreateDIBSection(hMemDC,&srcbmi,DIB_RGB_COLORS,&srcdib,NULL,0);
    ASSERTPR(hsrcdib != 0, "CreateDIBSection() failed #6");
    if (defaultDC) 
      hMemDC2 = defaultDC;
    else {
      hMemDC2 = CreateCompatibleDC(NULL);
      hprev2 = (HBITMAP) SelectObject(hMemDC2, hbmp);
    }
    hprev = (HBITMAP) SelectObject(hMemDC, hsrcdib);
    BitBlt(hMemDC,0,0,bmw,bmh,hMemDC2,xo,yo,SRCCOPY);
    SelectObject(hMemDC, hprev);
    if (!defaultDC) {
      SelectObject(hMemDC2, hprev2);
      DeleteDC(hMemDC2);
    }
    DeleteDC(hMemDC);
    bits=(ARGB32*)MALLOC_NOINIT(bmw*bmh*4);
    if (getHeight()+getY() > bm.bmHeight || getWidth()+getX() > bm.bmWidth) {
      ASSERTALWAYS(StringPrintf("Subbitmap coordinates outside master bitmap [%d,%d,%d,%d in 0,0,%d,%d]", getX(), getY(), getWidth(), getHeight(), bm.bmWidth, bm.bmHeight));
    }
    MEMCPY32(bits,srcdib,bmw*bmh);
    DeleteObject(hsrcdib);
    x_offset=-1;
    y_offset=-1;
    subimage_w=-1;
    subimage_h=-1;
    fullimage_w=bmw;
    fullimage_h=bmh;
  }
#elif LINUX
  if ( ! bits ) {
    fullimage_w=hbmp.bmWidth;
    fullimage_h=ABS(hbmp.bmHeight);

    bits=(ARGB32*)MALLOC_NOINIT( fullimage_w * fullimage_h * 4 );
    MEMCPY32(bits, hbmp.shmseginfo->shmaddr, fullimage_w * fullimage_h);
    x_offset=-1;
    y_offset=-1;
    subimage_w=-1;
    subimage_h=-1;
  }
#else
#warning port me: Bitmap::bmpToBits
#endif
}

#ifndef _NOSTUDIO
#ifdef WASABI_COMPILE_IMGLDR
Bitmap::Bitmap(const char *elementname, int _cached) {
  ASSERT(elementname!= NULL);

  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  bitmapname = elementname;
  BOOL isldd = TRUE;
  x_offset = -1;
  y_offset = -1;
  subimage_w = -1;
  subimage_h = -1;
  fullimage_w=fullimage_h=0;
  bits = NULL;
  last_failed = 0;
#ifdef WASABI_COMPILE_SKIN
  bits = WASABI_API_IMGLDR->imgldr_requestBitmap(elementname, &has_alpha, &x_offset, &y_offset, &subimage_w, &subimage_h, &fullimage_w, &fullimage_h,_cached);
  if (bits != NULL) allocmethod = ALLOC_IMGLDRAPI_MANAGED;
  else allocmethod = ALLOC_IMGLDRAPI;
//  if (bits == NULL) bits = WASABI_API_IMGLDR->imgldr_requestBitmap(ERRORBMP, &has_alpha, &x_offset, &y_offset, &subimage_w, &subimage_h, &fullimage_w, &fullimage_h,_cached);
#endif
  if (bits == NULL) bits = WASABI_API_IMGLDR->imgldr_makeBmp(elementname, &has_alpha, &fullimage_w, &fullimage_h);
  if (bits == NULL) {
    bits = WASABI_API_IMGLDR->imgldr_makeBmp(HARDERRORBMP, &has_alpha, &fullimage_w, &fullimage_h);
    last_failed = 1;
  }
  
  // check that coordinates are correct
  if(x_offset!=-1 && x_offset>fullimage_w) x_offset=fullimage_w-1;
  if(y_offset!=-1 && y_offset>fullimage_h) y_offset=fullimage_h-1;
  if(subimage_w!=-1 && (x_offset+subimage_w)>fullimage_w) subimage_w=fullimage_w-x_offset;
  if(subimage_h!=-1 && (y_offset+subimage_h)>fullimage_h) subimage_h=fullimage_h-y_offset;

  // ASSERTPR(bits != NULL, elementname);
  if (bits == NULL) {
    DebugString("element not found ! %s\n", elementname);
    makeInvalid();
  }
}
#endif
#endif

Bitmap::Bitmap(HBITMAP bitmap) {
#ifdef WIN32
  ASSERT(bitmap != NULL);
#endif
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;	// because it's safer

  subimage_w=-1;
  subimage_h=-1;
  x_offset=-1;
  y_offset=-1;
  bitmapname = "";
  fullimage_w=fullimage_h=0;
  has_alpha = 0;
  allocmethod = ALLOC_LOCAL;
  last_failed = 0;
  bmpToBits(bitmap,NULL);
}

Bitmap::Bitmap(HBITMAP bitmap, HDC dc, int _has_alpha, void *_bits) {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  subimage_w=-1;
  subimage_h=-1;
  x_offset=-1;
  y_offset=-1;
  last_failed = 0;
  bitmapname = "";
  fullimage_w=fullimage_h=0;
#ifdef WIN32
  ASSERT(bitmap != NULL);
#endif
  has_alpha = _has_alpha;
  bits = (ARGB32*)_bits;
  if (!_bits) 
  {
    allocmethod = ALLOC_LOCAL;
    bmpToBits(bitmap,dc);
  }
  else
  {
    allocmethod = ALLOC_NONE;
#ifdef WIN32
    BITMAP bm;
    int r = GetObject(bitmap, sizeof(BITMAP), &bm);
    ASSERT(r != 0);
    fullimage_w=bm.bmWidth;
    fullimage_h=ABS(bm.bmHeight);
#endif
#ifdef LINUX
    fullimage_w=bitmap.bmWidth;
    fullimage_h=ABS(bitmap.bmHeight);
#endif
//port me
  }
}

Bitmap::Bitmap(int w, int h, DWORD bgcolor, int noinit) {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  if (w < 0) w = 0;
  if (h < 0) h = 0;

  subimage_w=-1;
  subimage_h=-1;
  x_offset=-1;
  y_offset=-1;
  fullimage_w=w;
  bitmapname = "";
  fullimage_h=h;
  last_failed = 0;

  int memsize = w*h*sizeof(ARGB32);
  if (memsize == 0) memsize++; // +1 so no failure when 0x0
  bits = (ARGB32*)MALLOC_NOINIT(memsize); // no need to zero the mem since we're going to overwrite it with bgcolor
  if (bits == NULL) {
    makeInvalid();
  } else {
    if (!noinit) {
      DWORD *dw = (DWORD *)bits;
      MEMFILL<DWORD>(dw, bgcolor, w*h);
    }

    has_alpha = TRUE;
    allocmethod = ALLOC_LOCAL;
  }
}

Bitmap::Bitmap(ARGB32 *copybits, int bits_w, int bits_h, int subimage_offset_x, int subimage_offset_y, int _subimage_w, int _subimage_h) {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_NONE;

  if (bits_w < 0) bits_w = 0;
  if (bits_h < 0) bits_h = 0;

  x_offset=subimage_offset_x;
  y_offset=subimage_offset_y;
  subimage_w=_subimage_w;
  subimage_h=_subimage_h;
  fullimage_w=bits_w;
  fullimage_h=bits_w;
  bitmapname = "";
  last_failed = 0;

  int memsize = bits_w*bits_h*sizeof(ARGB32);
  if (memsize == 0) memsize++; // +1 so no failure when 0x0
  bits = (ARGB32*)MALLOC_NOINIT(memsize);

  DWORD *dw = (DWORD *)bits;
  ASSERT(copybits != NULL);
  if (copybits != NULL) MEMCPY32(bits, copybits, bits_w*bits_h);

  has_alpha = TRUE;	// who knows, so play it safe
  allocmethod = ALLOC_LOCAL;
}

Bitmap::Bitmap(void *data, int data_len) {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_MEMMGRAPI;

  svc_imageLoader *svc = ImgLoaderEnum(static_cast<unsigned char *>(data), data_len).getNext();
  if (svc == NULL) goto fail;
  bits = svc->loadImage(static_cast<unsigned char *>(data), data_len, &fullimage_w, &fullimage_h);
  x_offset = -1; 
  y_offset = -1;
  subimage_w = -1;
  subimage_h = -1;
  SvcEnum::release(svc);
  if (bits == NULL) {
    fail:
    makeInvalid();
  }
}

Bitmap::Bitmap(const Bitmap *b) {
  bits = NULL;
  allocmethod = ALLOC_NONE;
  invalid = FALSE;

  if(b)
    copyFromBitmap(b);
}

Bitmap::Bitmap(const Bitmap &b) {
  bits = NULL;
  allocmethod = ALLOC_NONE;
  invalid = FALSE;

  copyFromBitmap(&b);
}

Bitmap &Bitmap::operator=(const Bitmap &b) {
  if (this && this != &b) {
    if (bits) freeBitmap();
    bits = NULL;
    allocmethod = ALLOC_NONE;
    invalid = FALSE;
    copyFromBitmap(&b);
  }
  return *this;
}


Bitmap::~Bitmap() {
  if (bits) freeBitmap();
}

void Bitmap::init() {
  bits = NULL;
  invalid = FALSE;
  allocmethod = ALLOC_NONE;
}

void Bitmap::copyFromBitmap(const Bitmap *b) {
  if (this == b) return;	// no-op

  ASSERT(b != NULL);

  freeBitmap();
  
  fullimage_w = b->fullimage_w;
  fullimage_h = b->fullimage_h;
  x_offset = b->x_offset; 
  y_offset = b->y_offset;
  subimage_w = b->subimage_w;
  subimage_h = b->subimage_h;
  has_alpha = b->has_alpha;
  last_failed = b->last_failed;
  bitmapname = b->bitmapname;

  if (fullimage_w <= 0 || fullimage_h <= 0 || b->getBits() == NULL) {
    makeInvalid();
    return;
  }
#ifdef WASABI_COMPILE_SKIN
  if ( b->allocmethod == ALLOC_IMGLDRAPI_MANAGED ) { 
    // add a reference on those bits
    WASABI_API_IMGLDR->imgldr_retainBitmap(b->bits);
    allocmethod = b->allocmethod;
    bits = b->bits;
  }
  else {
#endif
    // copy just the bits we need in our buffer (then suppress the offsets)
    if ( x_offset >= 0 && y_offset >= 0 && subimage_w >0 && subimage_h>0 )  {
      bits = (ARGB32*)MALLOC_NOINIT(subimage_w * subimage_h * sizeof(ARGB32));
      //CUT superfluous int y = y_offset;
      int nb = 0;
      ARGB32* targetbits = b->getBits();
      //-- OPTIMIZE --
      for ( int y = y_offset; y< y_offset+subimage_h; y++ ) {
        MEMCPY32(bits + nb * subimage_w, targetbits + fullimage_w * y + x_offset, subimage_w);
        nb++;
      }
      // -- /OPTIMIZE --
      fullimage_w = subimage_w;
      fullimage_h = subimage_h;
      x_offset = -1; 
      y_offset = -1;
      subimage_w = -1;
      subimage_h = -1;
    }
    else {
      bits = (ARGB32*)MALLOC_NOINIT(fullimage_w * fullimage_h * sizeof(ARGB32));
      MEMCPY32(bits, b->getBits(), fullimage_w * fullimage_h);
    }
    allocmethod = ALLOC_LOCAL;
    has_alpha = b->has_alpha;
#ifdef WASABI_COMPILE_SKIN
  }
#endif
}


void Bitmap::blit(CanvasBase *canvas, int x, int y) {
  RECT src, dst;
  src.left=0;
  src.top=0;
  src.bottom=getHeight();
  src.right=getWidth();
  dst.left=x;
  dst.right=x+getWidth();
  dst.top=y;
  dst.bottom=y+getHeight();
  blitToRect(canvas,&src,&dst,255);
}

void Bitmap::blitRectToTile(CanvasBase *canvas, RECT *dest, RECT *src, int xoffs, int yoffs, int alpha) {
  int startx,starty;

  int w,h;

  w = src->right-src->left;
  h = src->bottom-src->top;
  if (w <= 0 || h <= 0) return;	//wtfmf

  RECT c;
  if (canvas->getClipBox(&c) == NULLREGION) {
    c = *dest;
  } else {
    if (dest->left > c.left) c.left = dest->left;
    if (dest->top > c.top) c.top = dest->top;
    if (dest->right < c.right) c.right = dest->right;
    if (dest->bottom < c.bottom) c.bottom = dest->bottom;
  }


  starty = c.top-((c.top - dest->top) % h)- yoffs;
  startx = c.left-((c.left - dest->left) % w) - xoffs;

  for (int j=starty;j<c.bottom;j+=h)
    for (int i=startx;i<c.right;i+=w) {
      int xp=i;
      int yp=j;
      int xo=0;
      int yo=0;
      int _w=getWidth();
      int _h=getHeight();
      if (xp < c.left) {
        xo=c.left-xp;
        _w+=xo;
        xp=c.left;
      }
      if (yp < c.top) {
        yo=c.top-yp;
        _h+=yo;
        yp=c.top;
      }
      if (xp + _w >= c.right) _w=c.right-xp;
      if (yp + _h >= c.bottom) _h=c.bottom-yp;
      RECT _s={xo, yo, xo+_w, yo+_h};
      RECT _d={xp, yp, xp+_w, yp+_h};
      blitToRect(canvas, &_s, &_d, alpha);
    }
}


void Bitmap::blitTile(CanvasBase *canvas, RECT *dest, int xoffs, int yoffs, int alpha) {
  RECT r={0,0,getWidth(),getHeight()};
  blitRectToTile(canvas, dest, &r, xoffs, yoffs, alpha);
}

#ifdef WIN32
#pragma warning(push) 
#pragma warning(disable : 4799) 
#endif

void Bitmap::blitToRect(CanvasBase *canvas, RECT *src, RECT *dst, int alpha, Blender *blender) { // only dst(top,left) are used
  if (alpha == -1) alpha = 255;
  if (alpha == 0) return;
  if (alpha > 255) alpha = 255;

  //HDC hdc = canvas->getHDC();
  //if (hdc == NULL) return;
  void *dib=canvas->getBits();
  int cwidth,cheight;
  BaseCloneCanvas clone(canvas);
  BltCanvas *hdib = NULL;

  RECT destrect=*dst;
  destrect.bottom=destrect.top+(src->bottom-src->top);
  destrect.right=destrect.left+(src->right-src->left);

  RECT c;
  int ctype=canvas->getClipBox(&c);

  if (ctype != NULLREGION) {
    if (c.top > destrect.top) destrect.top=c.top;
    if (c.left > destrect.left) destrect.left=c.left;
    if (c.bottom < destrect.bottom) destrect.bottom=c.bottom;
    if (c.right < destrect.right) destrect.right=c.right;
  }

#ifdef NO_SIMPLEFASTMODE
  dib=NULL;
#endif

  if (destrect.right <= destrect.left || destrect.bottom <= destrect.top) return;
  int xs,yp,xe,ye;

  if (!dib || canvas->getDim(NULL,&cheight,&cwidth) || !cwidth || cheight < 1 || ctype == COMPLEXREGION)
  {
    cwidth=destrect.right-destrect.left;
    cheight=destrect.bottom-destrect.top;
    hdib = new BltCanvas(cwidth, cheight, 32);
    dib = hdib->getBits();
    if (has_alpha || alpha < 255)
      clone.blit(destrect.left, destrect.top, hdib, 0, 0, cwidth, cheight, alpha);

    xs=0;
    yp=0;
    xe=cwidth;
    ye=cheight;
  }
  else 
  {
    xs=destrect.left;
    xe=destrect.right;
    yp=destrect.top;
    ye=destrect.bottom;

    cwidth/=4;
  }
  int xpo=(dst->left-destrect.left+xs)-(getX()+src->left);
  int ypo=(dst->top-destrect.top+yp)-(getY()+src->top);

  if (yp < 0) yp=0;
  if (xs < 0) xs=0;

  if (yp<getY()+ypo) yp=ypo+getY();
  if (xs<getX()+xpo) xs=xpo+getX();

  if (xe > getWidth()+getX()+xpo) xe=getWidth()+getX()+xpo;
  if (ye > getHeight()+getY()+ypo) ye=getHeight()+getY()+ypo;

  // blend bitmap to dib

#ifndef NO_PREMUL
  if (blender==NULL) {
    if (xs<xe) for (; yp < ye; yp ++) {
      int xp=xe-xs;
      unsigned int *dest=((unsigned int*)dib) + cwidth*yp + xs;
      unsigned int *src=((unsigned int*)bits) + (yp-ypo)*fullimage_w + (xs-xpo);
    
      if (!has_alpha && alpha==255) // simple copy
      {
        MEMCPY(dest,src,xp*4);
      }   
      else if (!has_alpha) { // no alpha channel info, but just a simple blend
        if (!HAS_MMX)
          while (xp--) *dest++ = Blenders::BLEND_ADJ1_FAST_C(*src++, *dest, alpha);
        #ifndef NO_MMX
          else
          {
  #ifdef WIN32
            if (xp>1) __asm
            {
              movd mm3, [alpha]
              mov ecx, xp

              movq mm4, [Bitmap_mmx_revn2]
              packuswb mm3, mm3 // 0000HHVV

              paddusw mm3, [Bitmap_mmx_one]
              mov edi, dest

              punpcklwd mm3, mm3 // HHVVHHVV
              mov esi, src
        
              punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV
              shr ecx, 1

              psubw mm4, mm3

              align 16
              _blitAlpha_Loop1:
  
                movd mm0, [edi]

                movd mm1, [esi]
                punpcklbw mm0, [Bitmap_mmx_zero]
        
                movd mm7, [edi+4]
                punpcklbw mm1, [Bitmap_mmx_zero]

                pmullw mm0, mm4
                pmullw mm1, mm3

                movd mm6, [esi+4]
                punpcklbw mm7, [Bitmap_mmx_zero]
            
                punpcklbw mm6, [Bitmap_mmx_zero]

                pmullw mm7, mm4      
                pmullw mm6, mm3

                paddw mm0, mm1

                psrlw mm0, 8

                packuswb mm0, mm0
                add esi, 8   

                movd [edi], mm0
                paddw mm7, mm6
           
                psrlw mm7, 8
    
                packuswb mm7, mm7

                movd [edi+4], mm7

                add edi, 8

              dec ecx
              jnz _blitAlpha_Loop1
              mov src, esi
              mov dest, edi
  #else
	        if ( xp > 1 ) {
	      __asm__ volatile (
			        "movd %6, %%mm3\n"
			        "mov %2, %%ecx\n"
			        "movq (Bitmap_mmx_revn2), %%mm4\n"
			        "packuswb %%mm3, %%mm3\n"
			        "paddusw (Bitmap_mmx_one), %%mm3\n"
			        "mov %0, %%edi\n"
			        "punpcklwd %%mm3, %%mm3\n"
			        "mov %1, %%esi\n"
			        "punpckldq %%mm3, %%mm3\n"
			        "shr $1, %%ecx\n"
			        "psubw %%mm3, %%mm4\n"
			        ".align 16\n"
			        "_blitAlpha_Loop1:\n"
			        "movd (%%edi), %%mm0\n"
			        "movd (%%esi), %%mm1\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm0\n"
			        "movd 4(%%edi), %%mm7\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm1\n"
			        "pmullw %%mm3, %%mm0\n"
			        "pmullw %%mm4, %%mm1\n"
			        "movd 4(%%esi), %%mm6\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm7\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm6\n"
			        "pmullw %%mm4, %%mm6\n"
			        "pmullw %%mm3, %%mm7\n"
			        "paddw %%mm1, %%mm0\n"
			        "psrlw $8, %%mm0\n"
			        "packuswb %%mm0, %%mm0\n"
			        "add $8, %%esi\n"
			        "movd %%mm0, (%%edi)\n"
			        "paddw %%mm6, %%mm7\n"
			        "psrlw $8, %%mm7\n"
			        "packuswb %%mm7, %%mm7\n"
			        "movd %%mm7, 4(%%edi)\n"
			        "add $8, %%edi\n"
			        "dec %%ecx\n"
			        "jnz _blitAlpha_Loop1\n"
			        "mov %%esi, %1\n"
			        "mov %%edi, %0\n"

			        : "=m" (dest), "=m" (src), "=m" (xp)
			        : "m" (dest), "m" (src), "m" (xp), "m" (alpha)
			        : "%eax", "%ecx", "%esi", "%edi" );

  #endif
            }
            if (xp & 1) *dest++ = Blenders::BLEND_ADJ1_FAST_MMX(*src++, *dest, alpha);
          } // mmx available
        #endif // !NO_MMX
      }
      else if (alpha == 255) { // no global alpha, just alpha channel
        if (!HAS_MMX)
          while (xp--) *dest++ = Blenders::BLEND_ADJ2_FAST_C(*dest, *src++);
        #ifndef NO_MMX
          else
          {
  #ifdef WIN32
            if (xp > 1) __asm
            {
              mov ecx, xp
              shr ecx, 1
              mov edi, dest
              mov esi, src
              align 16
              _blitAlpha_Loop2:

              movd mm3, [esi]
              movd mm5, [esi+4]

              movq mm2, [Bitmap_mmx_revn2]
              psrld mm3, 24

              movq mm4, [Bitmap_mmx_revn2]
              psrld mm5, 24
          
              movd mm0, [edi]
              packuswb mm3, mm3 // 0000HHVV

              movd mm1, [esi]
              packuswb mm5, mm5 // 0000HHVV
          
              movd mm6, [esi+4]
              paddusw mm3, [Bitmap_mmx_one]

              punpcklwd mm3, mm3 // HHVVHHVV
              paddusw mm5, [Bitmap_mmx_one]

              movd mm7, [edi+4]
              punpcklwd mm5, mm5 // HHVVHHVV
        
              punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV
              punpckldq mm5, mm5 // HHVVHHVV HHVVHHVV

              punpcklbw mm6, [Bitmap_mmx_zero]
              psubw mm4, mm5

              punpcklbw mm0, [Bitmap_mmx_zero]
              psubw mm2, mm3

              punpcklbw mm7, [Bitmap_mmx_zero]
              pmullw mm0, mm2
          
              pmullw mm7, mm4      
              punpcklbw mm1, [Bitmap_mmx_zero]

              psubw mm2, mm3

              psrlw mm0, 8
              psrlw mm7, 8
              paddw mm0, mm1
   
              paddw mm7, mm6
              packuswb mm0, mm0

              movd [edi], mm0
              packuswb mm7, mm7
                 
              movd [edi+4], mm7

              add esi, 8
              add edi, 8

              dec ecx
              jnz _blitAlpha_Loop2
              mov src, esi
              mov dest, edi
  #else
	  if( xp > 1 ) {
	      __asm__ volatile (
			        "mov %4, %%ecx\n"
			        "shr $1, %%ecx\n"
			        "mov %0, %%edi\n"
			        "mov %1, %%esi\n"
			        ".align 16\n"
			        "_blitAlpha_Loop2:\n"
			        "movd (%%esi), %%mm3\n"
			        "movd 4(%%esi), %%mm5\n"
			        "movq (Bitmap_mmx_revn2), %%mm2\n"
			        "psrld $24, %%mm3\n"
			        "movq (Bitmap_mmx_revn2), %%mm4\n"
			        "psrld $24, %%mm5\n"
			        "movd (%%edi), %%mm0\n"
			        "packuswb %%mm3, %%mm3\n"
			        "movd (%%esi), %%mm1\n"
			        "packuswb %%mm5, %%mm5\n"
			        "movd 4(%%esi), %%mm6\n"
			        "paddusw (Bitmap_mmx_one), %%mm3\n"
			        "punpcklwd %%mm3, %%mm3\n"
			        "paddusw (Bitmap_mmx_one), %%mm5\n"
 			        "movd 4(%%edi), %%mm7\n"
			        "punpcklwd %%mm5, %%mm5\n"
			        "punpckldq %%mm3, %%mm3\n"
			        "punpckldq %%mm5, %%mm5\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm6\n"
			        "psubw %%mm5, %%mm4\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm0\n"
			        "psubw %%mm3, %%mm2\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm7\n"
			        "pmullw %%mm2, %%mm0\n"
			        "pmullw %%mm4, %%mm7\n"
              "punpcklbw (Bitmap_mmx_zero), %%mm1\n"
			        "psubw %%mm3, %%mm2\n"
			        "psrlw $8, %%mm0\n"
			        "psrlw $8, %%mm7\n"
			        "paddw %%mm1, %%mm0\n"
			        "paddw %%mm6, %%mm7\n"
			        "packuswb %%mm0, %%mm0\n"
			        "movd %%mm0, (%%edi)\n"
			        "packuswb %%mm7, %%mm7\n"
			        "movd %%mm7, 4(%%edi)\n"
			        "add $8, %%esi\n"
			        "add $8, %%edi\n"
			        "dec %%ecx\n"
			        "jnz _blitAlpha_Loop2\n"
			        "mov %%esi, %1\n"
			        "mov %%edi, %0\n"

			        : "=m" (dest), "=m" (src)
			        : "m" (dest), "m" (src), "m" (xp)
			        : "%eax", "%ecx", "%esi", "%edi" );
  #endif
            }
            if (xp&1) *dest++ = Blenders::BLEND_ADJ2_FAST_MMX(*dest, *src++);
          } // HAS_MMX
        #endif // ifndef NO_MMX
      }
      else { // both
        if (!HAS_MMX)
          while (xp--) *dest++ = Blenders::BLEND_ADJ3_FAST_C(*dest, *src++, alpha);
        #ifndef NO_MMX
          else
          {
  #ifdef WIN32
            if (xp > 1) __asm
            {
              movd mm5, [alpha]
              mov ecx, xp

              packuswb mm5, mm5 
              shr ecx, 1

              paddusw mm5, [Bitmap_mmx_one]

              punpcklwd mm5, mm5        
              mov edi, dest

              punpckldq mm5, mm5
              mov esi, src

              align 16
              _blitAlpha_Loop3:

              movd mm3, [esi] // VVVVVVVV
              movd mm4, [esi+4] // VVVVVVVV

              movd mm0, [edi]    
              psrld mm3, 24

              movd mm1, [esi]
              psrld mm4, 24

              paddusw mm3, [Bitmap_mmx_one]
              paddusw mm4, [Bitmap_mmx_one]

              movd mm7, [edi+4]    
              punpcklwd mm3, mm3

              movd mm6, [esi+4]
              punpcklwd mm4, mm4

              punpckldq mm3, mm3
              punpckldq mm4, mm4
          
              pmullw mm3, mm5
              pmullw mm4, mm5

              punpcklbw mm7, [Bitmap_mmx_zero]
              punpcklbw mm6, [Bitmap_mmx_zero]

              movq mm2, [Bitmap_mmx_revn2]
              psrlw mm3, 8

              psrlw mm4, 8  

              punpcklbw mm0, [Bitmap_mmx_zero]
              punpcklbw mm1, [Bitmap_mmx_zero]

              psubw mm2, mm3
              pmullw mm0, mm2      

              pmullw mm1, mm5
              add esi, 8

              movq mm2, [Bitmap_mmx_revn2]
              pmullw mm6, mm5
    
              paddusw mm0, mm1
              psubw mm2, mm4

              pmullw mm7, mm2      
              psrlw mm0, 8   

              packuswb mm0, mm0
              paddusw mm7, mm6

              movd [edi], mm0
              psrlw mm7, 8   
                 
              packuswb mm7, mm7

              movd [edi+4], mm7

              add edi, 8

              dec ecx
              jnz _blitAlpha_Loop3
              mov src, esi
              mov dest, edi
  #else
	    if ( xp > 1 ) {
	      __asm__ volatile (
			        "movd %5, %%mm5\n"
			        "mov %4, %%ecx\n"
			        "packuswb %%mm5, %%mm5 \n"
			        "shr $1, %%ecx\n"
			        "paddusw (Bitmap_mmx_one), %%mm5\n"
			        "punpcklwd %%mm5, %%mm5\n"
			        "mov %0, %%edi\n"
			        "punpckldq %%mm5, %%mm5\n"
			        "mov %1, %%esi\n"
			        ".align 16\n"
			        "_blitAlpha_Loop3:\n"
			        "movd (%%esi), %%mm3\n"
			        "movd 4(%%esi), %%mm4\n"
			        "movd (%%edi), %%mm0\n"
			        "psrld $24, %%mm3\n"
			        "movd (%%esi), %%mm1\n"
			        "psrld $24, %%mm4\n"
			        "paddusw (Bitmap_mmx_one), %%mm3\n"
			        "paddusw (Bitmap_mmx_one), %%mm4\n"
			        "movd 4(%%edi), %%mm7\n"
			        "punpcklwd %%mm3, %%mm3\n"
			        "movd 4(%%esi), %%mm6\n"
			        "punpcklwd %%mm4, %%mm4\n"
			        "punpckldq %%mm3, %%mm3\n"
			        "punpckldq %%mm4, %%mm4\n"
			        "pmullw %%mm5, %%mm3\n"
			        "pmullw %%mm5, %%mm4\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm7\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm6\n"
			        "movq (Bitmap_mmx_revn2), %%mm2\n"
			        "psrlw $8, %%mm3\n"
			        "psrlw $8, %%mm4\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm0\n"
			        "punpcklbw (Bitmap_mmx_zero), %%mm1\n"
			        "psubw %%mm3, %%mm2\n"
			        "pmullw %%mm2, %%mm0\n"
			        "pmullw %%mm5, %%mm1\n"
			        "add $8, %%esi\n"
			        "movq (Bitmap_mmx_revn2), %%mm2\n"
			        "pmullw %%mm5, %%mm6\n"
			        "paddusw %%mm1, %%mm0\n"
			        "psubw %%mm4, %%mm2\n"
			        "pmullw %%mm2, %%mm7\n"
			        "psrlw $8, %%mm0\n"
			        "packuswb %%mm0, %%mm0\n"
			        "paddusw %%mm6, %%mm7\n"
			        "movd %%mm0, (%%edi)\n"
			        "psrlw $8, %%mm7\n"
			        "packuswb %%mm7, %%mm7\n"
			        "movd %%mm7, 4(%%edi)\n"
			        "add $8, %%edi\n"
			        "dec %%ecx\n"
			        "jnz _blitAlpha_Loop3\n"
			        "mov %%esi, %1\n"
			        "mov %%edi, %0\n"

			        : "=m" (dest), "=m" (src)
			        : "m" (dest), "m" (src), "m" (xp), "m" (alpha)
			        : "%eax", "%ecx", "%esi", "%edi" );
  #endif
            }
            if (xp&1) *dest++ = Blenders::BLEND_ADJ3_FAST_MMX(*dest, *src++, alpha);
          } // HAS_MMX
        #endif // ifndef NO_MMX
      }
    }
  #ifndef NO_MMX
    Blenders::BLEND_MMX_END();
  #endif
  } else {
#endif
    Blender *b = blender ? blender : &g_blend_normal;
    if (xs<xe) {
      for (; yp < ye; yp ++) {
        int xp=xe-xs;
        unsigned long *dest=((unsigned long *)dib) + cwidth*yp + xs;
        unsigned long *src=((unsigned long *)bits) + (yp-ypo)*fullimage_w + (xs-xpo);
        b->blendLine(dest, src, alpha, xp, xs, yp, xs-xpo, yp-ypo);
      }
      #ifndef NO_MMX
      Blenders::BLEND_MMX_END();
      #endif
    }
#ifndef NO_PREMUL
  }
#endif

  // write bits back to dib.
  if (hdib) {
    hdib->blit(0, 0, &clone, destrect.left, destrect.top, cwidth, cheight);
    delete hdib;
  }
}

#ifdef WIN32
#pragma warning(pop) 
#endif

void Bitmap::stretch(CanvasBase *canvas, int x, int y, int w, int h, Blender *blender) {
  RECT src, dst;
  src.left=0;
  src.top=0;
  src.right=getWidth();
  src.bottom=getHeight();
  dst.left=x;
  dst.right=x+w;
  dst.top=y;
  dst.bottom=y+h;
  stretchToRectAlpha(canvas,&src,&dst,255, blender);
}

void Bitmap::stretchToRect(CanvasBase *canvas, RECT *r, Blender *blender) {
  stretch(canvas, r->left, r->top, r->right - r->left, r->bottom - r->top, blender);
}

void Bitmap::stretchRectToRect(CanvasBase *canvas, RECT *src, RECT *dst, Blender *blender) {
  stretchToRectAlpha(canvas,src,dst,255, blender);
}


void Bitmap::stretchToRectAlpha(CanvasBase *canvas, RECT *r, int alpha, Blender *blender) {
  RECT re;
  re.left=0; re.top=0;
  re.right=getWidth(); re.bottom=getHeight();
  stretchToRectAlpha(canvas,&re,r,alpha, blender);
}

void Bitmap::blitAlpha(CanvasBase *canvas, int x, int y, int alpha, Blender *blender)
{
  RECT dst,src;
  dst.left=x;
  dst.top=y;
  src.left=0;
  src.top=0;
  src.bottom=getHeight();
  src.right=getWidth();
  blitToRect(canvas,&src,&dst,alpha, blender);
}

#ifdef WIN32
#pragma warning(push) 
#pragma warning(disable : 4799) 
#endif

template <class C>
class Stretcher {
public:
  static void _stretchToRectAlpha(Bitmap *bitmap, int ys, int ye, int xe, int xs, int xstart, int yv, void *dib, int cwidth, int dxv, int dyv, int alpha, Blender *blender) {
    int bitmap_x = bitmap->getX();
    int bitmap_y = bitmap->getY();
    int bmpheight = bitmap->getHeight();
    int fullimage_w = bitmap->getFullWidth();
    void *bits = bitmap->getBits();
    int xp=xe-xs;
    for (int yp = ys; yp < ye; yp ++) {
      int t=yv>>16;
      if (t < 0) t=0;
      if (t >= bmpheight) t=bmpheight-1;
      int *psrc=((int*)bits) + (t+bitmap_y)*fullimage_w + bitmap_x;
      int *dest=((int*)dib) + cwidth*yp + xs;     

      C::stretch(xp, psrc, dest, xstart, dxv, alpha, blender);

      yv+=dyv;
    }
#ifndef NO_MMX
  Blenders::BLEND_MMX_END();
#endif
  }
};

class StretchBlender {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    blender->stretchLine(xp, psrc, dest, xv, dxv, alpha);
  }
};


// no alpha, just stretch
class Stretch {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) { //JFtodo: assembly optimize - these first two modes aren't used that much anyway
      *dest++ = psrc[xv>>16];
      xv+=dxv;
    }
  }
};

// no alpha channel, just a global alpha val
class StretchGlobal_FAST_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) { //JFTODO: make MMX optimized version
      *dest++ = Blenders::BLEND_ADJ1_FAST_C(psrc[xv>>16], *dest, alpha);
      xv+=dxv;
    }
  }
};

// alpha channel, no global alpha val
class StretchChannel_FAST_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ2_FAST_C(*dest, psrc[xv>>16]);
      xv+=dxv;
    }
  }
};

class StretchGlobalChannel_FAST_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ3_FAST_C(*dest, psrc[xv>>16], alpha);
      xv+=dxv;
    }
  }
};


#ifndef NO_MMX

// no alpha channel, just a global alpha val
class StretchGlobal_FAST_MMX {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) { //JFTODO: make MMX optimized version
      *dest++ = Blenders::BLEND_ADJ1_FAST_MMX(psrc[xv>>16], *dest, alpha);
      xv+=dxv;
    }
  }
};


#ifndef NO_PREMUL

// alpha channel, no global alpha val
class StretchChannel_FAST_MMX {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
#ifdef WIN32
    if (xp>1) __asm
    {
      mov ecx, xp
      mov edi, dest

      shr ecx, 1
      mov esi, psrc

      mov edx, xv
      mov ebx, dxv

      align 16
    _stretchAlpha_Loop2:

      mov eax, edx
      movd mm0, [edi]

      movq mm4, [Bitmap_mmx_revn2]
      shr eax, 16

      movq mm2, [Bitmap_mmx_revn2]
      punpcklbw mm0, [Bitmap_mmx_zero]

      movd mm3, [esi+eax*4]
      movd mm1, [esi+eax*4]
      
      lea eax, [edx+ebx]
      shr eax, 16

      movd mm7, [edi+4]
      psrld mm3, 24

      packuswb mm3, mm3 // 0000HHVV
      movd mm5, [esi+eax*4]

      movd mm6, [esi+eax*4]
      psrld mm5, 24          

      paddusw mm3, [Bitmap_mmx_one]
      punpcklbw mm6, [Bitmap_mmx_zero]

      packuswb mm5, mm5 // 0000HHVV
      lea edx, [edx+ebx*2]
        
      paddusw mm5, [Bitmap_mmx_one]          
      punpcklwd mm3, mm3 // HHVVHHVV

      punpcklwd mm5, mm5 // HHVVHHVV
      add edi, 8
      
      punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV

      punpckldq mm5, mm5 // HHVVHHVV HHVVHHVV

      psubw mm4, mm5

      psubw mm2, mm3

      punpcklbw mm7, [Bitmap_mmx_zero]
      pmullw mm0, mm2
        
      pmullw mm7, mm4      
      punpcklbw mm1, [Bitmap_mmx_zero]

      psubw mm2, mm3

      psrlw mm0, 8
      psrlw mm7, 8
      paddw mm0, mm1
 
      paddw mm7, mm6
      packuswb mm0, mm0

      movd [edi-8], mm0
      packuswb mm7, mm7
                 
      movd [edi-4], mm7

      dec ecx
      jnz _stretchAlpha_Loop2
      mov dest, edi
      mov xv, edx
    }
#else
    if (xp>1)
    {
      __asm__ volatile (
			"mov %5, %%ecx\n"
			"mov %0, %%edi\n"
			"shr $1, %%ecx\n"
			"mov %1, %%esi\n"
			"mov %2, %%edx\n"
			"mov %7, %%ebx\n"
			".align 16\n"
			"_stretchAlpha_Loop2:\n"
			"mov %%edx, %%eax\n"
			"movd (%%edi), %%mm0\n"
			"movq (Bitmap_mmx_revn2), %%mm4\n"
			"shr $16, %%eax\n"
			"movq (Bitmap_mmx_revn2), %%mm2\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm0\n"
			"movd (%%esi,%%eax,4), %%mm3\n"
			"movd (%%esi,%%eax,4), %%mm1\n"
			"lea (%%edx,%%ebx), %%eax\n"
			"shr $16, %%eax\n"
			"movd 4(%%edi), %%mm7\n"
			"psrld $24, %%mm3\n"
			"packuswb %%mm3, %%mm3\n"
			"movd (%%esi,%%eax,4), %%mm5\n"
			"movd (%%esi,%%eax,4), %%mm6\n"
			"psrld $24, %%mm5\n"
			"paddusw (Bitmap_mmx_one), %%mm3\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm6\n"
			"packuswb %%mm5, %%mm5\n"
			"lea (%%edx,%%ebx,2), %%edx\n"
			"paddusw (Bitmap_mmx_one), %%mm5\n"
			"punpcklwd %%mm3, %%mm3\n"
			"punpcklwd %%mm5, %%mm5\n"
			"add $8, %%edi\n"
			"punpckldq %%mm3, %%mm3\n"
			"punpckldq %%mm5, %%mm5\n"
			"psubw %%mm5, %%mm4\n"
			"psubw %%mm3, %%mm2\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm7\n"
			"pmullw %%mm2, %%mm0\n"
			"pmullw %%mm4, %%mm7\n"     
			"punpcklbw (Bitmap_mmx_zero), %%mm1\n"
			"psubw %%mm3, %%mm2\n"
			"psrlw $8, %%mm0\n"
			"psrlw $8, %%mm7\n"
			"paddw %%mm1, %%mm0\n"
			"paddw %%mm6, %%mm7\n"
			"packuswb %%mm0, %%mm0\n"
			"movd %%mm0, -8(%%edi)\n"
			"packuswb %%mm7, %%mm7\n"
			"movd %%mm7, -4(%%edi)\n"
			"dec %%ecx\n"
			"jnz _stretchAlpha_Loop2\n"
			"mov %%edi, %0\n"
			"mov %%edx, %2\n"

			: "=m" (dest), "=m" (psrc), "=m" (xv)
			: "m" (dest), "m" (psrc), "m" (xp),
			  "m" (xv), "m" (dxv), "m" (alpha)
			: "%eax", "%ebx", "%ecx", "%edx",
			  "%esi", "%edi" );

    }
#endif

    if (xp&1) *dest++ = Blenders::BLEND_ADJ2_FAST_MMX(*dest, psrc[xv>>16]);
  }
};


class StretchGlobalChannel_FAST_MMX {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
#ifdef WIN32
    if (xp>1) __asm
    {
      movd mm5, [alpha]
      mov ecx, xp

      packuswb mm5, mm5 
      shr ecx, 1

      paddusw mm5, [Bitmap_mmx_one]

      punpcklwd mm5, mm5        
      mov edi, dest

      punpckldq mm5, mm5
      mov esi, psrc

      mov edx, xv
      mov ebx, dxv

      align 16
    _stretchAlpha_Loop3:
      movd mm0, [edi]    
      mov eax, edx

      movd mm7, [edi+4]    
      shr eax, 16

      movd mm1, [esi+eax*4]
      movd mm3, [esi+eax*4] // VVVVVVVV

      lea eax, [edx+ebx]
      psrld mm3, 24

      paddusw mm3, [Bitmap_mmx_one]

      punpcklwd mm3, mm3
      shr eax, 16

      punpckldq mm3, mm3

      pmullw mm3, mm5

      movd mm4, [esi+eax*4] // VVVVVVVV
      movd mm6, [esi+eax*4]

      movq mm2, [Bitmap_mmx_revn2]
      psrld mm4, 24

      paddusw mm4, [Bitmap_mmx_one]
      punpcklbw mm7, [Bitmap_mmx_zero]

      punpcklwd mm4, mm4
      lea edx, [edx+ebx*2]

      punpckldq mm4, mm4
      add edi, 8

      punpcklbw mm6, [Bitmap_mmx_zero]         
      pmullw mm4, mm5

      psrlw mm3, 8

      punpcklbw mm0, [Bitmap_mmx_zero]

      punpcklbw mm1, [Bitmap_mmx_zero]
      psubw mm2, mm3

      pmullw mm0, mm2      
      pmullw mm1, mm5

      pmullw mm6, mm5
      psrlw mm4, 8  

      movq mm2, [Bitmap_mmx_revn2]    
      paddusw mm0, mm1
      psubw mm2, mm4

      pmullw mm7, mm2      
      psrlw mm0, 8   

      packuswb mm0, mm0
      paddusw mm7, mm6

      movd [edi-8], mm0
      psrlw mm7, 8   
             
      packuswb mm7, mm7

      movd [edi-4], mm7

      dec ecx
      jnz _stretchAlpha_Loop3
      mov xv, edx
      mov dest, edi
    }
#else
    if (xp>1) 
    {
      __asm__ volatile (
			"movd %8, %%mm5\n"
			"mov %5, %%ecx\n"
			"packuswb %%mm5, %%mm5 \n"
			"shr $1, %%ecx\n"
			"paddusw (Bitmap_mmx_one), %%mm5\n"
			"punpcklwd %%mm5, %%mm5\n"
			"mov %0, %%edi\n"
			"punpckldq %%mm5, %%mm5\n"
			"mov %1, %%esi\n"
			"mov %6, %%edx\n"
			"mov %7, %%ebx\n"
			".align 16\n"
			"_stretchAlpha_Loop3:\n"
			"movd (%%edi), %%mm0\n"
			"mov %%edx, %%eax\n"
			"movd 4(%%edi), %%mm7\n"
			"shr $16, %%eax\n"
			"movd (%%esi,%%eax,4), %%mm1\n"
			"movd (%%esi,%%eax,4), %%mm3\n"
			"lea (%%edx,%%ebx), %%eax\n"
			"psrld $24, %%mm3\n"
			"paddusw (Bitmap_mmx_one), %%mm3\n"
			"punpcklwd %%mm3, %%mm3\n"
			"shr $16, %%eax\n"
			"punpckldq %%mm3, %%mm3\n"
			"pmullw %%mm5, %%mm3\n"
			"movd (%%esi,%%eax,4), %%mm4\n"
			"movd (%%esi,%%eax,4), %%mm6\n"
			"movq (Bitmap_mmx_revn2), %%mm2\n"
			"psrld $24, %%mm4\n"
			"paddusw (Bitmap_mmx_one), %%mm4\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm7\n"
			"punpcklwd %%mm4, %%mm4\n"
			"lea (%%edx,%%ebx,2), %%edx\n"
			"punpckldq %%mm4, %%mm4\n"
			"add $8, %%edi\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm6\n"
			"pmullw %%mm5, %%mm4\n"
			"psrlw $8, %%mm3\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm0\n"
			"punpcklbw (Bitmap_mmx_zero), %%mm1\n"
			"psubw %%mm3, %%mm2\n"
			"pmullw %%mm2, %%mm0\n"      
			"pmullw %%mm5, %%mm1\n"
			"pmullw %%mm5, %%mm6\n"
			"psrlw $8, %%mm4\n"
			"movq (Bitmap_mmx_revn2), %%mm2\n"
			"paddusw %%mm1, %%mm0\n"
			"psubw %%mm4, %%mm2\n"
			"pmullw %%mm2, %%mm7\n"      
			"psrlw $8, %%mm0\n"
			"packuswb %%mm0, %%mm0\n"
			"paddusw %%mm6, %%mm7\n"
			"movd %%mm0, -8(%%edi)\n"
			"psrlw $8, %%mm7\n"
			"packuswb %%mm7, %%mm7\n"
			"movd %%mm7, -4(%%edi)\n"
			"dec %%ecx\n"
			"jnz _stretchAlpha_Loop3\n"
			"mov %%edi, %0\n"
			"mov %%edx, %2\n"

			: "=m" (dest), "=m" (psrc), "=m" (xv)
			: "m" (dest), "m" (psrc), "m" (xp),
			  "m" (xv), "m" (dxv), "m" (alpha)
			: "%eax", "%ebx", "%ecx", "%edx",
			  "%esi", "%edi" );

    }
#endif

    if (xp&1) *dest++ = Blenders::BLEND_ADJ3_FAST_MMX(*dest, psrc[xv>>16], alpha);
  }
};

#else // #ifn no_premul

class StretchGlobalChannel_FAST_MMX {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ3_FAST_MMX(*dest, psrc[xv>>16], alpha);
      xv+=dxv;
    }
  }
};

// alpha channel, no global alpha val
class StretchChannel_FAST_MMX {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ2_FAST_MMX(*dest, psrc[xv>>16]);
      xv+=dxv;
    }
  }
};

#endif // #ifn no_premul

#endif // #if no mmx

#ifdef NO_PREMUL
// no alpha channel, just a global alpha val
class StretchGlobal_PRECISE_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) { //JFTODO: make MMX optimized version
      *dest++ = Blenders::BLEND_ADJ1_PRECISE_C(psrc[xv>>16], *dest, alpha);
      xv+=dxv;
    }
  }
};

// alpha channel, no global alpha val
class StretchChannel_PRECISE_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ2_PRECISE_C(*dest, psrc[xv>>16]);
      xv+=dxv;
    }
  }
};

class StretchGlobalChannel_PRECISE_C {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ3_PRECISE_C(*dest, psrc[xv>>16], alpha);
      xv+=dxv;
    }
  }
};
#ifndef NO_SSE
// no alpha channel, just a global alpha val
class StretchGlobal_PRECISE_SSE {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) { //JFTODO: make MMX optimized version
      *dest++ = Blenders::BLEND_ADJ1_PRECISE_SSE(psrc[xv>>16], *dest, alpha);
      xv+=dxv;
    }
  }
};

// alpha channel, no global alpha val
class StretchChannel_PRECISE_SSE {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ2_PRECISE_SSE(*dest, psrc[xv>>16]);
      xv+=dxv;
    }
  }
};

class StretchGlobalChannel_PRECISE_SSE {
public:
  static void stretch(int xp, int *psrc, int *dest, int xv, int dxv, int alpha, Blender *blender) {
    while (xp--) {
      *dest++ = Blenders::BLEND_ADJ3_PRECISE_SSE(*dest, psrc[xv>>16], alpha);
      xv+=dxv;
    }
  }
};
#endif
#endif


class __Stretch : public Stretcher<Stretch> {};
class __StretchBlender : public Stretcher<Stretch> {};
class __StretchGlobal_FAST_C : public Stretcher<StretchGlobal_FAST_C> {};
class __StretchChannel_FAST_C : public Stretcher<StretchChannel_FAST_C> {};
class __StretchGlobalChannel_FAST_C : public Stretcher<StretchGlobalChannel_FAST_C> {};
#ifndef NO_MMX
class __StretchGlobal_FAST_MMX : public Stretcher<StretchGlobal_FAST_MMX> {};
class __StretchChannel_FAST_MMX : public Stretcher<StretchChannel_FAST_MMX> {};
class __StretchGlobalChannel_FAST_MMX : public Stretcher<StretchGlobalChannel_FAST_MMX> {};
#endif

#ifdef NO_PREMUL
class __StretchGlobal_PRECISE_C : public Stretcher<StretchGlobal_PRECISE_C> {};
class __StretchChannel_PRECISE_C : public Stretcher<StretchChannel_PRECISE_C> {};
class __StretchGlobalChannel_PRECISE_C : public Stretcher<StretchGlobalChannel_PRECISE_C> {};
#ifndef NO_SSE
class __StretchGlobal_PRECISE_SSE : public Stretcher<StretchGlobal_PRECISE_SSE> {};
class __StretchChannel_PRECISE_SSE : public Stretcher<StretchChannel_PRECISE_SSE> {};
class __StretchGlobalChannel_PRECISE_SSE : public Stretcher<StretchGlobalChannel_PRECISE_SSE> {};
#endif
#endif

#ifdef WIN32
#pragma warning(pop) 
#endif


void Bitmap::stretchToRectAlpha(CanvasBase *canvas, RECT *_src, RECT *_dst, int alpha, Blender *blender)
{
  if (alpha <= 0) return;
  if (alpha > 255) alpha = 255;

  RECT src=*_src;

  RECT dst=*_dst;

  if ((src.right-src.left) == (dst.right-dst.left) &&
      (src.bottom-src.top) == (dst.bottom-dst.top))
  {
    blitToRect(canvas,_src,_dst,alpha);
    return;
  }

  if (src.left >= src.right || src.top >= src.bottom) return;
  if (dst.left >= dst.right || dst.top >= dst.bottom) return;

  void *dib=canvas->getBits();
  HDC hdc=canvas->getHDC();
  BltCanvas *hdib = NULL;
  BaseCloneCanvas clone(canvas);
  int cwidth, cheight;

  int dyv=((src.bottom-src.top)<<16)/(dst.bottom-dst.top);
  int dxv=((src.right-src.left)<<16)/(dst.right-dst.left);  
  int yv=(src.top<<16);
  int xstart=(src.left<<16);

  RECT c;
  int ctype=canvas->getClipBox(&c);
  if (c.top > dst.top) 
  {
    yv+=(c.top-dst.top)*dyv;
    dst.top=c.top;
  }
  if (c.left > dst.left)
  {
    xstart+=(c.left-dst.left)*dxv;
    dst.left=c.left;
  }
  if (c.bottom < dst.bottom) dst.bottom=c.bottom;
  if (c.right < dst.right) dst.right=c.right;

  if (dst.right <= dst.left || dst.bottom <= dst.top) return;

  int xs,xe,ys,ye;

#ifdef NO_SIMPLEFASTMODE
  dib=NULL;
#endif
  if (!dib || canvas->getDim(NULL,&cheight,&cwidth) || !cwidth || cheight < 1 || ctype == COMPLEXREGION)
  {
    cwidth=dst.right-dst.left;
    cheight=dst.bottom-dst.top;
    hdib = Canvas::makeTempCanvas(cwidth, cheight);
    dib = hdib->getBits();
    if ( has_alpha || alpha < 255 )
      clone.blit( dst.left, dst.top, hdib, 0, 0, cwidth, cheight );

    xs=0;
    ys=0;
    xe=cwidth;
    hdib->getDim(NULL, NULL, &cwidth);
    cwidth /= 4;
    ye=cheight;
  }
  else 
  {
    xs=dst.left;
    xe=dst.right;
    ys=dst.top;
    ye=dst.bottom;
    cwidth/=4;
  }

  // stretch and blend bitmap to dib

  if (xstart < 0) xstart=0;

  if (!blender) {
    if (xs<xe) {
      if (!has_alpha) {	// doesn't have alpha channel
        if (alpha == 255) {	// no global alpha
          __Stretch::_stretchToRectAlpha(this, ys, ye, xe, xs, xstart, yv, dib, cwidth, dxv, dyv, alpha, NULL);
        } else {	// has global alpha
          #undef BLEND_JOB
          #define BLEND_JOB(version) __StretchGlobal##version::_stretchToRectAlpha(this, ys, ye, xe, xs, xstart, yv, dib, cwidth, dxv, dyv, alpha, NULL);
          MAKE_BLEND_JOB;
        }
      } else {	// has alpha channel
        if (alpha == 255) {	// no global alpha
          #undef BLEND_JOB
          #define BLEND_JOB(version) __StretchChannel##version::_stretchToRectAlpha(this, ys, ye, xe, xs, xstart, yv, dib, cwidth, dxv, dyv, alpha, NULL);
          MAKE_BLEND_JOB;
        } else {	// has global alpha
          #undef BLEND_JOB
          #define BLEND_JOB(version) __StretchGlobalChannel##version::_stretchToRectAlpha(this, ys, ye, xe, xs, xstart, yv, dib, cwidth, dxv, dyv, alpha, NULL);
          MAKE_BLEND_JOB;
        }
      }
    }
  } else {
    if (xs<xe) {
      __StretchBlender::_stretchToRectAlpha(this, ys, ye, xe, xs, xstart, yv, dib, cwidth, dxv, dyv, alpha, blender);
    }
  }


#ifndef NO_MMX
  Blenders::BLEND_MMX_END();
#endif
  // write bits back to dib.

  if (hdib) {
    hdib->blit(0, 0, &clone, dst.left, dst.top, cwidth, cheight);
    Canvas::releaseTempCanvas(hdib);
  }
}

COLORREF Bitmap::getPixel(int x, int y) {
  ASSERT(bits != NULL);
  if (x < 0 || y < 0 || x >= getFullWidth()-getX() || y>= getFullHeight()-getY()) return (COLORREF)0;
  return (COLORREF)(((int*)bits)[x+getX()+(y+getY())*getFullWidth()]);
}

ARGB32 *Bitmap::getBits() const {
  return bits;
}

int Bitmap::isInvalid() {
  return last_failed || invalid;
}

void Bitmap::setHasAlpha(int ha) { 
  has_alpha=ha; 
}

const char *Bitmap::getBitmapName() {
  return bitmapname;
}

int Bitmap::setPixel(int x, int y, ARGB32 color) {
//FUCKO: maybe should check against subbitmap coords instead
  if (x < 0 || y < 0 || x >= fullimage_w || y >= fullimage_h) return 0;
  if (bits == NULL) return 0;
  bits[x + y*fullimage_w] = color;
  return 1;
}

void Bitmap::setAllPixels(ARGB32 color) {
//FUCKO: make aware of subbitmap stuff
  if (bits == NULL) return;
  MEMFILL<ARGB32>(bits, color, getWidth() * getHeight());
}

void Bitmap::makeInvalid() {
  // fall back to 1x1 magenta in case of load error
  invalid = TRUE;
  freeBitmap();

  // we use a static ARGB32 invalidbits and ALLOC_NONE

  bits = &invalid_bits;
  MEMFILL<ARGB32>(bits, INVALID_COLOR, INVALID_W*INVALID_H);
  allocmethod = ALLOC_NONE;
  fullimage_w = INVALID_W;
  fullimage_h = INVALID_H;
  x_offset = -1; 
  y_offset = -1;
  subimage_w = -1;
  subimage_h = -1;
}

void Bitmap::freeBitmap() {
  if (bits) {
    switch (allocmethod) {
      case ALLOC_NONE: /* do nothing */ break;
      case ALLOC_LOCAL:     FREE(bits); break;
#ifdef WASABI_API_MEMMGR
      case ALLOC_MEMMGRAPI: WASABI_API_MEMMGR->sysFree(bits); break;
#else
      // THIS IS ONLY SAFE IF YOUR IMGLOADERS ARE NOT IN WACS
      // IT WILL CRASH OTHERWISE. In this case, just enable the MEMMGR api
      case ALLOC_MEMMGRAPI: FREE(bits); break;
#endif
#ifdef WASABI_API_IMGLDR
      case ALLOC_IMGLDRAPI: WASABI_API_IMGLDR->imgldr_releaseBmp(bits); break;
#ifdef WASABI_API_SKIN
      case ALLOC_IMGLDRAPI_MANAGED: WASABI_API_IMGLDR->imgldr_releaseBitmap(bits); break;
#endif
#endif
    }
  }

  bits = NULL;
  allocmethod = ALLOC_NONE;
  invalid = 0;
}

void Bitmap::getBoundingBox(RECT *r) {
  int w = subimage_w;
  int h = subimage_h;
  if (w == -1) w = fullimage_w;
  if (h == -1) h = fullimage_h;
  r->left = w; r->top = h;
  r->right = r->bottom = -1;
  for (int y=0;y<h;y++) {
    ARGB32 *ptr = getBits()+y*fullimage_w;
    int gotonethisline=0;
    int maxx=0;
    int minx=0;
    for (int x=0;x<w;x++) {
      if (*(ptr++) >> 24) {
        gotonethisline=1;
        maxx = x;
        if (!minx) minx = x;
      }
    }
    if (gotonethisline) {
      r->left = MIN<int>(minx, r->left);
      r->right = MAX<int>(maxx, r->right);
      r->top = MIN<int>(y, r->top);
      r->bottom = MAX<int>(y, r->bottom);
    }
  }
  if (r->right == -1) {
    r->left = r->top = r->right = r->bottom = 0;
  }
}

// This tests if any pixel's rgb values are higher than the alpha value, which should not be possible
// if the alpha bytes have been premultiplied and if no bliting function breach consistancy. 
// The function returns the number of error, or 0 if consistancy is good.
// Note: this makes sense only for premultiplied bitmaps (and returns 0 no matter what if NO_PREMUL is defined)
int Bitmap::checkAlphaConsistancy() {
#ifdef NO_PREMUL
  return 0;
#else
  ARGB32 *ptr = getBits();
  int w = fullimage_w;
  int h = fullimage_h;
  int errcount = 0;
  for (int y=0;y<h;y++) {
    for (int x=0;x<w;x++) {
      ARGB32 byte = *ptr++;
      int a = (byte & 0xFF000000)>>24;
      int r = (byte & 0xFF0000)>>16;
      int g = (byte & 0xFF00)>>8;
      int b = (byte & 0xFF);
      if (r > a || g > a || b > a) errcount++;
    }
  }
  if (errcount > 0) {
    DebugString("%d alpha errors detected in bitmap\n", errcount);
  }
  return errcount;
#endif
}

// this does not actually do bgra->argb, rather it does abgr->argb
void Bitmap::bgra2argb() {

  COLORREF *b = bits;
  int y = subimage_h;
  if (y == -1) y = fullimage_h;
  int w = subimage_w;
  if (w == -1) w = fullimage_w;
  int next = fullimage_w-w;
  while (y--) {
    int x = w;
    while (x--) {
      *b = BGRATOARGB(*b);
      b++;
    }
    b += next;
  }
}

void Bitmap::blitRotated(CanvasBase *canvas, float x, float y, int w, int h, float angle_in_radians, float scale/* =-1 */, int alpha/* = 255 */, Blender *blender/* = NULL */, int subsample/* = 1 */, float centerxoffsetsrc/* = 0 */, float centeryoffsetsrc/* = 0*/, float centerxoffsetdst/* = 0 */, float centeryoffsetdst/* = 0*/, int wrapx/* =0 */, int wrapy/* =0 */) {
#ifdef PERSISTENT_STRETCH_TEMP_CANVAS
  static THREADSTORAGE BltCanvas *blit2_tmpcanvas;
#endif
  int srcw = getWidth();
  int srch = getHeight();
  
  int _dx;
  int _dy;

  int dstw, dsth, p;

  BltCanvas *tmpcanvas = NULL;
  ARGB32 *destbitsstart = (ARGB32*)canvas->getBits();
  if (destbitsstart != NULL && (int)x == x && (int)y == y) {
    canvas->getDim(&dstw, &dsth, &p);
    _dx = (int)x;
    _dy = (int)y;
  } else {
    #ifdef PERSISTENT_STRETCH_TEMP_CANVAS
    tmpcanvas = Canvas::makeTempCanvas(w, h, &blit2_tmpcanvas);
    #else
    tmpcanvas = Canvas::makeTempCanvas(w, h);
    #endif

    tmpcanvas->getDim(&dstw, &dsth, &p);
    destbitsstart = (ARGB32*)tmpcanvas->getBits();
    _dx = 0;
    _dy = 0;
  }

  int duCol,dvCol,duRow,dvRow;
  int cx,cy;

  float s = 1/scale;
  duCol = fastfrnd(cos(angle_in_radians) * s * 65536);
  dvCol = fastfrnd(sin(angle_in_radians) * s * 65536);

  duRow = -dvCol;
  dvRow = duCol;

  int startingU = (int)((((-cos(angle_in_radians) * ((srcw-1)/2.0f+centerxoffsetdst)) - (-sin(angle_in_radians) * ((srch-1)/2.0f+centeryoffsetdst))) * s + ((srcw-1)/2.0f) + centerxoffsetsrc) * 65536);
  int startingV = (int)((((-sin(angle_in_radians) * ((srcw-1)/2.0f+centerxoffsetdst)) + (-cos(angle_in_radians) * ((srch-1)/2.0f+centeryoffsetdst))) * s + ((srch-1)/2.0f) + centeryoffsetsrc) * 65536);

  int rowU=startingU;
  int rowV=startingV;

  int mw = MIN(dstw-(int)x, w-(int)x);
  int mh = MIN(dsth-(int)y, h-(int)y);
  /*if (!wrapx && !wrapy) {
    mw = MIN(dstw, srcw);
    mh = MIN(dsth, srch);
  } else {
    mw = dstw;
    mh = dsth;
  }*/

  int noalpha = (tmpcanvas != NULL) || alpha == -1;

  ARGB32 *srcbits = getBits();

  ARGB32 *p1,*p2,*p3,*p4;
  
  #define MMOD(x,y) (x>0?(x%y):(y-(ABS(x)%y))%y);
  if (subsample) {
    for (cy=0;cy<mh;cy++) {
      int u=rowU;
      int v=rowV;
      int dy = cy+_dy;
      if (dy < 0 || dy >= dsth) {
        rowU += duRow;
        rowV += dvRow;
        continue;
      }
      int dx = _dx;
      ARGB32 *destbits = destbitsstart+(dy*dstw)+dx;
      for(cx=0;cx<mw;cx++,dx++) {
        if (dx >= 0 && dx < dstw) {
          int sx = u>>16;
          int sy = v>>16;
          if (wrapx) sx = MMOD(sx,srcw);
          if (wrapy) sy = MMOD(sy,srch);
          if (sx >= -1 && sx < srcw && sy >= -1 && sy < srch) {
            // blit subpixel at p1=sx,sy / p2=sx+1,sy / p3=sx,sy+1 / p4=sx+1,sy+1
            p1 = srcbits+sx+sy*srcw;
            p2 = p1+1;
            p3 = p1+srcw;
            p4 = p3+1;
            if (noalpha) {
              if (sx >= 0 && sx < srcw-1 && sy >= 0 && sy < srch-1) {
                // most pixels, extras pixel on the left and on the botton are valid
                *destbits = Blenders::BLEND4(*p1, *p2, *p3, *p4, u, v);
              } else if (sx == -1) {
                if (sy >= 0 && sy < srch-1) {
                  // most pixels of first column, p1/p3 invalid, p2/p4 valid
                  *destbits = Blenders::BLEND4(0, *p2, 0, *p4, u, v);
                } else if (sy == -1) {
                  // top left pixel, only p4 is valid
                  *destbits = Blenders::BLEND4(0, 0, 0, *p4, u, v);
                } else {
                  // bottom left pixel only p2 is valid
                  *destbits = Blenders::BLEND4(0, *p2, 0, 0, u, v);
                }
              } else if (sy == -1) {
                if (sx < srcw-1) {
                  // most pixels of first line, p1/p2 invalid, p3/p4 valid
                  *destbits = Blenders::BLEND4(0, 0, *p3, *p4, u, v);
                } else {
                  // top right pixel, only p2 is valid
                  *destbits = Blenders::BLEND4(0, *p2, 0, 0, u, v);
                }
              } else if (sx == srcw-1) {
                if (sy < srch-1) {
                  // most pixels of last column, p1/p3 valid, p2/p4 invalid
                  if (wrapx) {
                    ARGB32 *wp2 = p1-(srcw-1);
                    *destbits = Blenders::BLEND4(*p1, *wp2, *p3, *(wp2+srcw), u, v);
                  } else
                    *destbits = Blenders::BLEND4(*p1, 0, *p3, 0, u, v);
                } else {
                  // bottom right pixel, only p1 is valid. 
                  if (wrapx && wrapy) {
                    ARGB32 *wp2 = p1-(srcw-1);
                    ARGB32 *wp3 = p1-(srch-1)*srcw;
                    ARGB32 *wp4 = wp3-(srcw-1);
                    *destbits = Blenders::BLEND4(*p1, *wp2, *wp3, *wp4, u, v);
                  } else if (wrapx) {
                    *destbits = Blenders::BLEND4(*p1, *(p1-(srcw-1)), 0, 0, u, v);
                  } else if (wrapy) {
                    *destbits = Blenders::BLEND4(*p1, 0, *(p1-(srch-1)*srcw), 0, u, v);
                  } else
                    *destbits = Blenders::BLEND4(*p1, 0, 0, 0, u, v);
                }
              } else {
                // most pixels of last line, p1/p2 valid, p3/p4 invalid 
                if (wrapy)
                  *destbits = Blenders::BLEND4(*p1, *p2, *(p1-(srch-1)*srcw), *(p1-(srch-1)*srcw+1), u, v);
                else
                  *destbits = Blenders::BLEND4(*p1, *p2, 0, 0, u, v);
              }
            } else {
              if (sx >= 0 && sx < srcw-1 && sy >= 0 && sy < srch-1) {
                // most pixels, extras pixel on the left and on the botton are valid
                *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *p2, *p3, *p4, u, v), alpha);
              } else if (sx == -1) {
                if (sy >= 0 && sy < srch-1) {
                  // most pixels of first column, p1/p3 invalid, p2/p4 valid
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(0, *p2, 0, *p4, u, v), alpha);
                } else if (sy == -1) {
                  // top left pixel, only p4 is valid
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(0, 0, 0, *p4, u, v), alpha);
                } else {
                  // bottom left pixel only p2 is valid
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(0, *p2, 0, 0, u, v), alpha);
                }
              } else if (sy == -1) {
                if (sx < srcw-1) {
                  // most pixels of first line, p1/p2 invalid, p3/p4 valid
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(0, 0, *p3, *p4, u, v), alpha);
                } else {
                  // top right pixel, only p2 is valid
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(0, *p2, 0, 0, u, v), alpha);
                }
              } else if (sx == srcw-1) {
                if (sy < srch-1) {
                  // most pixels of last column, p1/p3 valid, p2/p4 invalid
                  if (wrapx) {
                    ARGB32 *wp2 = p1-(srcw-1);
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *wp2, *p3, *(wp2+srcw), u, v), alpha);
                  } else
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, 0, *p3, 0, u, v), alpha);
                } else {
                  // bottom right pixel, only p1 is valid. 
                  if (wrapx && wrapy) {
                    ARGB32 *wp2 = p1-(srcw-1);
                    ARGB32 *wp3 = p1-(srch-1)*srcw;
                    ARGB32 *wp4 = wp3-(srcw-1);
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *wp2, *wp3, *wp4, u, v), alpha);
                  } else if (wrapx) {
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *(p1-(srcw-1)), 0, 0, u, v), alpha);
                  } else if (wrapy) {
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, 0, *(p1-(srch-1)*srcw), 0, u, v), alpha);
                  } else
                    *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, 0, 0, 0, u, v), alpha);
                }
              } else {
                // most pixels of last line, p1/p2 valid, p3/p4 invalid 
                if (wrapy)
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *p2, *(p1-(srch-1)*srcw), *(p1-(srch-1)*srcw+1), u, v), alpha);
                else
                  *destbits = Blenders::BLEND_ADJ3(*destbits, Blenders::BLEND4(*p1, *p2, 0, 0, u, v), alpha);
              }
            }
          }
        }
        destbits++;
        u += duCol;
        v += dvCol;
      }
      rowU += duRow;
      rowV += dvRow;
    }
    if (tmpcanvas) {
      if ((int)x == x && (int)y == y)
        tmpcanvas->blit(0, 0, canvas, (int)x, (int)y, w, h, alpha, blender);
      else
        tmpcanvas->stretchblit_hires(0, 0, (float)w, (float)h, canvas, x, y, (float)w, (float)h, alpha, blender);
    }
  } else {
    for (cy=0;cy<mh;cy++) {
      int u=rowU;
      int v=rowV;

      int dy = cy+_dy;
      if (dy < 0 || dy >= dsth) {
        rowU += duRow;
        rowV += dvRow;
        continue;
      }
      int dx = _dx;
      ARGB32 *destbits = destbitsstart+(dy*dstw)+dx;
      for(cx=0;cx<mw;cx++,dx++) {
        if (dx >= 0 && dx < dstw) {
          int sx = u>>16;
          int sy = v>>16;
          if (wrapx) sx = MMOD(sx,srcw);
          if (wrapy) sy = MMOD(sy,srch);
          if (sx >= 0 && sx < srcw && sy >= 0 && sy < srch) {
            if (noalpha) {
              *destbits = *((unsigned int *)srcbits+sx+sy*srcw);
            } else {
              *destbits = Blenders::BLEND_ADJ3(*destbits, *((unsigned int *)srcbits+sx+sy*srcw), alpha);
            }
          } 
        } 
        destbits++;
        u += duCol;
        v += dvCol;
      }
      rowU += duRow;
      rowV += dvRow;
    }
    if (tmpcanvas) tmpcanvas->blit(0, 0, canvas, (int)x, (int)y, w, h, alpha, blender);
  }
  if (tmpcanvas) Canvas::releaseTempCanvas(tmpcanvas);
}


void Bitmap::blitRotatedNonCroppedCentered(CanvasBase *canvas, float x, float y, float angle_in_radians, float scale/* =-1 */, RECT *blitrect/* =NULL */, int alpha/* =255 */, Blender *blender/* =NULL */, int subsample/* =1 */) {
  #define SIN45 0.7071068f
  int m = MAX(getWidth(), getHeight());
  int newm = (int) ceil(m + (1.0f-SIN45)*2.0f*m);

  float nx = x-newm/2.0f;
  float ny = y-newm/2.0f;
  float offsx = (newm-getWidth())/2.0f;
  float offsy = (newm-getHeight())/2.0f;

  blitRotated(canvas, nx, ny, newm, newm, angle_in_radians, scale, alpha, blender, subsample, 0, 0, offsx, offsy, 0, 0); 

  if (blitrect) {
    blitrect->left = (LONG)nx;
    blitrect->top = (LONG)ny;
    blitrect->right = (LONG)ceil(nx+newm);
    blitrect->bottom = (LONG)ceil(ny+newm);
  }
}

// -----------------------------------------------------------------------

WrapperBitmap::WrapperBitmap(ARGB32 *_bits, int bits_w, int bits_h, int subimage_offset_x, int subimage_offset_y, int _subimage_w, int _subimage_h) {
  bits = _bits;
  invalid = FALSE;
  allocmethod = ALLOC_NONE;

  if (bits_w < 0) bits_w = 0;
  if (bits_h < 0) bits_h = 0;

  subimage_w=_subimage_w;
  subimage_h=_subimage_h;
  x_offset=subimage_offset_x;
  y_offset=subimage_offset_y;
  fullimage_w=bits_w;
  fullimage_h=bits_h;
  bitmapname = "";
  last_failed = 0;

  int memsize = bits_w*bits_h*sizeof(ARGB32);
  if (memsize == 0) memsize++; // +1 so no failure when 0x0

  has_alpha = TRUE;	// who knows, so play it safe
}

WrapperBitmap::WrapperBitmap(BltCanvas *canvas) {
  bits = (ARGB32 *)canvas->getBits();;
  invalid = FALSE;
  allocmethod = ALLOC_NONE;

  int bits_w, bits_h, bits_p;
  canvas->getDim(&bits_w, &bits_h, &bits_p);

  if (bits_w < 0) bits_w = 0;
  if (bits_h < 0) bits_h = 0;

  subimage_w=-1;
  subimage_h=-1;
  x_offset=-1;
  y_offset=-1;
  fullimage_w=bits_w;
  fullimage_h=bits_h;
  bitmapname = "";
  last_failed = 0;

  int memsize = bits_w*bits_h*sizeof(ARGB32);
  if (memsize == 0) memsize++; // +1 so no failure when 0x0

  has_alpha = TRUE;	// who knows, so play it safe
}
