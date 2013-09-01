#include <precomp.h>
#include "std_cpu.h"

// 31-may-2004 BU created

int StdCPU::getNumCPUs() {
#ifdef WIN32
  SYSTEM_INFO si;
  ZERO(si);
  GetSystemInfo(&si);
  return si.dwNumberOfProcessors;
#else
#ifdef PLATFORM_WARNINGS
#warning Std::getNumCPUs not implemented
#endif
  return 1;
#endif
}

// most of this stuff came from intel.com, blame them

#define HT_BIT      0x10000000  // EDX[28] - Bit 28 set indicates 
                                // Hyper-Threading Technology is supported 
                                // in hardware.
#define FAMILY_ID   0x0f00      // EAX[11:8] - Bit 11 thru 8 contains family 
                                // processor id
#define EXT_FAMILY_ID 0x0f00000 // EAX[23:20] - Bit 23 thru 20 contains 
                                // extended family  processor id
#define PENTIUM4_ID   0x0f00    // Pentium 4 family processor id

#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 


// Returns non-zero if Hyper-Threading Technology is supported on 
// the processors and zero if not.  This does not mean that 
// Hyper-Threading Technology is necessarily enabled.
static unsigned int HTSupported(void) {
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64
  unsigned int reg_eax = 0;
  unsigned int reg_edx = 0;
  unsigned int vendor_id[3] = {0, 0, 0};
  __try {            // verify cpuid instruction is supported
    __asm {
      xor   eax, eax    // call cpuid with eax = 0
      cpuid        // get vendor id string
      mov   vendor_id, ebx
      mov   vendor_id + 4, edx
      mov   vendor_id + 8, ecx
      mov   eax, 1      // call cpuid with eax = 1
      cpuid
      mov   reg_eax, eax // eax contains cpu family type info
      mov   reg_edx, edx  // edx has info whether Hyper-Threading
                          // Technology is available
      }
    }
    __except (EXCEPTION_EXECUTE_HANDLER ) {
      return 0;   // CPUID is not supported and so Hyper-Threading Technology
                  // is not supported
    }
  
    //  Check to see if this is a Pentium 4 or later processor
// BU note: this is originally Intel code... if AMD adds HT we need to tweak
  if (((reg_eax & FAMILY_ID) ==  PENTIUM4_ID) || (reg_eax & EXT_FAMILY_ID))
    if (vendor_id[0] == 'uneG') 
      if (vendor_id[1] == 'Ieni')
        if (vendor_id[2] == 'letn')
          return (reg_edx & HT_BIT);  // Genuine Intel Processor with 
                                      // Hyper-Threading Technology
  return 0;  // This is not a genuine Intel processor.
#else
  return 0;	//portme
#endif
}

#define NUM_LOGICAL_BITS	0x00FF0000 // EBX[23:16] indicate number of
                                           // logical processors per package
// Returns the number of logical processors per physical processors.
static unsigned char LogicalProcessorsPerPackage(void) {
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64
  unsigned int reg_ebx = 0;
  if (!HTSupported()) return (unsigned char) 1;
  __asm {
    mov	 eax, 1			// call cpuid with eax = 1
    cpuid
    mov	 reg_ebx, ebx           // Has info on number of logical processors	
  }
  return (unsigned char) ((reg_ebx & NUM_LOGICAL_BITS) >> 16);
#else
  return 1;// port me!
#endif
}

unsigned char GetAPIC_ID(void) {
#if defined(WIN32) && !defined(_WIN64) // no __asm on x64
  unsigned int Regebx = 0;
  if (!HTSupported()) return (unsigned char) -1;  // HT not supported
                                                  // Logical processor = 1
  __asm
  {
    mov eax, 1
    cpuid
    mov Regebx, ebx
  }

  return (unsigned char) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);
#else
  return (unsigned char)0;
#endif
}


