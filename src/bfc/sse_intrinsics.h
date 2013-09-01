#ifndef SSE_INTRINSICS_H
#define SSE_INTRINSICS_H

#include "xmmintrin.h"

// -----------------------------------------------------------------------
// A few useful blending macros
// -----------------------------------------------------------------------

const __m128 __ONEOVER65025 = {_ONEOVER65025, _ONEOVER65025, _ONEOVER65025, _ONEOVER65025};
const __m128 __ONEOVER255 = {_ONEOVER255, _ONEOVER255, _ONEOVER255, _ONEOVER255};
const __m128 __255= {255.0f, 255.0f, 255.0f, 255.0f};
const __m128 __0= {0.0f, 0.0f, 0.0f, 0.0f};
const __m128 __1= {1.0f, 1.0f, 1.0f, 1.0f};

// converts ARGB to packed float in xmm1, then multiplies it by the factor and stores the result in xmm0
// we use a different register from SSE_BLEND_MUL2 so that the CPU can execute both multiplications in parallel
void __inline SSE_BLEND_PREMUL1(unsigned int pix, float factor) {
  // signed int to float is MUCH faster than unsigned int to float
  __declspec(align(16)) float fpix[4]={(signed)((pix>>16)&0xFF), (signed)((pix>>8)&0xFF), (signed)(pix&0xFF), 0};

  __asm movss xmm0, [factor]     // load afactor in xmm0[0]
  __asm shufps xmm0, xmm0, 0     // copy xmm0[0] to [1], [2], [3] 
  __asm movaps xmm1, [fpix]      // load pixel a in xmm1 
  __asm mulps xmm0, xmm1         // multiply each color from pixel a by afactor 
}

// converts ARGB to packed float in xmm3, then multiplies it by the factor and stores the result in xmm2
// we use a different register from SSE_BLEND_PREMUL1 so that the CPU can execute both multiplications in parallel
void __inline SSE_BLEND_PREMUL2(unsigned int pix, float factor) {
  // signed int to float is MUCH faster than unsigned int to float
  __declspec(align(16)) float fpix[4]={(signed)((pix>>16)&0xFF), (signed)((pix>>8)&0xFF), (signed)(pix&0xFF), 0};

  __asm movss xmm2, [factor]     // load bfactor in xmm2[0] (use new registers so that the two blocks can be executed in parallel)
  __asm shufps xmm2, xmm2, 0     // copy xmm2[0] to [1], [2], [3] 
  __asm movaps xmm3, [fpix]      // load pixel b in xmm3 
  __asm mulps xmm2, xmm3         // multiply each color from pixel b by bfactor 
}

// demultiplies a packed floating point RGB value using alpha ta
void __inline SSE_BLEND_DEPREMUL(float ta) {
  __declspec(align(16)) float tfactor = 255.0f / ta;

  __asm movss xmm1, [tfactor]    // load tfactor in xmm1[0]
  __asm shufps xmm1, xmm1, 0     // copy xmm1[0] to [1], [2], [3] 
  __asm mulps xmm0, xmm1         // multiply the result of the color additions by tfactor 
}

// min(255, xmm0+xmm2)
void __inline SSE_BLEND_ADD() {
  __asm addps xmm0, xmm2         // add the result of both pixel multiplications
  __asm minps xmm0, __255
}

// max(0, xmm0-xmm2)
void __inline SSE_BLEND_SUB() {
  __asm subps xmm0, xmm2         // subtract the result of the second pixel multiplications from the first
  __asm maxps xmm0, __0
}

#ifdef WIN32
#pragma warning( push, 1 )
#pragma warning(disable: 4799)
#endif

// unpacks an RGB value
// this expects 3 floats (r, g, b) to be present in xmm0
unsigned int __inline SSE_BLEND_RESULT() {
  __asm {
    push ecx
    push ebx
    
    minps xmm0, __255

    movaps xmm1, xmm0
    movaps xmm2, xmm0
  
    cvtss2si eax, xmm0       // convert float to int 
  
    shufps xmm1, xmm1, 1     // xmm1[3,2,1,0] -> xmm1[0,0,0,1]
    cvtss2si ebx, xmm1       // convert float to int 
  
    shufps xmm2, xmm2, 2     // xmm1[3,2,1,0] -> xmm1[0,0,0,2]
    cvtss2si ecx, xmm2       // convert float to int 

    shl eax, 0x10
    shl ebx, 0x8
    or eax, ebx
    or eax, ecx
    pop ebx
    pop ecx
  }
}

#ifdef WIN32
#pragma warning( pop ) 
#endif

// normalized versions

// packs an rgb value into floats, then normalize each component into xmm0
void __inline SSE_BLEND_NORMALIZE1(unsigned int pix) {
  __declspec(align(16)) float fpix[4]={(signed)((pix>>16)&0xFF), (signed)((pix>>8)&0xFF), (signed)(pix&0xFF), 0};
  __asm movaps xmm0, [fpix]      // load pixel a in xmm0
  __asm divps xmm0, __255        // divide r,g,b by 255
}

// expects a normalized color in xmm0
void __inline SSE_BLEND_SCALE() {
  __asm mulps xmm0, __255
}

// packs an rgb value into floats, then normalize each component into xmm2
void __inline SSE_BLEND_NORMALIZE2(unsigned int pix) {
  __declspec(align(16)) float fpix[4]={(signed)((pix>>16)&0xFF), (signed)((pix>>8)&0xFF), (signed)(pix&0xFF), 0};
  __asm movaps xmm2, [fpix]      // load pixel a in xmm2 (use new registers so that the two blocks can be executed in parallel)
  __asm mulps xmm2, __ONEOVER255 // divide r,g,b by 255
}

// premultiplies a packed floating point rgb value by factor
// expects a normalized color in xmm0, sends result in xmm0
void __inline SSE_BLEND_PREMUL1_NORM(float factor) {
  __asm movss xmm1, [factor]     // load factor in xmm1[0] 
  __asm shufps xmm1, xmm1, 0     // copy xmm1[0] to [1], [2], [3] 
  __asm mulps xmm0, xmm1         // multiply r,g,b by factor
}

// premultiplies a packed floating point rgb value by factor
// expects a normalized color in xmm2, sends result in xmm2
void __inline SSE_BLEND_PREMUL2_NORM(float factor) {
  __asm movss xmm3, [factor]     // load factor in xmm2[0] (use new registers so that the two blocks can be executed in parallel)
  __asm shufps xmm3, xmm3, 0     // copy xmm3[0] to [1], [2], [3] 
  __asm mulps xmm2, xmm3         // multiply r,g,b by factor
}

// demultiplies a packed floating point rgb value by divisor
// expects a normalized & premultiplied color in xmm0, sends result in xmm0
void __inline SSE_BLEND_DEPREMUL_NORM(float divisor) {
  __asm movss xmm1, [divisor]    // load factor in xmm1[0] 
  __asm shufps xmm1, xmm1, 0     
  __asm divps xmm0, xmm1         // divide r,g,b by factor
}

// -----------------------------------------------------------------------


#endif
