#ifndef _WASABI_BLENDING_H_
#define _WASABI_BLENDING_H_

#include <bfc/std_math.h>

#ifdef _MSC_VER
#pragma warning( push, 1 )
// Disable the "conversion from 'int' to 'float', possible loss of data" warning.
#pragma warning(disable: 4244)
#pragma warning( push, 1 )
#pragma warning(disable: 4731)
#endif

// A note about premultiply vs non-premultiplied bitmaps.
// 
// You may notice that most of the non-premultiplied blend functions first perform a multiplication 
// of the rgb components (via afactor and bfactor), apply the blend function, and then unmultiply them 
// (via tfactor which is 1/resulting_alpha). The premultiplied version of the same blend modes will not
// need to do these steps. This could make it seem like there is no point in using non-premultiplied 
// pixels and that it only makes the blend functions more complicated, but there actually is a point: 
// there is no loss of precision then, since the factors are floating point values. When you keep pixels 
// premultiplied, these factors are essentially embedded into each rgb component, which is a byte. 
//
// Here's an example: if alpha is 15, and a component was originally 15, the premultiplied component value 
// is 15*15/255, which truncates to 0. Obviously unpremultiplying that value by the alpha again will give
// you 0 (255/15*0), instead of 15, you have lost 15 shades. You could try rounding instead of truncating: 
// 15*15/255 rounds to 1, 255/15*1 then rounds to 17, you have gain two shades. There is no such deviation 
// when you keep color component and alpha value separated until you actually need to do the multiplication, 
// and then do it using floating point: 15.0f*15.0f/255.0f = 0.88235...f, and 255.0f/15.0f*0.88235...f = 15.
//
// Of course, the loss/gain is generally pretty small, and most applications do not care, it is only important 
// to use non-premultiplied pixels if you are repeatedly blending the results of previous blends, in that case, 
// bit loss/gain will accumulate and eventually, your bitmaps will turn white or black. It's up to you to 
// consider wether you need the extra precision, at the expense of blend speed.
//
// To use non-premultiplied pixels, define NO_PREMUL. 
//
// Oh, and yeah, theres a bunch of non-premultiplied blenders that haven't been converted to their premultiplied
// versions. They will come eventually, but if you are in a hurry, feel free to do it :P

#define FP_TYPE signed int

class Blenders {
public:
  static void init();
  static unsigned int inline DEMULTIPLY(unsigned int a);
  static unsigned int inline PREMULTIPLY(unsigned int a);
  static unsigned int inline BLEND_MUL_SCALAR(unsigned int a, int v);

  // these functions perform tests and call the appropriate blend version for the architecture and blitmode,
  // so you should not call them repeatedly, instead make the appropriate tests/ifdefs and call the corresponding
  // functions directly
  static unsigned int inline BLEND_ADJ1(unsigned int a, unsigned int b, int alpha); 
  static unsigned int inline BLEND_ADJ2(unsigned int a, unsigned int b); 
  static unsigned int inline BLEND_ADJ3(unsigned int a, unsigned int b, int alpha); 
  static unsigned int inline BLEND_ADD(unsigned int a, unsigned int b, int alpha);
  static unsigned int inline BLEND_SUB(unsigned int a, unsigned int b, int alpha);
  static unsigned int inline BLEND_AVG(unsigned int a, unsigned int b);

  static unsigned int inline BLEND4(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp);
  static unsigned int inline BLEND4_LASTCOL(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTLINE(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTPIX(unsigned int *p1, unsigned int w, int xp, int yp);

  static unsigned int inline BLEND_PREMULTIPLIED_ADD(unsigned int a, unsigned int b, int alpha); // premultiplied add, if you are using premultiplied pixel, this is the same as BLEND_ADD, otherwise use DEMULTIPLY to convert your results to non-premul

  // these functions use their respective architecture directly, use them in blend loops
  static unsigned int inline BLEND_MUL_SCALAR_FAST_C(unsigned int a, int v);
  static unsigned int inline BLEND_ADJ1_FAST_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADJ2_FAST_C(unsigned int a, unsigned int b);
  static unsigned int inline BLEND_ADJ3_FAST_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADD_FAST_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_SUB_FAST_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_AVG_FAST_C(unsigned int a, unsigned int b);
  static unsigned int inline BLEND4_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_FAST_C(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp);
  static unsigned int inline BLEND4_LASTCOL_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTLINE_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTPIX_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND_PREMULTIPLIED_ADD_FAST_C(unsigned int a, unsigned int b, int alpha); // premultiplied add, if you are using premultiplied pixel, this is the same as BLEND_ADD, otherwise use DEMULTIPLY to convert your results to non-premul
#ifndef NO_MMX
  static unsigned int inline BLEND_MUL_SCALAR_FAST_MMX(unsigned int a, int v);
  static unsigned int inline BLEND_ADJ1_FAST_MMX(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADJ2_FAST_MMX(unsigned int a, unsigned int b);
  static unsigned int inline BLEND_ADJ3_FAST_MMX(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADD_FAST_MMX(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_SUB_FAST_MMX(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_AVG_FAST_MMX(unsigned int a, unsigned int b);
  static unsigned int inline BLEND4_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_FAST_MMX(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp);
  static unsigned int inline BLEND4_LASTCOL_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTLINE_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTPIX_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND_PREMULTIPLIED_ADD_FAST_MMX(unsigned int a, unsigned int b, int alpha); // premultiplied add, if you are using premultiplied pixel, this is the same as BLEND_ADD, otherwise use DEMULTIPLY to convert your results to non-premul
#endif
#ifdef NO_PREMUL
  static unsigned int inline BLEND_MUL_SCALAR_PRECISE_C(unsigned int a, int v);
  static unsigned int inline BLEND_ADJ1_PRECISE_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADJ2_PRECISE_C(unsigned int a, unsigned int b);
  static unsigned int inline BLEND_ADJ3_PRECISE_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADD_PRECISE_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_SUB_PRECISE_C(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_AVG_PRECISE_C(unsigned int a, unsigned int b);
  static unsigned int inline BLEND4_PRECISE_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_PRECISE_C(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp);
  static unsigned int inline BLEND4_LASTCOL_PRECISE_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTLINE_PRECISE_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTPIX_PRECISE_C(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND_PREMULTIPLIED_ADD_PRECISE_C(unsigned int a, unsigned int b, int alpha); // premultiplied add, if you are using premultiplied pixel, this is the same as BLEND_ADD, otherwise use DEMULTIPLY to convert your results to non-premul
#ifndef NO_SSE
  static unsigned int inline BLEND_MUL_SCALAR_PRECISE_SSE(unsigned int a, int v);
  static unsigned int inline BLEND_ADJ1_PRECISE_SSE(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADJ2_PRECISE_SSE(unsigned int a, unsigned int b);
  static unsigned int inline BLEND_ADJ3_PRECISE_SSE(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_ADD_PRECISE_SSE(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_SUB_PRECISE_SSE(unsigned int a, unsigned int b, int w);
  static unsigned int inline BLEND_AVG_PRECISE_SSE(unsigned int a, unsigned int b);
  static unsigned int inline BLEND4_PRECISE_SSE(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_PRECISE_SSE(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp);
  static unsigned int inline BLEND4_LASTCOL_PRECISE_SSE(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTLINE_PRECISE_SSE(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND4_LASTPIX_PRECISE_SSE(unsigned int *p1, unsigned int w, int xp, int yp);
  static unsigned int inline BLEND_PREMULTIPLIED_ADD_PRECISE_SSE(unsigned int a, unsigned int b, int alpha); // premultiplied add, if you are using premultiplied pixel, this is the same as BLEND_ADD, otherwise use DEMULTIPLY to convert your results to non-premul
#endif
#endif

  static int inline MMX_AVAILABLE() { return mmx_available; }
  static int inline SSE_AVAILABLE() { return sse_available; }

  // Blit modes are only meaningful in non premul mode, since premul can't do precise blit
  // FAST_BLIT 
  // PRECISE_BLIT 

  enum {
    FAST_BLIT,    // faster blit, but with some bit loss (ADJ3(0x80FFFFFF, 0x80FFFFFF, 128) = A0FEFEFE)
    PRECISE_BLIT, // slower blit, but no bit loss (ADJ3(0x80FFFFFF, 0x80FFFFFF, 128) = A0FFFFFF)
  };

  static void pushBlitMode(int fastorprecise);
  static void popBlitMode();
  static int getBlitMode() { 
#ifndef NO_PREMUL
    return FAST_BLIT;
#else
    return blitmode; 
#endif
  }

#ifndef NO_MMX
  static void inline BLEND_MMX_END() {
    if (getBlitMode() == Blenders::PRECISE_BLIT || !MMX_AVAILABLE()) return;
#ifdef WIN32
    if (mmx_available)  __asm emms; 
#elif defined(LINUX)
    if (mmx_available) __asm__ volatile ( "emms" : :);
#else
#error portme
#endif
  }
#endif

  static unsigned char alphatable[256][256];
  static unsigned char demultable[256][256];
  static FP_TYPE fp_alphatable[256][256];
  static FP_TYPE fp_divtable[256][256];
  static FP_TYPE fp15_divtable[256][256];
  static unsigned char addtable[256][256];
  static unsigned char subtable[256][256];
  static int inited;

  static THREADSTORAGE Stack<int> *blitmodestack; // pointer here, because THREADSTORAGE can't do constructors
  static THREADSTORAGE int blitmode;

private:
  static int mmx_available;
  static int sse_available;
};

const float _ONEOVER255 = 1/255.0f;
const float _ONEOVER65025 = 1/65025.0f;

#ifndef NO_MMX
#ifdef WIN32
#define MMX_CONST const
#else
#define MMX_CONST
#endif

extern unsigned int MMX_CONST Blenders__mmx_revn2[2];
extern unsigned int MMX_CONST Blenders__mmx_zero[2];
extern unsigned int MMX_CONST Blenders__mmx_one[2];
extern unsigned int MMX_CONST Blenders__mmx_255[2];
extern unsigned int MMX_CONST Blenders__mmx_lw255[2];

#undef MMX_CONST

#endif

#ifndef NO_SSE

#include <bfc/sse_intrinsics.h>

#endif

#define FP_TYPE signed int
#define _FP_255 0xFF0000
#define _FP_0 0

#define MAKEARGB(a, r, g, b) (((a)<<24)|((r)<<16)|((g)<<8)|(b))

// -----------------------------------------------------------------------
// Blend path selection macro
// -----------------------------------------------------------------------

// Use MAKE_BLEND_JOB to create your blend loops. ie:
//
// #undef BLEND_JOB
// #define BLEND_JOB(version) { while (n--) *dest++ Blenders::BLEND_ADJ3##version(*src1++, *src2++, alpha); }
// MAKE_BLEND_JOB

#ifndef NO_PREMUL
#ifndef NO_MMX
#define MAKE_BLEND_JOB\
    if (Blenders::MMX_AVAILABLE()) { \
      BLEND_JOB(_FAST_MMX); \
    } else { \
      BLEND_JOB(_FAST_C); \
    }
#else 
#define MAKE_BLEND_JOB \
    BLEND_JOB(_FAST_C);
#endif // NO_MMX
#else // NO_PREMUL
#if !defined(NO_MMX) && !defined(NO_SSE)
#define MAKE_BLEND_JOB \
  if (Blenders::getBlitMode() == Blenders::FAST_BLIT) { \
    if (Blenders::MMX_AVAILABLE()) { \
      BLEND_JOB(_FAST_MMX); \
    } else { \
      BLEND_JOB(_FAST_C); \
    } \
  } else { \
    if (Blenders::SSE_AVAILABLE()) { \
      BLEND_JOB(_PRECISE_SSE); \
    } else { \
      BLEND_JOB(_PRECISE_C); \
    } \
  }
#elif defined(NO_MMX) && !defined(NO_SSE)
#define MAKE_BLEND_JOB \
  if (Blenders::getBlitMode() == Blenders::FAST_BLIT) { \
    BLEND_JOB(_FAST_C); \
  } else { \
    if (Blenders::SSE_AVAILABLE()) { \
      BLEND_JOB(_PRECISE_SSE); \
    } else { \
      BLEND_JOB(_PRECISE_C); \
    } \
  }
#elif !defined(NO_MMX) && defined(NO_SSE)
#define MAKE_BLEND_JOB \
  if (Blenders::getBlitMode() == Blenders::FAST_BLIT) { \
    if (Blenders::MMX_AVAILABLE()) { \
      BLEND_JOB(_FAST_MMX); \
    } else { \
      BLEND_JOB(_FAST_C); \
    } \
  } else { \
    BLEND_JOB(_PRECISE_C); \
  }
#else
#define MAKE_BLEND_JOB \
  if (Blenders::getBlitMode() == Blenders::FAST_BLIT) { \
    BLEND_JOB(_FAST_C); \
  } else { \
    BLEND_JOB(_PRECISE_C); \
  }
#endif
#endif // NO_PREMUL

// -----------------------------------------------------------------------
// Blender base class
// -----------------------------------------------------------------------

class Blender {
public:
  // Blend description
  virtual const char *getName()=0;
  // Blend two pixels, it is not recommended to call this function unless you are blending a single color,
  // if you blend pixels repeatedly, use blendLine or the MAKE_BLEND_JOB macro
  virtual ARGB32 __fastcall blend(ARGB32 a, ARGB32 b, signed int alpha)=0;
  // Blend a consecutive number of pixels, (xdst,ydst) and (xsrc,ysrc) are informative, most blends do not need them
  virtual void __fastcall blendLine(ARGB32 *dest, ARGB32 *src, signed int alpha, int n, int xdst, int ydst, int xsrc, int ysrc)=0;
  // Stretches a consecutive number of pixels
  virtual void __fastcall stretchLine(int xp, int *psrc, int *dest, int xv, int dxv, int alpha)=0;
  // Wether (a blend b blend c) == (a blend (b blend c))
  virtual int isAssociative()=0;
  // optimized blend paths
  virtual ARGB32 __fastcall blend_FAST_C(ARGB32 a, ARGB32 b, int alpha)=0;
#ifndef NO_MMX
  virtual ARGB32 __fastcall blend_FAST_MMX(ARGB32 a, ARGB32 b, int alpha)=0;
#endif
#ifdef NO_PREMUL
  virtual ARGB32 __fastcall blend_PRECISE_C(ARGB32 a, ARGB32 b, int alpha)=0;
#ifndef NO_SSE
  virtual ARGB32 __fastcall blend_PRECISE_SSE(ARGB32 a, ARGB32 b, int alpha)=0;
#endif
#endif
};

// -----------------------------------------------------------------------
// Blender implementation template, inherit this to make your own blender,
// give it as a parameter a class with the following static functions :
//
//  static  __inline ARGB32 blend_FAST_C(ARGB32 a, ARGB32 b, int alpha);      // fast, both premul and non premul
//  static  __inline ARGB32 blend_FAST_MMX(ARGB32 a, ARGB32 b, int alpha);    // fast, both premul and non premul, mmx only
//  static  __inline ARGB32 blend_PRECISE_C(ARGB32 a, ARGB32 b, int alpha);   // precise, non premul only
//  static  __inline ARGB32 blend_PRECISE_SSE(ARGB32 a, ARGB32 b, int alpha); // precise, non premul only, sse only
//  static __inline const char *getName();
//  static __inline int isAssociative();
//
//  note: there is no FAST_SSE, since if SSE is available, MMX is too, and 
//  since SSE is slower at fast blit than MMX is. there is also no PRECISE_MMX
//  since MMX is limited to integer arithmetic
//
// You could also wrap another blend and modify the blending parameters
// on the fly (ie, according to a map), or chain several blend modes
// together. You get the idea
// -----------------------------------------------------------------------
template <class T>
class BlenderImpl : public Blender {
public:
  virtual const char *getName() { return T::getName(); }
  virtual ARGB32 __fastcall blend(ARGB32 a, ARGB32 b, signed int alpha) { 
    #undef BLEND_JOB
    #define BLEND_JOB(version) return T::blend##version(a, b, alpha);
    MAKE_BLEND_JOB;
  }
  virtual void __fastcall blendLine(ARGB32 *dest, ARGB32 *src, int alpha, int n, int xdst, int ydst, int xsrc, int ysrc) { 
    #undef BLEND_JOB
    #define BLEND_JOB(version) \
      while (n--) { \
        *dest = T::blend##version(*dest, *src, alpha); \
        dest++; src++; \
      }
    MAKE_BLEND_JOB;
  }
  virtual void __fastcall stretchLine(int xp, int *psrc, int *dest, int xv, int dxv, int alpha) {
    #undef BLEND_JOB
    #define BLEND_JOB(version) \
    while (xp--) { \
      *dest++ = T::blend##version(*dest, psrc[xv>>16], alpha); \
      xv+=dxv; \
    }
    MAKE_BLEND_JOB;
  }

  virtual int isAssociative() { return T::isAssociative(); }

  // optimized blend paths
  virtual ARGB32 __fastcall blend_FAST_C(ARGB32 a, ARGB32 b, int alpha) {
    return T::blend_FAST_C(a, b, alpha);
  }
#ifndef NO_MMX
  virtual ARGB32 __fastcall blend_FAST_MMX(ARGB32 a, ARGB32 b, int alpha) {
    return T::blend_FAST_MMX(a, b, alpha);
  }
#endif
#ifdef NO_PREMUL
  virtual ARGB32 __fastcall blend_PRECISE_C(ARGB32 a, ARGB32 b, int alpha) {
    return T::blend_PRECISE_C(a, b, alpha);
  }
#ifndef NO_SSE
  virtual ARGB32 __fastcall blend_PRECISE_SSE(ARGB32 a, ARGB32 b, int alpha) {
    return T::blend_PRECISE_SSE(a, b, alpha);
  }
#endif
#endif
};

// -----------------------------------------------------------------------
// Normal blend, a*(1-AlphaOf(b)*alpha) + b*alpha, this is the default 
// blend when NULL is passed to the blit functions
// -----------------------------------------------------------------------

class BlendNormal {
public:
  static __inline const char *getName() { return "Normal"; }
  static __inline unsigned int blend(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ3(a, b, alpha); }
  static __inline int isAssociative() { return 1; }

  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ3_FAST_C(a, b, alpha); }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ3_FAST_MMX(a, b, alpha); }
#endif
#ifdef NO_PREMUL
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ3_PRECISE_C(a, b, alpha); }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ3_PRECISE_SSE(a, b, alpha); }
#endif
#endif
};

extern BlenderImpl<BlendNormal> g_blend_normal;

// -----------------------------------------------------------------------
// Additive blend, pixels alpha values are blended too
// -----------------------------------------------------------------------

class BlendAdd {
public:
  static __inline const char *getName() { return "Add"; }
  static __inline unsigned int blend(ARGB32 a, ARGB32 b, signed int w) { return Blenders::BLEND_ADD(a, b, w); }
  static __inline int isAssociative() { return 1; }

  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADD_FAST_C(a, b, alpha); }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADD_FAST_MMX(a, b, alpha); }
#endif
#ifdef NO_PREMUL
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADD_PRECISE_C(a, b, alpha); }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADD_PRECISE_SSE(a, b, alpha); }
#endif
#endif
};

extern BlenderImpl<BlendAdd> g_blend_add;

// -----------------------------------------------------------------------
// Subtractive blend, pixels alpha values are blended too
// -----------------------------------------------------------------------

class BlendSubtract {
public:
  static __inline const char *getName() { return "Subtract"; }
  static __inline unsigned int blend(ARGB32 a, ARGB32 b, signed int w) {
    return Blenders::BLEND_SUB(a, b, w);
  }
  static __inline int isAssociative() { return 1; }

  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_SUB_FAST_C(a, b, alpha); }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_SUB_FAST_MMX(a, b, alpha); }
#endif
#ifdef NO_PREMUL
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_SUB_PRECISE_C(a, b, alpha); }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_SUB_PRECISE_SSE(a, b, alpha); }
#endif
#endif
};

extern BlenderImpl<BlendSubtract> g_blend_subtract;

// -----------------------------------------------------------------------
// Median blend, (1.0-alpha)*a + alpha*b
// -----------------------------------------------------------------------

class BlendMedian {
public:
  static __inline const char *getName() { return "Median"; }
  static __inline unsigned int blend(ARGB32 a, ARGB32 b, signed int alpha) { return Blenders::BLEND_ADJ1(a, b, alpha); }
  static __inline int isAssociative() { return 1; }

  // optimized blend paths
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ1_FAST_C(a, b, alpha); }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ1_FAST_MMX(a, b, alpha); }
#endif
#ifdef NO_PREMUL
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ1_PRECISE_C(a, b, alpha); }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int alpha) {  return Blenders::BLEND_ADJ1_PRECISE_SSE(a, b, alpha); }
#endif
#endif
};