int StdCPU::getNumPhysicalCPUs() {
  int ret = getNumCPUs();
#ifdef WIN32
  if (HTSupported()) {
    int logprocs = LogicalProcessorsPerPackage();

    if (logprocs >= 1) {    // >1 Doesn't mean HT is actually enabled
      HANDLE hCurrentProcessHandle;
      DWORD  dwProcessAffinity;
      DWORD  dwSystemAffinity;
      DWORD  dwAffinityMask;

      // Calculate the appropriate  shifts and mask based on the 
      // number of logical processors.

      unsigned char i = 1,
                    PHY_ID_MASK  = 0xFF,
                    PHY_ID_SHIFT = 0;

      while (i < logprocs) {
        i *= 2;
        PHY_ID_MASK <<= 1;
        PHY_ID_SHIFT++;
      }
                        
      hCurrentProcessHandle = GetCurrentProcess();
#ifdef _WIN64
#define W64CAST (PDWORD_PTR)
#else
#define W64CAST
#endif
      GetProcessAffinityMask(hCurrentProcessHandle, W64CAST &dwProcessAffinity, W64CAST &dwSystemAffinity);
#undef W64CAST
      // Check if available process affinity mask is equal to the
      // available system affinity mask
      if (dwProcessAffinity != dwSystemAffinity) {
//CUT        StatusFlag = HT_CANNOT_DETECT;
//CUT        *PhysicalNum = (unsigned char)-1;
//CUT        return StatusFlag;
        return 1;	// ummm?
      }

      dwAffinityMask = 1;

      int HT_Enabled = 0;

      while (dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity) {
        // Check if this CPU is available
        if (dwAffinityMask & dwProcessAffinity) {
          if (SetProcessAffinityMask(hCurrentProcessHandle, dwAffinityMask)) {
            unsigned char APIC_ID, LOG_ID, PHY_ID;
            Sleep(0); // Give OS time to switch CPU

            APIC_ID = GetAPIC_ID();
            LOG_ID  = APIC_ID & ~PHY_ID_MASK;
            PHY_ID  = APIC_ID >> PHY_ID_SHIFT;
 
            if (LOG_ID != 0)  HT_Enabled = 1;
          }
        }

        dwAffinityMask = dwAffinityMask << 1;
      }
             
      // Reset the processor affinity
      SetProcessAffinityMask(hCurrentProcessHandle, dwProcessAffinity);

      if (logprocs != 1 && HT_Enabled) {
        ret /= logprocs;
      }
    }
  }
#else
#ifdef PLATFORM_WARNINGS
#warning Std::getNumPhysicalCPUs not implemented
#endif
#endif
//  ASSERTPR(ret >= 1, "num CPU code failed!");
  if (ret < 1) ret = 1;
  return ret;
}

THREADID StdCPU::getCurrentThreadId() {
#ifdef WIN32
  return GetCurrentThreadId();
#else
  return (THREADID)pthread_self();
#endif
}

PROCESSID StdCPU::getCurrentProcessId() {
#ifdef WIN32
  return GetCurrentProcessId();
#else
  return getpid();
#endif
}

void StdCPU::setThreadPriority(int delta, OSTHREADHANDLE thread_handle) {
#ifdef WIN32
  if (thread_handle == NULL) thread_handle = GetCurrentThread();
  int v = THREAD_PRIORITY_NORMAL;
  switch (delta) {
    case -32767: v = THREAD_PRIORITY_IDLE; break;
    case -2: v = THREAD_PRIORITY_LOWEST; break;
    case -1: v = THREAD_PRIORITY_BELOW_NORMAL; break;
    case 1: v = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case 2: v = THREAD_PRIORITY_HIGHEST; break;
    case 32767: v = THREAD_PRIORITY_TIME_CRITICAL; break;
  }
  SetThreadPriority(thread_handle, v);
#else
#ifdef PLATFORM_WARNINGS
#warning Std::setThreadPriority not implemented on LINUX
#endif
#endif
}
