#include "precomp.h"
#include "blending.h"
#include <bfc/std.h>

#if !defined(WIN32) && !defined(LINUX)
#error port me!
#endif

THREADSTORAGE Stack<int> *Blenders::blitmodestack = NULL;
//#ifdef NO_PREMUL
//THREADSTORAGE int Blenders::blitmode = Blenders::PRECISE_BLIT;
//#else
THREADSTORAGE int Blenders::blitmode = Blenders::FAST_BLIT;
//#endif

class BlenderInit {
public:
  BlenderInit() { Blenders::init(); }
};
static BlenderInit blender_init;

int Blenders::inited = 0;

void Blenders::init() {
  if (!inited) {
    inited = 1;
    int i,j;
    float v;
    for (j=0;j<256;j++) {
      for (i=0;i<256;i++) {
        alphatable[i][j] = (unsigned char)(((i*j)/255.0f)+0.5f);//>>8;
        fp_alphatable[i][j] = (FP_TYPE)(65536.0f*(((i*j)/255.0f)));//>>8;
        if (j == 0) v = 0;
        else v = (float)i / (float)j;
        fp_divtable[i][j] = (int)(v*65536);
        fp15_divtable[i][j] = (int)(v*32767);
        demultable[i][j] = 255.0f/(float)i*(float)j;
        addtable[i][j] = MIN(255, i+j);
        subtable[i][j] = MAX(0, i-j);
      }
    }

    DWORD retval1,retval2;	
#ifdef WIN32
    __try { 		
      _asm {
        mov eax, 1  // set up CPUID to return processor version and features
                    // 0 = vendor string, 1 = version info, 2 = cache info
        _emit 0x0f  // code bytes = 0fh,  0a2h
        _emit 0xa2
        mov retval1, eax		
        mov retval2, edx
      }	
    } __except(EXCEPTION_EXECUTE_HANDLER) { retval1 = retval2 = 0;}
#else
    __asm__ volatile ( "movl $1, %%eax\n"
                       ".byte 15, 162\n"
                       "movl %%eax, %0\n"
                       "movl %%edx, %1\n"
                      : "=m" (retval1), "=m" (retval2) 
                      :  // No inputs...
                      : "%eax", "%edx", "%ebx" ); // somehow, %ebx gets clobbered too..
#endif
#ifndef NO_MMX
    mmx_available = retval1 && (retval2&0x800000);
#endif	//ndef NO_MMX
#ifndef NO_SSE
    sse_available = retval1 && (retval2&0x2000000);
#endif
  }
}

void Blenders::pushBlitMode(int fastorprecise) {
#ifdef NO_PREMUL
  if (!blitmodestack) blitmodestack = new Stack<int>;
  blitmodestack->push(blitmode);
  blitmode = fastorprecise;
#endif
}

void Blenders::popBlitMode() {
#ifdef NO_PREMUL
  ASSERT(blitmodestack && !blitmodestack->isempty());
  blitmodestack->pop(&blitmode);
#endif
}


int Blenders::mmx_available=0;
int Blenders::sse_available=0;

unsigned char Blenders::alphatable[256][256];
unsigned char Blenders::demultable[256][256];
unsigned char Blenders::addtable[256][256];
unsigned char Blenders::subtable[256][256];
FP_TYPE Blenders::fp_alphatable[256][256];
FP_TYPE Blenders::fp_divtable[256][256];
FP_TYPE Blenders::fp15_divtable[256][256];

#ifdef WIN32
#define MMX_CONST const
#else
#define MMX_CONST
#endif

unsigned int MMX_CONST Blenders__mmx_revn2[2]={0x01000100,0x01000100};
unsigned int MMX_CONST Blenders__mmx_zero[2]={0,0};
unsigned int MMX_CONST Blenders__mmx_one[2]={1,0};
unsigned int MMX_CONST Blenders__mmx_255[2]={0x00FF00FF,0x00FF00FF};
unsigned int MMX_CONST Blenders__mmx_lw255[2]={0x000000FF,0x00000000};

#undef MMX_CONST

BlenderImpl<BlendNormal> g_blend_normal;
BlenderImpl<BlendMedian> g_blend_median;
BlenderImpl<BlendAdd> g_blend_add;
BlenderImpl<BlendSubtract> g_blend_subtract;
#ifdef NO_PREMUL
BlenderImpl<BlendAddNoAlpha> g_blend_addnoalpha;
BlenderImpl<BlendSubtractNoAlpha> g_blend_subtractnoalpha;
BlenderImpl<BlendMultiply> g_blend_multiply;
BlenderImpl<BlendScreen> g_blend_screen;
BlenderImpl<BlendDarken> g_blend_darken;
BlenderImpl<BlendLighten> g_blend_lighten;
BlenderImpl<BlendDifference> g_blend_difference;
BlenderImpl<BlendExclusion> g_blend_exclusion;
BlenderImpl<BlendOverlay> g_blend_overlay;
BlenderImpl<BlendHardLight> g_blend_hardlight;
BlenderImpl<BlendSoftLight> g_blend_softlight;
BlenderImpl<BlendColorDodge> g_blend_colordodge;
BlenderImpl<BlendColorBurn> g_blend_colorburn;
BlenderImpl<BlendInside> g_blend_inside;
BlenderImpl<BlendOutside> g_blend_outside;
BlenderImpl<BlendOpacify> g_blend_opacify;
BlenderImpl<BlendErase> g_blend_erase;
BlenderImpl<BlendNone> g_blend_none;
#endif