extern BlenderImpl<BlendMedian> g_blend_median;

// -----------------------------------------------------------------------

unsigned int __inline Blenders::PREMULTIPLY(unsigned int a) {
  register int v = a&0xFF000000;
  register int alpha = a>>24;
  v |= Blenders::alphatable[alpha][(a)&0xFF];
  v |= Blenders::alphatable[alpha][(a>>8)&0xFF]<<8;
  v |= Blenders::alphatable[alpha][(a>>16)&0xFF]<<16;
  return v;
}

unsigned int __inline Blenders::DEMULTIPLY(unsigned int a) {
  register int v = a&0xFF000000;
  register int alpha = a>>24;
  v |= demultable[alpha][(a)&0xFF];
  v |= demultable[alpha][(a>>8)&0xFF]<<8;
  v |= demultable[alpha][(a>>16)&0xFF]<<16;
  return v;
}

// -----------------------------------------------------------------------
// Non-optimized blend path functions, call them punctually. If you are
// going to call them repeatedly, you should instead make specific blend
// loops in your code for each architecture and blitmode (MMX, SSE, fast/precise)
// and call the appropriate functions directly (ie, do this via MAKE_BLEND_JOB)
// -----------------------------------------------------------------------

// V is scalar (0-255), (1.0-V)*b + V*a
unsigned int inline Blenders::BLEND_ADJ1(unsigned int a, unsigned int b, int w){
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_ADJ1##version(a, b, w); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// returns a*(1.0-Alpha(b)) + b
unsigned int inline Blenders::BLEND_ADJ2(unsigned int a, unsigned int b) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_ADJ2##version(a, b); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// returns a*(1-Alpha(b)*W) + b*W, clamped (W is scalar 0-0xff). 
unsigned int inline Blenders::BLEND_ADJ3(unsigned int a, unsigned int b, int w){
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_ADJ3##version(a, b, w); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// retuns min(255, a+b)
unsigned int __inline Blenders::BLEND_ADD(unsigned int a, unsigned int b, int w) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_ADD##version(a, b, w); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// returns max(0, a-b)
unsigned int __inline Blenders::BLEND_SUB(unsigned int a, unsigned int b, int w) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_SUB##version(a, b, w); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// returns (a+b)/2
unsigned int __inline Blenders::BLEND_AVG(unsigned int a, unsigned int b) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_AVG##version(a, b); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// quad blends
unsigned int __inline Blenders::BLEND4(unsigned int *p1, unsigned int w, int xp, int yp) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND4##version(p1, w, xp, yp); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

unsigned int __inline Blenders::BLEND4(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND4##version(p1, p2, p3, p4, xp, yp); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

unsigned int __inline Blenders::BLEND4_LASTCOL(unsigned int *p1, unsigned int w, int xp, int yp) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND4_LASTCOL##version(p1, w, xp, yp); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

unsigned int __inline Blenders::BLEND4_LASTLINE(unsigned int *p1, unsigned int w, int xp, int yp) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND4_LASTLINE##version(p1, w, xp, yp); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

unsigned int __inline Blenders::BLEND4_LASTPIX(unsigned int *p1, unsigned int w, int xp, int yp) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND4_LASTPIX##version(p1, w, xp, yp); }
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

unsigned int __inline Blenders::BLEND_MUL_SCALAR(unsigned int a, int w) {
  ARGB32 c;
  #undef BLEND_JOB
  #define BLEND_JOB(version) { c = BLEND_MUL_SCALAR##version(a, w); } 
  MAKE_BLEND_JOB;
  #ifndef NO_MMX
  BLEND_MMX_END();
  #endif
  return c;
}

// -----------------------------------------------------------------------

// NON MMX

#ifndef NO_PREMUL

// PREMULTIPLIED PIXELS VERSION

// average blend of a and b.
unsigned int inline Blenders::BLEND_AVG_FAST_C(unsigned int a, unsigned int b) {
  return ((a>>1)&~((1<<7)|(1<<15)|(1<<23)))+((b>>1)&~((1<<7)|(1<<15)|(1<<23)));
}

// multiplies 32 bit color A by scalar V (0-255)
unsigned int inline Blenders::BLEND_MUL_SCALAR_FAST_C(unsigned int a, int v) {
  register int t;
  t=Blenders::alphatable[a&0xFF][v];
  t|=Blenders::alphatable[(a&0xFF00)>>8][v]<<8;
  t|=Blenders::alphatable[(a&0xFF0000)>>16][v]<<16;
  t|=Blenders::alphatable[(a&0xFF000000)>>24][v]<<24;
  return t;
}


// V is scalar (0-255), (1.0-V)*b + V*a
unsigned int inline Blenders::BLEND_ADJ1_FAST_C(unsigned int a, unsigned int b, int v){
  register int t,z;
  t=Blenders::alphatable[b&0xFF][0xFF-v]+Blenders::alphatable[a&0xFF][v];
  z=Blenders::alphatable[(b&0xFF00)>>8][0xFF-v]+Blenders::alphatable[(a&0xFF00)>>8][v];
  t|=z<<8;
  z=Blenders::alphatable[(b&0xFF0000)>>16][0xFF-v]+Blenders::alphatable[(a&0xFF0000)>>16][v];
  t|=z<<16;
  z=Blenders::alphatable[(b&0xFF000000)>>24][0xFF-v]+Blenders::alphatable[(a&0xFF000000)>>24][v];
  t|=z<<24;
  return t;
}

// returns a*(1.0-Alpha(b)) + b
unsigned int inline Blenders::BLEND_ADJ2_FAST_C(unsigned int a, unsigned int b) {
  register int t,z;
  int v=0xff-((b>>24)&0xff);
  if (v==0) return b;
  t=Blenders::alphatable[a&0xFF][v]+(b&0xFF);
  if (t > 255) t=255;
  z=(Blenders::alphatable[(a&0xFF00)>>8][v])+((b&0xFF00)>>8);
  if (z > 255) z=255;
  t|=z<<8;
  z=(Blenders::alphatable[(a&0xFF0000)>>16][v])+((b&0xFF0000)>>16);
  if (z > 255) z=255;
  t|=z<<16;
  z=(Blenders::alphatable[(a&0xFF000000)>>24][v])+((b&0xFF000000)>>24);
  if (z > 255) z=255;
  t|=z<<24;
  return t;
}

// returns a*(1-Alpha(b)*W) + b*W, clamped (W is scalar 0-0xff). 
unsigned int inline Blenders::BLEND_ADJ3_FAST_C(unsigned int a, unsigned int b, int w){
  register int t,z;
  int v=0xff-Blenders::alphatable[(b>>24)&0xff][w];

  t=Blenders::alphatable[a&0xFF][v]+Blenders::alphatable[b&0xFF][w];
  if (t > 255) t=255;
  z=Blenders::alphatable[(a&0xFF00)>>8][v]+Blenders::alphatable[(b&0xFF00)>>8][w];
  if (z > 255) z=255;
  t|=z<<8;
  z=Blenders::alphatable[(a&0xFF0000)>>16][v]+Blenders::alphatable[(b&0xFF0000)>>16][w];
  if (z > 255) z=255;
  t|=z<<16;
  z=Blenders::alphatable[(a&0xFF000000)>>24][v]+Blenders::alphatable[(b&0xFF000000)>>24][w];
  if (z > 255) z=255;
  t|=z<<24;
  return t;
}

unsigned int __inline Blenders::BLEND4_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp) {
  register int t;
  unsigned char a1,a2,a3,a4;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=alphatable[255-xp][255-yp];
  a2=alphatable[xp][255-yp];
  a3=alphatable[255-xp][yp];
  a4=alphatable[xp][yp];
  t=alphatable[p1[0]&0xff][a1]+alphatable[p1[1]&0xff][a2]+alphatable[p1[w]&0xff][a3]+alphatable[p1[w+1]&0xff][a4];
  t|=(alphatable[(p1[0]>>8)&0xff][a1]+alphatable[(p1[1]>>8)&0xff][a2]+alphatable[(p1[w]>>8)&0xff][a3]+alphatable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(alphatable[(p1[0]>>16)&0xff][a1]+alphatable[(p1[1]>>16)&0xff][a2]+alphatable[(p1[w]>>16)&0xff][a3]+alphatable[(p1[w+1]>>16)&0xff][a4])<<16;
  t|=(alphatable[(p1[0]>>24)&0xff][a1]+alphatable[(p1[1]>>24)&0xff][a2]+alphatable[(p1[w]>>24)&0xff][a3]+alphatable[(p1[w+1]>>24)&0xff][a4])<<24;
  return t;      
}

unsigned int __inline Blenders::BLEND4_FAST_C(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp) {
  register int t;
  unsigned char a1,a2,a3,a4;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=alphatable[255-xp][255-yp];
  a2=alphatable[xp][255-yp];
  a3=alphatable[255-xp][yp];
  a4=alphatable[xp][yp];
  t=alphatable[p1&0xff][a1]+alphatable[p2&0xff][a2]+alphatable[p3&0xff][a3]+alphatable[p4&0xff][a4];
  t|=(alphatable[(p1>>8)&0xff][a1]+alphatable[(p2>>8)&0xff][a2]+alphatable[(p3>>8)&0xff][a3]+alphatable[(p4>>8)&0xff][a4])<<8;
  t|=(alphatable[(p1>>16)&0xff][a1]+alphatable[(p2>>16)&0xff][a2]+alphatable[(p3>>16)&0xff][a3]+alphatable[(p4>>16)&0xff][a4])<<16;
  t|=(alphatable[(p1>>24)&0xff][a1]+alphatable[(p2>>24)&0xff][a2]+alphatable[(p3>>24)&0xff][a3]+alphatable[(p4>>24)&0xff][a4])<<24;
  return t;      
}

unsigned int __inline Blenders::BLEND4_LASTCOL_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp) {
  register int t;
  unsigned char a1,a3;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=alphatable[255-xp][255-yp];
  a3=alphatable[255-xp][yp];
  t=alphatable[p1[0]&0xff][a1]+alphatable[p1[w]&0xff][a3];
  t|=(alphatable[(p1[0]>>8)&0xff][a1]+alphatable[(p1[w]>>8)&0xff][a3])<<8;
  t|=(alphatable[(p1[0]>>16)&0xff][a1]+alphatable[(p1[w]>>16)&0xff][a3])<<16;
  t|=(alphatable[(p1[0]>>24)&0xff][a1]+alphatable[(p1[w]>>24)&0xff][a3])<<24;
  return t;      
}

unsigned int __inline Blenders::BLEND4_LASTLINE_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp) {
  register int t;
  unsigned char a1,a2;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=alphatable[255-xp][255-yp];
  a2=alphatable[xp][255-yp];
  t=alphatable[p1[0]&0xff][a1]+alphatable[p1[1]&0xff][a2];
  t|=(alphatable[(p1[0]>>8)&0xff][a1]+alphatable[(p1[1]>>8)&0xff][a2])<<8;
  t|=(alphatable[(p1[0]>>16)&0xff][a1]+alphatable[(p1[1]>>16)&0xff][a2])<<16;
  t|=(alphatable[(p1[0]>>24)&0xff][a1]+alphatable[(p1[1]>>24)&0xff][a2])<<24;
  return t;      
}

unsigned int __inline Blenders::BLEND4_LASTPIX_FAST_C(unsigned int *p1, unsigned int w, int xp, int yp) {
  register int t;
  unsigned char a1;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=alphatable[255-xp][255-yp];
  t=alphatable[p1[0]&0xff][a1];
  t|=(alphatable[(p1[0]>>8)&0xff][a1])<<8;
  t|=(alphatable[(p1[0]>>16)&0xff][a1])<<16;
  t|=(alphatable[(p1[0]>>24)&0xff][a1])<<24;
  return t;      
}

unsigned int __inline Blenders::BLEND_ADD_FAST_C(unsigned int a, unsigned int b, int w) {
  register int v;
  int t = (a&0xFF)+alphatable[b&0xFF][w];
  if (t > 255) t = 255;
  v = (((a&0xFF00)>>8)+alphatable[(b&0xFF00)>>8][w]);
  if (v > 255) v = 255;
  t |= v<<8;
  v = (((a&0xFF0000)>>16)+alphatable[(b&0xFF0000)>>16][w]);
  if (v > 255) v = 255;
  t |= v<<16;
  v = (((a&0xFF000000)>>24)+alphatable[(b&0xFF000000)>>24][w]);
  if (v > 255) v = 255;
  t |= v<<24;
  return t;
}

unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_FAST_C(unsigned int a, unsigned int b, int w) {
  return BLEND_ADD_FAST_C(a, b, w);
}

unsigned int __inline Blenders::BLEND_SUB_FAST_C(unsigned int a, unsigned int b, int w) {
  register int v;
  int t = (a&0xFF)-alphatable[b&0xFF][w];
  if (t < 0) t = 0;
  v = (((a&0xFF00)>>8)-alphatable[(b&0xFF00)>>8][w]);
  if (v < 0) v = 0;
  t |= v<<8;
  v = (((a&0xFF0000)>>16)-alphatable[(b&0xFF0000)>>16][w]);
  if (v < 0) v = 0;
  t |= v<<16;
  v = (((a&0xFF000000)>>24)-alphatable[(b&0xFF000000)>>24][w]);
  if (v < 0) v = 0;
  t |= v<<24;
  return t;
}

#ifndef NO_MMX

#ifdef WIN32
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
#endif

/// MMX

// average blend of a and b.
unsigned int inline Blenders::BLEND_AVG_FAST_MMX(unsigned int a, unsigned int b) {
  return ((a>>1)&~((1<<7)|(1<<15)|(1<<23)))+((b>>1)&~((1<<7)|(1<<15)|(1<<23)));
}

// multiplies 32 bit color A by scalar V (0-255)
unsigned int inline Blenders::BLEND_MUL_SCALAR_FAST_MMX(unsigned int a, int v)
{
#ifdef WIN32
  __asm
  {
    movd mm3, [v] // VVVVVVVV
    
    movd mm0, [a]
    packuswb mm3, mm3 // 0000HHVV

    punpcklbw mm0, [Blenders__mmx_zero]
    punpcklwd mm3, mm3 // HHVVHHVV
        
    punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV

    pmullw mm0, mm3      
    
    psrlw mm0, 8
    
    packuswb mm0, mm0

    movd eax, mm0
  }
#else
  __asm__ volatile (
		    "movd %0, %%mm3\n"
		    "movd %1, %%mm0\n"
		    "packuswb %%mm3, %%mm3\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm0\n"
		    "punpcklwd %%mm3, %%mm3\n"        
		    "punpckldq %%mm3, %%mm3\n"
		    "pmullw %%mm3, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "packuswb %%mm0, %%mm0\n"
		    "movd %%mm0, %%eax\n"
		    :
		    : "m" (v), "m" (a)
		    : "%mm0", "%mm3" );
#endif
}


// V is scalar (0-255), (1.0-V)*b + V*a
unsigned int inline Blenders::BLEND_ADJ1_FAST_MMX(unsigned int a, unsigned int b, int v)
{
#ifdef WIN32
  __asm
  {
    movd mm3, [v] // VVVVVVVV
    
    movd mm0, [a]
    packuswb mm3, mm3 // 0000HHVV

    movd mm1, [b]
    paddusw mm3, [Blenders__mmx_one]
        
    movq mm4, [Blenders__mmx_revn2]
    punpcklwd mm3, mm3 // HHVVHHVV

    punpcklbw mm0, [Blenders__mmx_zero]
    punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV

    punpcklbw mm1, [Blenders__mmx_zero]
    psubw mm4, mm3

    pmullw mm0, mm3      
    pmullw mm1, mm4
    
    paddw mm0, mm1

    psrlw mm0, 8
    
    packuswb mm0, mm0

    movd eax, mm0
  }
#else
  __asm__ volatile (
		    "movd %0, %%mm3\n"
		    "movd %1, %%mm0\n"
		    "packuswb %%mm3, %%mm3\n"
		    "movd %2, %%mm1\n"
		    "paddusw (Blenders__mmx_one), %%mm3\n"
		    "movq (Blenders__mmx_revn2), %%mm4\n"
		    "punpcklwd %%mm3, %%mm3\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm0\n"
		    "punpckldq %%mm3, %%mm3\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm1\n"
		    "psubw %%mm3, %%mm4\n"
		    "pmullw %%mm3, %%mm0\n"
		    "pmullw %%mm4, %%mm1\n"
		    "paddw %%mm1, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "packuswb %%mm0, %%mm0\n"
		    "movd %%mm0, %%eax\n"
		    :
		    : "m" (v), "m" (a), "m" (b)
		    : "%mm0", "%mm1", "%mm3", "%mm4" );
#endif
}

// returns a*(1.0-Alpha(b)) + b
unsigned int inline Blenders::BLEND_ADJ2_FAST_MMX(unsigned int a, unsigned int b)
{
#ifdef WIN32
  __asm
  {
    movd mm3, [b] // VVVVVVVV
    movq mm4, [Blenders__mmx_revn2]
    
    movd mm0, [a]
    psrld mm3, 24

    movd mm1, [b]
    paddusw mm3, [Blenders__mmx_one]
    
    punpcklwd mm3, mm3 // HHVVHHVV
    punpcklbw mm0, [Blenders__mmx_zero]
        
    punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV
    punpcklbw mm1, [Blenders__mmx_zero]

    psubw mm4, mm3

    pmullw mm0, mm4      
    // stall

    // stall
    
    // stall
    
    psrlw mm0, 8
    // stall

    paddw mm0, mm1
    // stall
    
    packuswb mm0, mm0
    // stall

    movd eax, mm0
  }
#else
  __asm__ volatile (
		    "movd %1, %%mm3\n"
		    "movq (Blenders__mmx_revn2), %%mm4\n"
		    "movd %0, %%mm0\n"
		    "psrld $24, %%mm3\n"
		    "movd %1, %%mm1\n"
		    "paddusw (Blenders__mmx_one), %%mm3\n"
		    "punpcklwd %%mm3, %%mm3\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm0\n"
		    "punpckldq %%mm3, %%mm3\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm1\n"
		    "psubw %%mm3, %%mm4\n"
		    "pmullw %%mm4, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "paddw %%mm1, %%mm0\n"
		    "packuswb %%mm0, %%mm0\n"
		    "movd %%mm0, %%eax\n"
		    :
		    : "m" (a), "m" (b)
		    : "%esi", "%mm0", "%mm1", "%mm3", "%mm4" );
#endif
}

// returns a*(1-Alpha(b)*W) + b*W, clamped (W is scalar 0-0xff). 
unsigned int inline Blenders::BLEND_ADJ3_FAST_MMX(unsigned int a, unsigned int b, int w)
{
#ifdef WIN32
  __asm
  {
    movd mm3, [b] // VVVVVVVV
    movd mm5, [w]

    movd mm0, [a]
    psrld mm3, 24

    movd mm1, [b]
    paddusw mm3, [Blenders__mmx_one]

    movq mm4, [Blenders__mmx_revn2]
    pmullw mm3, mm5

    packuswb mm5, mm5 
    punpcklbw mm0, [Blenders__mmx_zero]

    punpcklwd mm5, mm5        
    punpcklbw mm1, [Blenders__mmx_zero]

    psrlw mm3, 8
    punpckldq mm5, mm5
        
    paddusw mm3, [Blenders__mmx_one]

    punpcklwd mm3, mm3 // HHVVHHVV

    punpckldq mm3, mm3 // HHVVHHVV HHVVHHVV


    psubw mm4, mm3

    pmullw mm0, mm4      
    pmullw mm1, mm5
    
    paddusw mm0, mm1

    psrlw mm0, 8
    
    packuswb mm0, mm0

    movd eax, mm0
  }
#else
  __asm__ volatile (
		    "movd %2, %%mm3\n"
		    "movd %0, %%mm5\n"
		    "movd %1, %%mm0\n"
		    "psrld $24, %%mm3\n"
		    "movd %2, %%mm1\n"
		    "paddusw (Blenders__mmx_one), %%mm3\n"
		    "movq (Blenders__mmx_revn2), %%mm4\n"
		    "pmullw %%mm5, %%mm3\n"
		    "packuswb %%mm5, %%mm5 \n"
		    "punpcklbw (Blenders__mmx_zero), %%mm0\n"
		    "punpcklwd %%mm5, %%mm5\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm1\n"
		    "psrlw $8, %%mm3\n"
		    "punpckldq %%mm5, %%mm5\n"
		    "paddusw (Blenders__mmx_one), %%mm3\n"
		    "punpcklwd %%mm3, %%mm3\n"
		    "punpckldq %%mm3, %%mm3\n"
		    "psubw %%mm3, %%mm4\n"
		    "pmullw %%mm4, %%mm0\n"
		    "pmullw %%mm5, %%mm1\n"
		    "paddusw %%mm1, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "packuswb %%mm0, %%mm0\n"
		    "movd %%mm0, %%eax\n"

		    :
		    : "m" (w), "m" (a), "m" (b)
		    : "%mm0", "%mm1", "%mm4", "%mm3", "%mm5" );
#endif
}

// does bilinear filtering. p1 is upper left pixel, w is width of framebuffer
// xp and yp's low 16 bits are used for the subpixel positioning.
unsigned int inline Blenders::BLEND4_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp)
{
#ifdef WIN32
  __asm
  {
    movd mm6, xp
    mov eax, p1

    movd mm7, yp
    mov esi, w

    movq mm4, Blenders__mmx_revn2
    psrlw mm6, 8

    movq mm5, Blenders__mmx_revn2
    psrlw mm7, 8

    movd mm0, [eax]
    punpcklwd mm6,mm6

    movd mm1, [eax+4]
    punpcklwd mm7,mm7

    movd mm2, [eax+esi*4]
    punpckldq mm6,mm6

    movd mm3, [eax+esi*4+4]
    punpckldq mm7,mm7

    punpcklbw mm0, [Blenders__mmx_zero]
    psubw mm4, mm6

    punpcklbw mm1, [Blenders__mmx_zero]
    pmullw mm0, mm4

    punpcklbw mm2, [Blenders__mmx_zero]
    pmullw mm1, mm6

    punpcklbw mm3, [Blenders__mmx_zero]
    psubw mm5, mm7

    pmullw mm2, mm4
    pmullw mm3, mm6

    paddw mm0, mm1
    // stall (mm0)

    psrlw mm0, 8
    // stall (waiting for mm3/mm2)

    paddw mm2, mm3
    pmullw mm0, mm5

    psrlw mm2, 8
    // stall (mm2)

    pmullw mm2, mm7
    // stall

    // stall (mm2)

    paddw mm0, mm2
    // stall

    psrlw mm0, 8
    // stall

    packuswb mm0, mm0
    // stall

    movd eax, mm0
  }
#else
  __asm__ volatile (
		    "movd %2, %%mm6\n"
		    "mov %0, %%eax\n"
		    "movd %3, %%mm7\n"
		    "mov %1, %%esi\n"
		    "movq (Blenders__mmx_revn2), %%mm4\n"
		    "psrlw $8, %%mm6\n"
		    "movq (Blenders__mmx_revn2), %%mm5\n"
		    "psrlw $8, %%mm7\n"
		    "movd (%%eax), %%mm0\n"
		    "punpcklwd %%mm6,%%mm6\n"
		    "movd 4(%%eax), %%mm1\n"
		    "punpcklwd %%mm7,%%mm7\n"
		    "movd (%%eax,%%esi,4), %%mm2\n"
		    "punpckldq %%mm6,%%mm6\n"
		    "movd 4(%%eax,%%esi,4), %%mm3\n"
		    "punpckldq %%mm7,%%mm7\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm0\n"
		    "psubw %%mm6, %%mm4\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm1\n"
		    "pmullw %%mm4, %%mm0\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm2\n"
		    "pmullw %%mm6, %%mm1\n"
		    "punpcklbw (Blenders__mmx_zero), %%mm3\n"
		    "psubw %%mm7, %%mm5\n"
		    "pmullw %%mm4, %%mm2\n"
		    "pmullw %%mm6, %%mm3\n"
		    "paddw %%mm1, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "paddw %%mm3, %%mm2\n"
		    "pmullw %%mm5, %%mm0\n"
		    "psrlw $8, %%mm2\n"
		    "pmullw %%mm7, %%mm2\n"
		    "paddw %%mm2, %%mm0\n"
		    "psrlw $8, %%mm0\n"
		    "packuswb %%mm0, %%mm0\n"
		    "movd %%mm0, %%eax\n"

		    :
		    : "m" (p1), "m" (w), "m" (xp), "m" (yp)
		    : "%mm0", "%mm1", "%mm4", "%mm3", "%mm5" );

#endif
}

unsigned int inline Blenders::BLEND4_FAST_MMX(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp) {
  return Blenders::BLEND4_FAST_C(p1, p2, p3, p4, xp, yp); // no MMX version for now
}

unsigned int inline Blenders::BLEND4_LASTPIX_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp) {
  return BLEND4_LASTPIX_FAST_C(p1, w, xp, yp); // no MMX version for now
}

unsigned int inline Blenders::BLEND4_LASTLINE_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp) {
  return BLEND4_LASTLINE_FAST_C(p1, w, xp, yp);  // no MMX version for now
}

unsigned int inline Blenders::BLEND4_LASTCOL_FAST_MMX(unsigned int *p1, unsigned int w, int xp, int yp) {
  return BLEND4_LASTCOL_FAST_C(p1, w, xp, yp);  // no MMX version for now
}

unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_PREMULTIPLIED_ADD_FAST_C(a, b, w);  // no MMX version for now
}

unsigned int inline Blenders::BLEND_ADD_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_ADD_FAST_C(a, b, w);  // no MMX version for now
}

unsigned int inline Blenders::BLEND_SUB_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_SUB_FAST_C(a, b, w);  // no MMX version for now
}

unsigned int inline Blenders::BLEND_PREMULTIPLIED_ADD(unsigned int a, unsigned int b, int alpha) {
  return BLEND_ADD_FAST_MMX(a, b, alpha);
}

#ifdef _MSC_VER
#pragma warning( pop ) 
#endif

#else	// ndef NO_MMX

unsigned int inline Blenders::BLEND_PREMULTIPLIED_ADD(unsigned int a, unsigned int b, int alpha) {
  return BLEND_ADD_FAST_C(a, b, alpha);
}

#endif

#ifdef WIN32
#pragma warning( pop ) 
#endif

#else

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// **                                                                   **
// **                NON PREMULTIPLIED PIXELS VERSION                   **
// **                                                                   **
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// Non Premultiplied Average blend
// -----------------------------------------------------------------------

// average blend of a and b.
unsigned int inline Blenders::BLEND_AVG_FAST_C(unsigned int a, unsigned int b) {
  return BLEND_ADJ1_FAST_C(a, b, 127);
}

unsigned int inline Blenders::BLEND_AVG_PRECISE_C(unsigned int a, unsigned int b) {
  return BLEND_ADJ1_PRECISE_C(a, b, 127);
}

#ifndef NO_MMX
unsigned int inline Blenders::BLEND_AVG_FAST_MMX(unsigned int a, unsigned int b) {
  return BLEND_ADJ1_FAST_MMX(a, b, 127);
}
#endif

#ifndef NO_SSE
unsigned int inline Blenders::BLEND_AVG_PRECISE_SSE(unsigned int a, unsigned int b) {
  return BLEND_ADJ1_PRECISE_SSE(a, b, 127);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied ARGB32/scalar multiplication
// -----------------------------------------------------------------------

// multiplies 32 bit color A by scalar V (0-255)
unsigned int inline Blenders::BLEND_MUL_SCALAR_FAST_C(unsigned int a, int v) {
  register int t;
  t=Blenders::alphatable[(a>>24)&0xFF][v];
  return (t<<24)|(a&0xFFFFFF);
}

#ifndef NO_MMX
unsigned int inline Blenders::BLEND_MUL_SCALAR_FAST_MMX(unsigned int a, int v) {
  return BLEND_MUL_SCALAR_FAST_C(a, v);
}
#endif

unsigned int inline Blenders::BLEND_MUL_SCALAR_PRECISE_C(unsigned int a, int v) {
  return BLEND_MUL_SCALAR_FAST_C(a, v);
}

#ifndef NO_SSE
unsigned int inline Blenders::BLEND_MUL_SCALAR_PRECISE_SSE(unsigned int a, int v) {
  return BLEND_MUL_SCALAR_FAST_C(a, v);
}
#endif


// -----------------------------------------------------------------------
// Non-premultiplied weighted average between a and b
// -----------------------------------------------------------------------

// returns a*(1.0-W) + b*W
unsigned int inline Blenders::BLEND_ADJ1_FAST_C(unsigned int a, unsigned int b, int w) {
  int b_a = b>>24;
  int v = 255 - w;
  int a_a_v = alphatable[a>>24][v];
  int b_a_w = alphatable[b_a][w];
  int ta = addtable[a_a_v][b_a_w];

  //if (!b_a_w) return a;
  //if (!ta) return 0;

  register FP_TYPE fp_afactor = fp_divtable[a_a_v][ta]; 
  register FP_TYPE fp_bfactor = fp_divtable[b_a_w][ta]; 

  return MAKEARGB(ta, (((a>>16)&0xFF)*fp_afactor + ((b>>16)&0xFF)*fp_bfactor) >> 16, 
                      (((a>>8)&0xFF)*fp_afactor + ((b>>8)&0xFF)*fp_bfactor) >> 16,
                      (((a)&0xFF)*fp_afactor + ((b)&0xFF)*fp_bfactor) >> 16);
}

#ifndef NO_MMX
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
unsigned int inline Blenders::BLEND_ADJ1_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_ADJ1_FAST_C(a, b, w);
  int b_a = b>>24;
  int v = 255 - w;
  int a_a_v = alphatable[a>>24][v];
  int b_a_w = alphatable[b_a][w];
  int ta = addtable[a_a_v][b_a_w];

  __declspec(align(16)) int fp_afactor = Blenders::fp15_divtable[a_a_v][ta]; 
  __declspec(align(16)) int fp_bfactor = Blenders::fp15_divtable[b_a_w][ta]; 

  __asm {
    shl [ta], 24

    pxor mm5, mm5

    movd mm2, [fp_afactor] // 00000000 0000ffff
    punpcklwd mm2, mm2     // 00000000 FFFFffff
    punpckldq mm2, mm2     // FFFFffff FFFFffff

    movd mm3, [fp_bfactor]
    punpcklwd mm3, mm3
    punpckldq mm3, mm3

    movd mm0, [a] // 00RRGGBB
    movd mm1, [b] // 00RRGGBB

    // pack & copy
    
    punpcklbw mm0, mm5 // 000000RR 00GG00BB
    punpcklbw mm1, mm5 // 000000RR 00GG00BB

    movq mm6, mm0
    movq mm7, mm1

    // begin unsigned 16bits packed mul (a*afactor, b*factor)

    pmulhw mm0, mm2  
    pmulhw mm1, mm3  

    pmullw mm6, mm2  // low word
    pmullw mm7, mm3  // low word

    psllw mm0, 1     // make room
    psllw mm1, 1

    psrlw mm6, 15    // discard all but 1 bit
    psrlw mm7, 15

    paddusb mm0, mm6 // add
    paddusb mm1, mm7 

    // end unsigned 16bit packed mul

    paddusb mm0, mm1 // add colors

    packuswb mm0, mm0// 00RRGGBB
    movd eax, mm0
    and eax, 0xFFFFFF
    or eax, [ta]
  }
}
#pragma warning( pop ) 
#endif

unsigned int inline Blenders::BLEND_ADJ1_PRECISE_C(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int v=0xff-w;
  int a_a_v = ((a>>24)&0xFF)*v;
  int a_b_w = ((b>>24)&0xFF)*w;

  float ta = (a_a_v + a_b_w)*_ONEOVER255;

  float afactor = a_a_v*_ONEOVER65025;
  float bfactor = a_b_w*_ONEOVER65025;

  if (!fastfnz(ta)) return 0;

  float tfactor;
  if (fastfaboveequal(ta, 255.0f)) { ta = 255.0f; tfactor = 1.0f; }
  else tfactor = 255.0f / ta;

  register int s,t,u;
  s = fastfrnd(((a&0xFF)*afactor + (b&0xFF)*bfactor) * tfactor);
  if (s > 255) s=255;
  t=fastfrnd((((a>>8)&0xFF)*afactor + ((b>>8)&0xFF)*bfactor) * tfactor);
  if (t > 255) t=255;
  u=fastfrnd((((a>>16)&0xFF)*afactor + ((b>>16)&0xFF)*bfactor) * tfactor);
  if (u > 255) u=255;

  return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
}

#ifndef NO_SSE
unsigned int inline Blenders::BLEND_ADJ1_PRECISE_SSE(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int v=0xff-w;
  __declspec(align(4)) int a_a_v = ((a>>24)&0xFF)*v;
  __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;

  __declspec(align(4)) float ta = (a_a_v + a_b_w)*_ONEOVER255;

  if (!fastfnz(ta)) return 0;
  __declspec(align(16)) float afactor = a_a_v*_ONEOVER65025;
  SSE_BLEND_PREMUL1(a, afactor);

  __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
  SSE_BLEND_PREMUL2(b, bfactor);

  SSE_BLEND_ADD();

  if (fastfaboveequal(ta, 255.0f)) ta = 255.0f;
  else SSE_BLEND_DEPREMUL(ta);

  return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied replace blend
// -----------------------------------------------------------------------

// returns a*(1.0-Alpha(b)) + b
unsigned int inline Blenders::BLEND_ADJ2_FAST_C(unsigned int a, unsigned int b) {
  int b_a = b>>24;
  int v = 255 - b_a;
  int a_a_v = alphatable[a>>24][v];
  int ta = addtable[a_a_v][b_a];

  //if (!b_a) return a;
  //if (!a_a_v) return b;
  //if (!ta) return 0;

  register FP_TYPE fp_afactor = fp_divtable[a_a_v][ta]; 
  register FP_TYPE fp_bfactor = fp_divtable[b_a][ta]; 

  return MAKEARGB(ta, (((a>>16)&0xFF)*fp_afactor + ((b>>16)&0xFF)*fp_bfactor) >> 16, 
                      (((a>>8)&0xFF)*fp_afactor + ((b>>8)&0xFF)*fp_bfactor) >> 16,
                      (((a)&0xFF)*fp_afactor + ((b)&0xFF)*fp_bfactor) >> 16);
}

#ifndef NO_MMX
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
unsigned int inline Blenders::BLEND_ADJ2_FAST_MMX(unsigned int a, unsigned int b) {
  return BLEND_ADJ2_FAST_C(a, b);
  unsigned int b_a = b>>24;
  int v = 255 - b_a;
  int a_a_v = alphatable[a>>24][v];
  int ta = addtable[a_a_v][b_a];

  __declspec(align(16)) int fp_afactor = Blenders::fp15_divtable[a_a_v][ta]; 
  __declspec(align(16)) int fp_bfactor = Blenders::fp15_divtable[b_a][ta]; 

  __asm {
    shl [ta], 24

    pxor mm5, mm5

    movd mm2, [fp_afactor] // 00000000 0000ffff
    punpcklwd mm2, mm2     // 00000000 FFFFffff
    punpckldq mm2, mm2     // FFFFffff FFFFffff

    movd mm3, [fp_bfactor]
    punpcklwd mm3, mm3
    punpckldq mm3, mm3

    movd mm0, [a] // 00RRGGBB
    movd mm1, [b] // 00RRGGBB

    // pack & copy
    
    punpcklbw mm0, mm5 // 000000RR 00GG00BB
    punpcklbw mm1, mm5 // 000000RR 00GG00BB

    movq mm6, mm0
    movq mm7, mm1

    // begin unsigned 16bits packed mul (a*afactor, b*factor)

    pmulhw mm0, mm2  
    pmulhw mm1, mm3  

    pmullw mm6, mm2  // low word
    pmullw mm7, mm3  // low word

    psllw mm0, 1     // make room
    psllw mm1, 1

    psrlw mm6, 15    // discard all but 1 bit
    psrlw mm7, 15

    paddusb mm0, mm6 // add
    paddusb mm1, mm7 

    // end unsigned 16bit packed mul

    paddusb mm0, mm1 // add colors

    packuswb mm0, mm0// 00RRGGBB
    movd eax, mm0
    and eax, 0xFFFFFF
    or eax, [ta]
  }
}
#pragma warning( pop ) 
#endif

unsigned int inline Blenders::BLEND_ADJ2_PRECISE_C(unsigned int a, unsigned int b) {
  __declspec(align(4)) int v=0xff-((b>>24)&0xff);

  int a_a_v = ((a>>24)&0xFF) * v;
  int a_b = (b>>24)&0xFF;

  float ta = a_a_v*_ONEOVER255 + a_b;

  if (!a_b) return a;
  if (!a_a_v) return b;
  if (!fastfnz(ta)) return 0;

  float afactor = a_a_v*_ONEOVER65025;
  float bfactor = a_b*_ONEOVER255;

  float tfactor;
  if (fastfaboveequal(ta, 255.0f)) { ta = 255.0f; tfactor = 1.0f; }
  else tfactor = 255.0f / ta;

  register int s,t,u;

  s = fastfrnd(((a&0xFF)*afactor + (b&0xFF)*bfactor) * tfactor);
  if (s > 255) s=255;
  t=fastfrnd((((a>>8)&0xFF)*afactor + ((b>>8)&0xFF)*bfactor) * tfactor);
  if (t > 255) t=255;
  u=fastfrnd((((a>>16)&0xFF)*afactor + ((b>>16)&0xFF)*bfactor) * tfactor);
  if (u > 255) u=255;

  return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
}

#ifndef NO_SSE
unsigned int inline Blenders::BLEND_ADJ2_PRECISE_SSE(unsigned int a, unsigned int b) {
  __declspec(align(4)) int v=0xff-((b>>24)&0xff);

  __declspec(align(4)) int a_a_v = ((a>>24)&0xFF) * v;
  __declspec(align(4)) int a_b = (b>>24)&0xFF;

  __declspec(align(4)) float ta = a_a_v*_ONEOVER255 + a_b;

  if (!a_b) return a;
  if (!a_a_v) return b;
  if (!fastfnz(ta)) return 0;

  __declspec(align(16)) float afactor = a_a_v*_ONEOVER65025;
  SSE_BLEND_PREMUL1(a, afactor);

  __declspec(align(16)) float bfactor = a_b*_ONEOVER255;
  SSE_BLEND_PREMUL2(b, bfactor);

  SSE_BLEND_ADD();

  if (fastfaboveequal(ta, 255.0f)) ta = 255.0f;
  else SSE_BLEND_DEPREMUL(ta);
  return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied normal blend
// -----------------------------------------------------------------------

// returns a*(1-Alpha(b)*W) + b*W, clamped (W is scalar 0-0xff). 
unsigned int inline Blenders::BLEND_ADJ3_FAST_C(unsigned int a, unsigned int b, int w) {
  int b_a = b>>24;
  int v = 255 - alphatable[b_a][w];
  int a_a_v = alphatable[a>>24][v];
  int b_a_w = alphatable[b_a][w];
  int ta = addtable[a_a_v][b_a_w];

  //if (!b_a_w) return a;
  //if (!ta) return 0;

  register FP_TYPE fp_afactor = fp_divtable[a_a_v][ta]; 
  register FP_TYPE fp_bfactor = fp_divtable[b_a_w][ta]; 

  return MAKEARGB(ta, (((a>>16)&0xFF)*fp_afactor + ((b>>16)&0xFF)*fp_bfactor) >> 16, 
                      (((a>>8)&0xFF)*fp_afactor + ((b>>8)&0xFF)*fp_bfactor) >> 16,
                      (((a)&0xFF)*fp_afactor + ((b)&0xFF)*fp_bfactor) >> 16);
}

#ifndef NO_MMX
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
unsigned int inline Blenders::BLEND_ADJ3_FAST_MMX(unsigned int a, unsigned int b, int w) {
  unsigned int b_a = b>>24;
  int v = 255 - Blenders::alphatable[b_a][w];
  int a_a_v = Blenders::alphatable[a>>24][v];
  int b_a_w = Blenders::alphatable[b_a][w];
  int ta = Blenders::addtable[a_a_v][b_a_w];

  __declspec(align(16)) int fp_afactor = Blenders::fp15_divtable[a_a_v][ta]; 
  __declspec(align(16)) int fp_bfactor = Blenders::fp15_divtable[b_a_w][ta]; 

  __asm {
    shl [ta], 24

    pxor mm5, mm5

    movd mm2, [fp_afactor] // 00000000 0000ffff
    punpcklwd mm2, mm2     // 00000000 FFFFffff
    punpckldq mm2, mm2     // FFFFffff FFFFffff

    movd mm3, [fp_bfactor]
    punpcklwd mm3, mm3
    punpckldq mm3, mm3

    movd mm0, [a] // 00RRGGBB
    movd mm1, [b] // 00RRGGBB

    // pack & copy
    
    punpcklbw mm0, mm5 // 000000RR 00GG00BB
    punpcklbw mm1, mm5 // 000000RR 00GG00BB

    movq mm6, mm0
    movq mm7, mm1

    // begin unsigned 16bits packed mul (a*afactor, b*factor)

    pmulhw mm0, mm2  
    pmulhw mm1, mm3  

    pmullw mm6, mm2  // low word
    pmullw mm7, mm3  // low word

    psllw mm0, 1     // make room
    psllw mm1, 1

    psrlw mm6, 15    // discard all but 1 bit
    psrlw mm7, 15

    paddusb mm0, mm6 // add
    paddusb mm1, mm7 

    // end unsigned 16bit packed mul

    paddusb mm0, mm1 // add colors

    packuswb mm0, mm0// 00RRGGBB
    movd eax, mm0
    and eax, 0xFFFFFF
    or eax, [ta]
  }
}
#pragma warning( pop ) 
#endif

unsigned int inline Blenders::BLEND_ADJ3_PRECISE_C(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int v=0xff-Blenders::alphatable[(b>>24)&0xff][w];
  int a_a_v = ((a>>24)&0xFF)*v;
  int a_b_w = ((b>>24)&0xFF)*w;

  float ta = (a_a_v + a_b_w)*_ONEOVER255;

  float afactor = a_a_v*_ONEOVER65025;
  float bfactor = a_b_w*_ONEOVER65025;
  float tfactor;

  if (!a_b_w) return a;
  if (!fastfnz(ta)) return 0;

  if (fastfaboveequal(ta, 255.0f)) { ta = 255.0f; tfactor = 1.0f; }
  else tfactor = 255.0f / ta;

  register int s,t,u;
  s = fastfrnd(((a&0xFF)*afactor + (b&0xFF)*bfactor) * tfactor);
  if (s > 255) s=255;
  t=fastfrnd((((a>>8)&0xFF)*afactor + ((b>>8)&0xFF)*bfactor) * tfactor);
  if (t > 255) t=255;
  u=fastfrnd((((a>>16)&0xFF)*afactor + ((b>>16)&0xFF)*bfactor) * tfactor);
  if (u > 255) u=255;

  return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
}

#ifndef NO_SSE
unsigned int inline Blenders::BLEND_ADJ3_PRECISE_SSE(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int v=0xff-Blenders::alphatable[(b>>24)&0xff][w];
  __declspec(align(4)) int a_a_v = ((a>>24)&0xFF)*v;
  __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;

  __declspec(align(4)) float ta = (a_a_v + a_b_w)*_ONEOVER255;

  if (!a_b_w) return a;
  if (!fastfnz(ta)) return 0;

  __declspec(align(16)) float afactor = a_a_v*_ONEOVER65025;
  SSE_BLEND_PREMUL1(a, afactor);

  __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
  SSE_BLEND_PREMUL2(b, bfactor);

  SSE_BLEND_ADD();

  if (fastfaboveequal(ta, 255.0f)) ta = 255.0f;
  else SSE_BLEND_DEPREMUL(ta);

  return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied additive blend
// -----------------------------------------------------------------------

unsigned int Blenders::BLEND_ADD_FAST_C(unsigned int a, unsigned int b, int w) {
  int b_a = b>>24;
  int a_a = a>>24;
  int b_a_w = alphatable[b_a][w];
  int ta = addtable[a_a][b_a_w];

  //if (!b_a_w) return a;
  //if (!ta) return 0;

  register int fp_afactor = fp_divtable[a_a][ta]; 
  register int fp_bfactor = fp_divtable[b_a_w][ta]; 

  int _r = (((a>>16)&0xFF)*fp_afactor + ((b>>16)&0xFF)*fp_bfactor) >> 16; 
  int _g = (((a>>8)&0xFF)*fp_afactor + ((b>>8)&0xFF)*fp_bfactor) >> 16;
  int _b = (((a)&0xFF)*fp_afactor + ((b)&0xFF)*fp_bfactor) >> 16;

  return MAKEARGB(ta, MIN(255, _r), MIN(255, _g), MIN(255, _b));
}

#ifndef NO_MMX
unsigned int Blenders::BLEND_ADD_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_ADD_FAST_C(a, b, w);
}
#endif

unsigned int Blenders::BLEND_ADD_PRECISE_C(unsigned int a, unsigned int b, int w) {
  int s,t,u;
  int a_a = (a>>24)&0xFF;
  int a_b_w = ((b>>24)&0xFF)*w;

  float ta = a_a+a_b_w*_ONEOVER255;

  float afactor = a_a*_ONEOVER255;
  float bfactor = a_b_w*_ONEOVER65025;
  float tfactor;

  if (!a_b_w) return a;
  if (!fastfnz(ta)) return 0;

  if (fastfaboveequal(ta, 255)) { ta = 255; tfactor = 1.0f; }
  else tfactor = 255.0f / ta;

  s = fastfrnd(((a&0xFF)*afactor + (b&0xFF)*bfactor) * tfactor);
  if (s > 255) s = 255;
  t = fastfrnd((((a>>8)&0xFF)*afactor + ((b>>8)&0xFF)*bfactor) * tfactor);
  if (t > 255) t = 255;
  u = fastfrnd((((a>>16)&0xFF)*afactor + ((b>>16)&0xFF)*bfactor) * tfactor);
  if (u > 255) u = 255;

  return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
}

#ifndef NO_SSE
unsigned int Blenders::BLEND_ADD_PRECISE_SSE(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;

  __declspec(align(4)) int a_a = (a>>24)&0xFF;
  __declspec(align(4)) float ta = a_a+a_b_w*_ONEOVER255;

  if (!a_b_w) return a;
  if (!fastfnz(ta)) return 0;

  __declspec(align(16)) float afactor = a_a*_ONEOVER255;
  SSE_BLEND_PREMUL1(a, afactor);

  __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
  SSE_BLEND_PREMUL2(b, bfactor);

  SSE_BLEND_ADD();

  if (fastfaboveequal(ta, 255)) ta = 255;
  else SSE_BLEND_DEPREMUL(ta);

  return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied subtractive blend
// -----------------------------------------------------------------------

unsigned int Blenders::BLEND_SUB_FAST_C(unsigned int a, unsigned int b, int w) {
  int a_a = a>>24;
  int b_a_w = alphatable[b>>24][w];
  int ta = subtable[a_a][b_a_w];

  if (!b_a_w) return a;
  if (!ta) return 0;

  register FP_TYPE fp_afactor = fp_divtable[a_a][ta]; 
  register FP_TYPE fp_bfactor = fp_divtable[b_a_w][ta]; 

  FP_TYPE _r = (((a>>16)&0xFF)*fp_afactor - ((b>>16)&0xFF)*fp_bfactor); 
  FP_TYPE _g = (((a>>8)&0xFF)*fp_afactor - ((b>>8)&0xFF)*fp_bfactor);
  FP_TYPE _b = (((a)&0xFF)*fp_afactor - ((b)&0xFF)*fp_bfactor);

  _r = MIN(_FP_255, _r); _g = MIN(_FP_255, _g);
  _b = MIN(_FP_255, _b); _r = MAX(_FP_0,   _r);
  _g = MAX(_FP_0,   _g); _b = MAX(_FP_0,   _b);

  return MAKEARGB(ta, _r>>16, _g>>16, _b>>16);
}

#ifndef NO_MMX
unsigned int Blenders::BLEND_SUB_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_SUB_FAST_C(a, b, w); // no mmx version  
}
#endif

unsigned int Blenders::BLEND_SUB_PRECISE_C(unsigned int a, unsigned int b, int w) {
  int s,t,u;
  int a_a = (a>>24)&0xFF;
  int a_b_w = ((b>>24)&0xFF)*w;

  float ta = a_a-a_b_w*_ONEOVER255;

  float afactor = a_a*_ONEOVER255;
  float bfactor = a_b_w*_ONEOVER65025;
  float tfactor = 255.0f / ta;

  if (!a_b_w) return a;
  if (ta <= 0) return 0;

  s = fastfrnd(((a&0xFF)*afactor - (b&0xFF)*bfactor) * tfactor);
  if (s < 0) s = 0; else if (s > 255) s = 255;
  t = fastfrnd((((a>>8)&0xFF)*afactor - ((b>>8)&0xFF)*bfactor) * tfactor);
  if (t < 0) t = 0; else if (t > 255) t = 255;
  u = fastfrnd((((a>>16)&0xFF)*afactor - ((b>>16)&0xFF)*bfactor) * tfactor);
  if (u < 0) u = 0; else if (u > 255) u = 255;

  return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
}

#ifndef NO_SSE
unsigned int Blenders::BLEND_SUB_PRECISE_SSE(unsigned int a, unsigned int b, int w) {
  __declspec(align(4)) int a_a = (a>>24)&0xFF;
  __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;

  __declspec(align(4)) float ta = a_a-a_b_w*_ONEOVER255;

  if (!a_b_w) return a;
  if (ta <= 0) return 0;

  __declspec(align(16)) float afactor = a_a*_ONEOVER255;
  SSE_BLEND_PREMUL1(a, afactor);

  __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
  SSE_BLEND_PREMUL2(b, bfactor);

  SSE_BLEND_SUB();

  if (fastfaboveequal(ta, 255)) ta = 255;
  else SSE_BLEND_DEPREMUL(ta);
  return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
}
#endif

// -----------------------------------------------------------------------
// Non-premultiplied quad blends
// -----------------------------------------------------------------------

#define DECL_BLEND4_PTR(version) \
unsigned int __inline Blenders::BLEND4_##version(unsigned int *p1, unsigned int w, int xp, int yp) { \
  register int t; \
  unsigned char a1,a2,a3,a4; \
  xp=(xp>>8)&0xff; \
  yp=(yp>>8)&0xff; \
  a1=alphatable[255-xp][255-yp]; \
  a2=alphatable[xp][255-yp]; \
  a3=alphatable[255-xp][yp]; \
  a4=alphatable[xp][yp]; \
  t = Blenders::BLEND_MUL_SCALAR_##version(*p1, a1); \
  t = Blenders::BLEND_ADD_##version(t, p1[1], a2); \
  t = Blenders::BLEND_ADD_##version(t, p1[w], a3); \
  t = Blenders::BLEND_ADD_##version(t, p1[w+1], a4); \
  return t; \
}

#define DECL_BLEND4_VARS(version) \
unsigned int __inline Blenders::BLEND4_##version(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, int xp, int yp) { \
  register int t; \
  unsigned char a1,a2,a3,a4; \
  xp=(xp>>8)&0xff; \
  yp=(yp>>8)&0xff; \
  a1=alphatable[255-xp][255-yp]; \
  a2=alphatable[xp][255-yp]; \
  a3=alphatable[255-xp][yp]; \
  a4=alphatable[xp][yp]; \
  t = Blenders::BLEND_MUL_SCALAR_##version(p1, a1); \
  t = Blenders::BLEND_ADD_##version(t, p2, a2); \
  t = Blenders::BLEND_ADD_##version(t, p3, a3); \
  t = Blenders::BLEND_ADD_##version(t, p4, a4); \
  return t; \
} 

#define DECL_BLEND4_PTR_LASTCOL(version) \
unsigned int __inline Blenders::BLEND4_LASTCOL_##version(unsigned int *p1, unsigned int w, int xp, int yp) { \
  register int t; \
  unsigned char a1,a3; \
  xp=(xp>>8)&0xff; \
  yp=(yp>>8)&0xff; \
  a1=alphatable[255-xp][255-yp]; \
  a3=alphatable[255-xp][yp]; \
  t = Blenders::BLEND_MUL_SCALAR_##version(*p1, a1); \
  t = Blenders::BLEND_ADD_##version(t, p1[w], a3); \
  return t; \
}

#define DECL_BLEND4_PTR_LASTLINE(version) \
unsigned int __inline Blenders::BLEND4_LASTLINE_##version(unsigned int *p1, unsigned int w, int xp, int yp) { \
  register int t; \
  unsigned char a1,a2; \
  xp=(xp>>8)&0xff; \
  yp=(yp>>8)&0xff; \
  a1=alphatable[255-xp][255-yp]; \
  a2=alphatable[xp][255-yp]; \
  t = Blenders::BLEND_MUL_SCALAR_##version(*p1, a1); \
  t = Blenders::BLEND_ADD_##version(t, p1[1], a2); \
  return t; \
}

#define DECL_BLEND4_PTR_LASTPIX(version) \
unsigned int __inline Blenders::BLEND4_LASTPIX_##version(unsigned int *p1, unsigned int w, int xp, int yp) { \
  register int t; \
  xp=(xp>>8)&0xff; \
  yp=(yp>>8)&0xff; \
  t = Blenders::BLEND_MUL_SCALAR_##version(*p1, alphatable[255-xp][255-yp]); \
  return t; \
} 

DECL_BLEND4_PTR(FAST_C);
DECL_BLEND4_VARS(FAST_C);
DECL_BLEND4_PTR_LASTCOL(FAST_C);
DECL_BLEND4_PTR_LASTLINE(FAST_C);
DECL_BLEND4_PTR_LASTPIX(FAST_C);
#ifndef NO_MMX
DECL_BLEND4_PTR(FAST_MMX);
DECL_BLEND4_VARS(FAST_MMX);
DECL_BLEND4_PTR_LASTCOL(FAST_MMX);
DECL_BLEND4_PTR_LASTLINE(FAST_MMX);
DECL_BLEND4_PTR_LASTPIX(FAST_MMX);
#endif
DECL_BLEND4_PTR(PRECISE_C);
DECL_BLEND4_VARS(PRECISE_C);
DECL_BLEND4_PTR_LASTCOL(PRECISE_C);
DECL_BLEND4_PTR_LASTLINE(PRECISE_C);
DECL_BLEND4_PTR_LASTPIX(PRECISE_C);
#ifndef NO_SSE
DECL_BLEND4_PTR(PRECISE_SSE);
DECL_BLEND4_VARS(PRECISE_SSE);
DECL_BLEND4_PTR_LASTCOL(PRECISE_SSE);
DECL_BLEND4_PTR_LASTLINE(PRECISE_SSE);
DECL_BLEND4_PTR_LASTPIX(PRECISE_SSE);
#endif

// -----------------------------------------------------------------------
// Premultiplied Add (so that you can still use it when you are using non-premul)
// -----------------------------------------------------------------------

// careful when using non premul and this function, you should only use it for non repeating blends (or you'll lose
// bits overtime). You should also pass premultiplied pixels as parameters, and demultiply the result before using 
// it in the rest of your app
unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD(unsigned int a, unsigned int b, int w) {
  #undef BLEND_JOB
  #define BLEND_JOB(version) return BLEND_PREMULTIPLIED_ADD##version(a, b, w);
  MAKE_BLEND_JOB;
}

unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_FAST_C(unsigned int a, unsigned int b, int w) {
  register int v;
  int t = (a&0xFF)+Blenders::alphatable[b&0xFF][w];
  if (t > 255) t = 255;
  v = (((a&0xFF00)>>8)+Blenders::alphatable[(b&0xFF00)>>8][w]);
  if (v > 255) v = 255;
  t |= v<<8;
  v = (((a&0xFF0000)>>16)+Blenders::alphatable[(b&0xFF0000)>>16][w]);
  if (v > 255) v = 255;
  t |= v<<16;
  v = (((a&0xFF000000)>>24)+Blenders::alphatable[(b&0xFF000000)>>24][w]);
  if (v > 255) v = 255;
  t |= v<<24;
  return t;
}

#ifndef NO_MMX
unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_FAST_MMX(unsigned int a, unsigned int b, int w) {
  return BLEND_PREMULTIPLIED_ADD_FAST_C(a, b, w);
}
#endif

unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_PRECISE_C(unsigned int a, unsigned int b, int w) {
  return BLEND_PREMULTIPLIED_ADD_FAST_C(a, b, w);
}

#ifndef NO_SSE
unsigned int __inline Blenders::BLEND_PREMULTIPLIED_ADD_PRECISE_SSE(unsigned int a, unsigned int b, int w) {
  return BLEND_PREMULTIPLIED_ADD_FAST_C(a, b, w);
}
#endif

// -----------------------------------------------------------------------
// AdditiveNoAlpha blend, pixels alpha values are not blended
// -----------------------------------------------------------------------
class BlendAddNoAlpha {
public:
  static __inline const char *getName() { return "Add Color"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    int s,t,u;
    int a_a = (a>>24)&0xFF;
    int a_b_w = ((b>>24)&0xFF)*w;

    float ta = a_a;

    float afactor = a_a*_ONEOVER255;
    float bfactor = a_b_w*_ONEOVER65025;
    float tfactor;

    if (!a_b_w) return a;
    if (!fastfnz(ta)) return 0;

    if (fastfaboveequal(ta, 255.0f)) { ta = 255.0f; tfactor = 1.0f; }
    else tfactor = 255.0f / ta;

    s = fastfrnd(((a&0xFF)*afactor + (b&0xFF)*bfactor) * tfactor);
    if (s > 255) s = 255;
    t = fastfrnd((((a>>8)&0xFF)*afactor + ((b>>8)&0xFF)*bfactor) * tfactor);
    if (t > 255) t = 255;
    u = fastfrnd((((a>>16)&0xFF)*afactor + ((b>>16)&0xFF)*bfactor) * tfactor);
    if (u > 255) u = 255;

    return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    __declspec(align(4)) int a_a = (a>>24)&0xFF;
    __declspec(align(4)) float ta = a_a;
    __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;

    if (!a_b_w) return a;
    if (!fastfnz(ta)) return 0;

    __declspec(align(16)) float afactor = a_a*_ONEOVER255;
    SSE_BLEND_PREMUL1(a, afactor);

    __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
    SSE_BLEND_PREMUL2(b, bfactor);

    SSE_BLEND_ADD();

    if (fastfaboveequal(ta, 255.0f)) ta = 255.0f;
    else SSE_BLEND_DEPREMUL(ta);

    return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
  }
#endif
  static __inline int isAssociative() { return 1; }
};

extern BlenderImpl<BlendAddNoAlpha> g_blend_addnoalpha;

// -----------------------------------------------------------------------
// Subtractive blend, pixels alpha values are not blended
// -----------------------------------------------------------------------
class BlendSubtractNoAlpha {
public:
  static __inline const char *getName() { return "Subtract Color"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    int s,t,u;
    int a_b_w = ((b>>24)&0xFF)*w;
    int a_a = (a>>24)&0xFF;

    float ta = a_a;

    float afactor = a_a*_ONEOVER255;
    float bfactor = a_b_w*_ONEOVER65025;
    float tfactor;

    if (!a_b_w) return a;
    if (!fastfnz(ta)) return 0;

    tfactor = 255.0f / ta;

    s = fastfrnd(((a&0xFF)*afactor - (b&0xFF)*bfactor) * tfactor);
    if (s < 0) s = 0;
    t = fastfrnd((((a>>8)&0xFF)*afactor - ((b>>8)&0xFF)*bfactor) * tfactor);
    if (t < 0) t = 0;
    u = fastfrnd((((a>>16)&0xFF)*afactor - ((b>>16)&0xFF)*bfactor) * tfactor);
    if (u < 0) u = 0;

    return s | (t<<8) | (u<<16) | fastfrnd(ta)<<24;
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    __declspec(align(4)) int a_a = (a>>24)&0xFF;
    __declspec(align(4)) int a_b_w = ((b>>24)&0xFF)*w;
    __declspec(align(4)) float ta = a_a;
  
    if (!a_b_w) return a;
    if (!fastfnz(ta)) return 0;

    __declspec(align(16)) float afactor = a_a*_ONEOVER255;
    SSE_BLEND_PREMUL1(a, afactor);

    __declspec(align(16)) float bfactor = a_b_w*_ONEOVER65025;
    SSE_BLEND_PREMUL2(b, bfactor);

    SSE_BLEND_SUB();

    SSE_BLEND_DEPREMUL(ta);
    return SSE_BLEND_RESULT() | (fastfrnd(ta)<<24);
  }
#endif
  static __inline int isAssociative() { return 1; }
};

extern BlenderImpl<BlendSubtractNoAlpha> g_blend_subtractnoalpha;

// -----------------------------------------------------------------------
// Multiplicative blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = (1-alpha_a)*color_b + (1-alpha_b)*color_a + color_a*color_b
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_MUL(float alpha_a, float alpha_b) {
  __asm movss xmm1, [alpha_a]    // load alpha_a in xmm1[0] 
  __asm shufps xmm1, xmm1, 0     
  __asm movss xmm3, [alpha_b]    // load alpha_b in xmm3[0] 
  __asm shufps xmm3, xmm3, 0     

  __asm movaps xmm4, __1
  __asm subps xmm4, xmm1         // 1-alpha_a
  __asm mulps xmm4, xmm2         // *color_b
  __asm movaps xmm5, xmm4        // save result in xmm5

  __asm movaps xmm4, __1
  __asm subps xmm4, xmm3         // 1-alpha_b
  __asm mulps xmm4, xmm0         // *color_a, keep result

  __asm mulps xmm0, xmm2         // color_a*color_b
  
  // add the 3 multiplications
  __asm addps xmm0, xmm4          
  __asm addps xmm0, xmm5
}
#endif

class BlendMultiply {
public:
  static __inline const char *getName() { return "Multiply"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = omqa*cb_r + omqb*ca_r + ca_r*cb_r;
    float qc_g = omqa*cb_g + omqb*ca_g + ca_g*cb_g;
    float qc_b = omqa*cb_b + omqb*ca_b + ca_b*cb_b;

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_MUL(qa, qb);     // color = (1-alpha_a)*color_b + (1-alpha_b)*color_a + color_a*color_b

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  // although multiplication is associative, multiplicative blend is not
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendMultiply> g_blend_multiply;

// -----------------------------------------------------------------------
// Screen blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = color_b + color_a - (color_a * color_b)
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_SCREEN() {
  __asm movaps xmm1, xmm2
  __asm mulps xmm1, xmm0   // ca * cb
  __asm addps xmm0, xmm2   // ca + cb
  __asm subps xmm0, xmm1   // - (ca * cb)
}
#endif

class BlendScreen {
public:
  static __inline const char *getName() { return "Screen"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w);// no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w);// no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float qr = 1.0f - (1.0f-qa) * (1.0f-qb);
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = cb_r + ca_r - ca_r*cb_r;
    float qc_g = cb_r + ca_r - ca_r*cb_r;
    float qc_b = cb_r + ca_r - ca_r*cb_r;

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_SCREEN();        // color = color_b + color_a - (color_a * color_b)

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendScreen> g_blend_screen;

// -----------------------------------------------------------------------
// Darken blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = min((1 - alpha_a) * color_b + color_a, (1 - alpha_b) * color_a + color_b)
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_DARKEN(float alpha_a, float alpha_b) {
  __asm movss xmm1, [alpha_a]    // load alpha_a in xmm1[0] 
  __asm shufps xmm1, xmm1, 0     
  __asm movss xmm3, [alpha_b]    // load alpha_b in xmm3[0] 
  __asm shufps xmm3, xmm3, 0     

  __asm movaps xmm4, __1
  __asm subps xmm4, xmm1         // 1-alpha_a
  __asm mulps xmm4, xmm2         // *color_b
  __asm addps xmm4, xmm0         // + color_a

  __asm movaps xmm5, __1
  __asm subps xmm5, xmm3         // 1-alpha_b
  __asm mulps xmm5, xmm0         // *color_a
  __asm addps xmm5, xmm2         // + color_b

  __asm minps xmm4, xmm5         // min
  __asm movaps xmm0, xmm4
}
#endif

class BlendDarken {
public:
  static __inline const char *getName() { return "Darken"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = min(omqa * cb_r + ca_r, omqb * ca_r + cb_r);
    float qc_g = min(omqa * cb_g + ca_g, omqb * ca_g + cb_g);
    float qc_b = min(omqa * cb_b + ca_b, omqb * ca_b + cb_b);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_DARKEN(qa, qb);  // color = min((1 - alpha_a) * color_b + color_a, (1 - alpha_b) * color_a + color_b)

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendDarken> g_blend_darken;

// -----------------------------------------------------------------------
// Lighten blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = max((1 - alpha_a) * color_b + color_a, (1 - alpha_b) * color_a + color_b)
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_LIGHTEN(float alpha_a, float alpha_b) {
  __asm movss xmm1, [alpha_a]    // load alpha_a in xmm1[0] 
  __asm shufps xmm1, xmm1, 0     
  __asm movss xmm3, [alpha_b]    // load alpha_b in xmm3[0] 
  __asm shufps xmm3, xmm3, 0     

  __asm movaps xmm4, __1
  __asm subps xmm4, xmm1         // 1-alpha_a
  __asm mulps xmm4, xmm2         // *color_b
  __asm addps xmm4, xmm0         // + color_a

  __asm movaps xmm5, __1
  __asm subps xmm5, xmm3         // 1-alpha_b
  __asm mulps xmm5, xmm0         // *color_a
  __asm addps xmm5, xmm2         // + color_b

  __asm maxps xmm4, xmm5         // max
  __asm movaps xmm0, xmm4
}
#endif

class BlendLighten {
public:
  static __inline const char *getName() { return "Lighten"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = max(omqa * cb_r + ca_r, omqb * ca_r + cb_r);
    float qc_g = max(omqa * cb_g + ca_g, omqb * ca_g + cb_g);
    float qc_b = max(omqa * cb_b + ca_b, omqb * ca_b + cb_b);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_LIGHTEN(qa, qb); // color = max((1 - alpha_a) * color_b + color_a, (1 - alpha_b) * color_a + color_b)

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendLighten> g_blend_lighten;

// -----------------------------------------------------------------------
// Difference blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = abs(color_a - color_b)
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_DIFFERENCE() {
  // todo :P
}
#endif

class BlendDifference {
public:
  static __inline const char *getName() { return "Difference"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = ca_r + cb_r - 2.0f*min(ca_r*qb, cb_r*qa);
    float qc_g = ca_g + cb_g - 2.0f*min(ca_g*qb, cb_g*qa);
    float qc_b = ca_b + cb_b - 2.0f*min(ca_b*qb, cb_b*qa);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    // todo: implement sse difference function
    return blend_PRECISE_C(a, b, w);
    
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_DIFFERENCE();

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendDifference> g_blend_difference;

// -----------------------------------------------------------------------
// Exclusion blend
// -----------------------------------------------------------------------

#ifndef NO_SSE
// color = 1-abs(1-color_a-color_b)
// expects two normalized & premultiplied colors, in xmm0 and xmm2
void __inline SSE_BLEND_EXCLUSION() {
  // todo :P
}
#endif

class BlendExclusion {
public:
  static __inline const char *getName() { return "Exclusion"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = (cb_r*qa + ca_r*qb - 2.0f*cb_r*ca_r) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    float qc_g = (cb_g*qa + ca_g*qb - 2.0f*cb_g*ca_g) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    float qc_b = (cb_b*qa + ca_b*qb - 2.0f*cb_b*ca_b) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    // todo: implement sse exclusion function
    return blend_PRECISE_C(a, b, w);

    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;
    float qa = ((a>>24)&0xFF) * _ONEOVER255;

    if (!fastfnz(qb)) return a;

    float qr = 1.0f - (1.0f-qa)*(1.0f-qb);

/*
    float ca_r = ((a)&0xFF)/255.0f * qa;
    float ca_g = ((a>>8)&0xFF)/255.0f * qa;
    float ca_b = ((a>>16)&0xFF)/255.0f * qa;
*/

    SSE_BLEND_NORMALIZE1(a);   // a = a / 255
    SSE_BLEND_PREMUL1_NORM(qa);    // a = a * qa

/*
    float cb_r = ((b)&0xFF)/255.0f * qb;
    float cb_g = ((b>>8)&0xFF)/255.0f * qb;
    float cb_b = ((b>>16)&0xFF)/255.0f * qb;
*/

    SSE_BLEND_NORMALIZE2(b);   // b = b / 255
    SSE_BLEND_PREMUL2_NORM(qb);     // b = b * qb

/*
    float qc_r = (1-qa)*cb_r + (1-qb)*ca_r + ca_r*cb_r;
    float qc_g = (1-qa)*cb_g + (1-qb)*ca_g + ca_g*cb_g;
    float qc_b = (1-qa)*cb_b + (1-qb)*ca_b + ca_b*cb_b;
*/

    SSE_BLEND_EXCLUSION();

/*
    float tfactor = (1.0f / qr) * 255.0f;
*/

    SSE_BLEND_DEPREMUL_NORM(qr);    // rgb = rgb / alpha
    SSE_BLEND_SCALE();         // rgb = rgb * 255

    //return RGB((int)(qc_r*tfactor+0.5f), (int)(qc_g*tfactor+0.5f), (int)(qc_b*tfactor+0.5f)) | (((int)(qr*255.0f+0.5f)) << 24);

    return SSE_BLEND_RESULT() | (fastfrnd(qr*255.0f)<<24);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendExclusion> g_blend_exclusion;

// -----------------------------------------------------------------------
// Overlay blend
// -----------------------------------------------------------------------

class BlendOverlay {
public:
  static __inline const char *getName() { return "Overlay"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version, ever
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r, qc_g, qc_b;

    if (fastfabove(2.0f*ca_r, qa)) qc_r = qb*qa - 2.0f*(qa - ca_r)*(qb - cb_r) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    else qc_r = 2.0f*cb_r*ca_r + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);

    if (fastfabove(2.0f*ca_g, qa)) qc_g = qb*qa - 2.0f*(qa - ca_g)*(qb - cb_g) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    else qc_g = 2.0f*cb_g*ca_g + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);

    if (fastfabove(2.0f*ca_b, qa)) qc_b = qb*qa - 2.0f*(qa - ca_b)*(qb - cb_b) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);
    else qc_b = 2.0f*cb_b*ca_b + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version, ever
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendOverlay> g_blend_overlay;

// -----------------------------------------------------------------------
// Hard Light blend
// -----------------------------------------------------------------------

class BlendHardLight {
public:
  static __inline const char *getName() { return "Hard Light"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version, ever
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r, qc_g, qc_b;

    if (fastfabove(2.0f*cb_r, qb)) qc_r = qb*qa - 2.0f*(qa - ca_r)*(qb - cb_r) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    else qc_r = 2.0f*cb_r*ca_r + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);

    if (fastfabove(2.0f*cb_g, qb)) qc_g = qb*qa - 2.0f*(qa - ca_g)*(qb - cb_g) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    else qc_g = 2.0f*cb_g*ca_g + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);

    if (fastfabove(2.0f*cb_b, qb)) qc_b = qb*qa - 2.0f*(qa - ca_b)*(qb - cb_b) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);
    else qc_b = 2.0f*cb_b*ca_b + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version, ever
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendHardLight> g_blend_hardlight;

// -----------------------------------------------------------------------
// Soft Light blend
// -----------------------------------------------------------------------

class BlendSoftLight {
public:
  static __inline const char *getName() { return "Soft Light"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version, ever
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r, qc_g, qc_b;

    if (qa == 0) qa = 0.0000001f;

    if (2.0f*cb_r < qb)
       qc_r = ca_r*(qb + (1.0f - ca_r/qa)*(2.0f*cb_r - qb)) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    else 
       qc_r = (ca_r*qb + (sqrt(ca_r/qa)*qa - ca_r)*(2.0f*cb_r - qb)) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);

    if (2.0f*cb_g < qb)
       qc_g = ca_g*(qb + (1.0f - ca_g/qa)*(2.0f*cb_g - qb)) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    else 
       qc_g = (ca_g*qb + (sqrt(ca_g/qa)*qa - ca_g)*(2.0f*cb_g - qb)) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);

    if (2.0f*cb_b < qb)
       qc_b = ca_b*(qb + (1.0f - ca_b/qa)*(2.0f*cb_b - qb)) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);
    else 
       qc_b = (ca_b*qb + (sqrt(ca_b/qa)*qa - ca_b)*(2.0f*cb_b - qb)) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version, ever
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendSoftLight> g_blend_softlight;

// -----------------------------------------------------------------------
// Color Dodge blend
// -----------------------------------------------------------------------

class BlendColorDodge {
public:
  static __inline const char *getName() { return "Color Dodge"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version, ever
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;
   
    float qc_r, qc_g, qc_b;
    if (qb == 0) qb = 0.0000001f;

    if (cb_r*qa + ca_r*qb >= qb*qa)
      qc_r = qb*qa + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    else
      qc_r = ca_r*qb/(1.0f-cb_r/qb) + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);

    if (cb_g*qa + ca_g*qb >= qb*qa)
      qc_g = qb*qa + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    else
      qc_g = ca_g*qb/(1.0f-cb_g/qb) + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);

    if (cb_b*qa + ca_b*qb >= qb*qa)
      qc_b = qb*qa + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);
    else
      qc_b = ca_b*qb/(1.0f-cb_b/qb) + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version, ever
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendColorDodge> g_blend_colordodge;

// -----------------------------------------------------------------------
// Color Burn blend
// -----------------------------------------------------------------------

class BlendColorBurn{
public:
  static __inline const char *getName() { return "Color Burn"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version, ever
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float omqa = 1.0f-qa;
    float omqb = 1.0f-qb;

    float qr = 1.0f - omqa * omqb;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r, qc_g, qc_b;

    if (cb_r*qa + ca_r*qb <= qb*qa || cb_r == 0)
      qc_r = cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
    else
      qc_r = qb*(cb_r*qa + ca_r*qb - qb*qa)/cb_r + cb_r*(1.0f - qa) + ca_r*(1.0f - qb);
   
    if (cb_g*qa + ca_g*qb <= qb*qa || cb_g == 0)
      qc_g = cb_g*(1.0f - qa) + ca_g*(1.0f - qb);
    else
      qc_g = qb*(cb_g*qa + ca_g*qb - qb*qa)/cb_g + cb_g*(1.0f - qa) + ca_g*(1.0f - qb);

    if (cb_b*qa + ca_b*qb <= qb*qa || cb_b == 0)
      qc_b = cb_b*(1.0f - qa) + ca_b*(1.0f - qb);
    else
      qc_b = qb*(cb_b*qa + ca_b*qb - qb*qa)/cb_b + cb_b*(1.0f - qa) + ca_b*(1.0f - qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version, ever
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendColorBurn> g_blend_colorburn;

// -----------------------------------------------------------------------
// Inside blend
// -----------------------------------------------------------------------

class BlendInside {
public:
  static __inline const char *getName() { return "Inside"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float qr = qa;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = cb_r*qa + ca_r * (1.0f-qb);
    float qc_g = cb_g*qa + ca_g * (1.0f-qb);
    float qc_b = cb_b*qa + ca_b * (1.0f-qb);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version yet
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendInside> g_blend_inside;

// -----------------------------------------------------------------------
// Outside blend
// -----------------------------------------------------------------------

class BlendOutside {
public:
  static __inline const char *getName() { return "Outside"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    float qa = ((a>>24)&0xFF) * _ONEOVER255;
    float qb = (((b>>24)&0xFF) * w) * _ONEOVER65025;

    if (!fastfnz(qb)) return a;

    float mqa = _ONEOVER255 * qa;
    float mqb = _ONEOVER255 * qb;

    float qr = qb + qa - qb*qa;
    float tfactor = (1.0f / qr) * 255.0f;

    float ca_r = ((a)&0xFF) * mqa;
    float ca_g = ((a>>8)&0xFF) * mqa;
    float ca_b = ((a>>16)&0xFF) * mqa;

    float cb_r = ((b)&0xFF) * mqb;
    float cb_g = ((b>>8)&0xFF) * mqb;
    float cb_b = ((b>>16)&0xFF) * mqb;

    float qc_r = ca_r + cb_r*(1.0f - qa);
    float qc_g = ca_g + cb_g*(1.0f - qa);
    float qc_b = ca_b + cb_b*(1.0f - qa);

    return RGB(fastfrnd(qc_r*tfactor), fastfrnd(qc_g*tfactor), fastfrnd(qc_b*tfactor)) | (fastfrnd(qr*255.0f) << 24);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version yet
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendOutside> g_blend_outside;

// -----------------------------------------------------------------------
// Opacify
// -----------------------------------------------------------------------

class BlendOpacify {
public:
  static __inline const char *getName() { return "Opacify"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    int qa = ((a>>24)&0xFF);
    if (!qa) return a;
    int qr = MIN(255, fastfrnd(qa + (((b>>24)&0xFF) * w) * _ONEOVER255));
    return (a & 0xFFFFFF) | qr<<24;
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version yet
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendOpacify> g_blend_opacify;

// -----------------------------------------------------------------------
// Erase
// -----------------------------------------------------------------------

class BlendErase {
public:
  static __inline const char *getName() { return "Erase"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast version yet
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no fast/mmx version yet
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    int qa = MAX(0, fastfrnd(((a>>24)&0xFF) - (((b>>24)&0xFF) * w) * _ONEOVER255));
    return (a & 0xFFFFFF) | qa<<24;
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return blend_PRECISE_C(a, b, w); // no sse version yet
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendErase> g_blend_erase;

// -----------------------------------------------------------------------
// None, blend(a,b) = b
// -----------------------------------------------------------------------

class BlendNone {
public:
  static __inline const char *getName() { return "None"; }
  static __inline unsigned int blend_FAST_C(ARGB32 a, ARGB32 b, signed int w) {
    return Blenders::BLEND_MUL_SCALAR_FAST_C(b, w);
  }
#ifndef NO_MMX
  static __inline unsigned int blend_FAST_MMX(ARGB32 a, ARGB32 b, signed int w) {
    return Blenders::BLEND_MUL_SCALAR_FAST_MMX(b, w);
  }
#endif
  static __inline unsigned int blend_PRECISE_C(ARGB32 a, ARGB32 b, signed int w) {
    return Blenders::BLEND_MUL_SCALAR_PRECISE_C(b, w);
  }
#ifndef NO_SSE
  static __inline unsigned int blend_PRECISE_SSE(ARGB32 a, ARGB32 b, signed int w) {
    return Blenders::BLEND_MUL_SCALAR_PRECISE_SSE(b, w);
  }
#endif
  static __inline int isAssociative() { return 0; }
};

extern BlenderImpl<BlendNone> g_blend_none;


#endif

#ifdef _MSC_VER
#pragma warning( pop ) 
#endif

#endif
