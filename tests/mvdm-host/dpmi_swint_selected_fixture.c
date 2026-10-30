#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#pragma pack(push,1)
typedef struct { USHORT CsSelector, Flags; ULONG Eip; } HANDLER;
#pragma pack(pop)
extern HANDLER DpmiInterruptHandlers[256];
extern BOOL __cdecl DpmiSwIntHandler(ULONG);
extern void __cdecl DpmiFaultHandlerIret32(void);
extern void __cdecl DpmiIntHandlerIret32(void);
static LDT_ENTRY descriptors[4]; LDT_ENTRY *Ldt=descriptors;
static BYTE memory[131072]; ULONG IntelBase=(ULONG)(ULONG_PTR)memory;
static USHORT ss=16,cs=0x20;
static ULONG sp=0x100,ip=0x30;
static ULONG flags=0x4321,msw,mappings,targets,dispatches,iret_reports;
USHORT CurrentAppFlags, DosxRmCodeSelector, LockedPMStackSel, LockedPMStackCount;
ULONG PMLockOrigEIP,PMLockOrigSS,PMLockOrigESP,DosxFaultHandlerIret,DosxFaultHandlerIretd;
ULONG DosxIntHandlerIret,DosxIntHandlerIretd,Cpu40PmStackInfoAddress,Cpu40LdtShadowAddress,DosxIret,DosxIretd;
/* Explicitly unreachable non-test object dependencies terminate rather than
 * pretending to implement unrelated DPMI services. */
#define UNREACHED(n) void n(void) { fprintf(stderr,"unexpected %s\n",#n); abort(); }
UNREACHED(getESI) UNREACHED(getEDI) UNREACHED(setECX) UNREACHED(setEDX)
UNREACHED(setEBP) UNREACHED(setESI) UNREACHED(setEDI)
UNREACHED(DpmiAllocateVirtualMemory) UNREACHED(RcErrorDialogBox) UNREACHED(TerminateVDM)
UNREACHED(getDX) UNREACHED(getBP) UNREACHED(getSI) UNREACHED(getDI)
UNREACHED(setAX) UNREACHED(setCX) UNREACHED(setDX) UNREACHED(getES) UNREACHED(getDS)
UNREACHED(setSTATUS) UNREACHED(getCR0) UNREACHED(setCR0)
UNREACHED(VdmInstallHardwareIntHandler) UNREACHED(VdmInstallSoftwareIntHandler)
UNREACHED(VdmInstallFaultHandler)
UNREACHED(mvdm_softpc_report_dpmi_iret16_state) UNREACHED(mvdm_softpc_report_dpmi_fault_dispatch)
UNREACHED(mvdm_softpc_report_dpmi_fault_stack)
USHORT getAX(void){return 1;} USHORT getBX(void){return 2;}
USHORT getCX(void){return 3;} USHORT getCS(void){return cs;}
USHORT getSS(void){return ss;} USHORT getSP(void){return (USHORT)sp;}
USHORT getIP(void){return (USHORT)ip;} ULONG getEIP(void){return ip;}
ULONG getEAX(void){return 0;} ULONG getEBP(void){return 0;}
ULONG getEBX(void){return 0;} ULONG getECX(void){return 0;}
ULONG getEDX(void){return 0;} ULONG getESP(void){return sp;}
ULONG getEFLAGS(void){return flags;} ULONG getMSW(void){return msw;}
void setEFLAGS(ULONG x){flags=x;} void setEAX(ULONG x){(void)x;}
void setCF(ULONG x){(void)x;} void setCS(USHORT x){cs=x;dispatches++;}
void setIP(USHORT x){ip=x;} void setEIP(ULONG x){ip=x;}
void setSP(USHORT x){sp=(sp & 0xffff0000u)|x;} void setESP(ULONG x){sp=x;}
void setSS(USHORT x){ss=x;}
LONG DpmiSetWowLdtEntry(ULONG a,ULONG b,ULONG c){(void)a;(void)b;(void)c;abort();}
void *Sim32pGetVDMPointer(ULONG address,ULONG mode){(void)address;(void)mode;mappings++;return memory;}
void mvdm_softpc_report_dpmi_swint(ULONG a,USHORT b,USHORT c,USHORT d,USHORT e,ULONG f)
{(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;}
void mvdm_softpc_report_dpmi_swint_target(ULONG a,USHORT b,ULONG c,USHORT d)
{(void)a;(void)b;(void)c;(void)d;targets++;}
void mvdm_softpc_report_dpmi_iret16(USHORT bits,USHORT a,USHORT b,USHORT c,USHORT d,ULONG e,ULONG f)
{(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;if(bits==32)++iret_reports;}
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(void){
  unsigned mode, depth;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==0 && targets==0 && flags==0x4321);
  descriptors[1].HighWord.Bits.Pres=1; DpmiInterruptHandlers[0x31].CsSelector=8;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==1 && dispatches==1);
  CHECK(sp==0xfa && *(USHORT *)(memory+0xfa)==0x30);
  msw=1; sp=2; descriptors[2].LimitLow=0xffff;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==2 && dispatches==1);
  puts("DPMI_SWINT_SELECTED_OBJECT_OK absent=1 v86=1 stack_failure=1");
  /* Real selected fault-return body, with controlled register and memory
   * boundaries. Different stack descriptor sizes select SP versus ESP. */
  for (mode=0;mode<2;++mode) for (depth=1;depth<=2;++depth) {
    ULONG offset=mode ? 0x10020u : 0x20u;
    memset(memory,0,sizeof(memory));
    *(ULONG *)(memory+offset+4)=0x12345678u;
    *(ULONG *)(memory+offset+8)=0x2345u;
    *(ULONG *)(memory+offset+12)=0x202u;
    *(ULONG *)(memory+offset+16)=0x87654321u;
    *(USHORT *)(memory+offset+20)=0x37u;
    descriptors[2].HighWord.Bits.Default_Big=mode;
    ss=16;sp=0x10020u;ip=0;cs=0;flags=0;
    LockedPMStackCount=(USHORT)depth;
    PMLockOrigEIP=0xabcdu;PMLockOrigSS=0x18u;PMLockOrigESP=0x200u;
    DpmiFaultHandlerIret32();
    CHECK(LockedPMStackCount==depth-1 && ip==0x12345678u && cs==0x2345u &&
          flags==0x202u && sp==0x87654321u && ss==0x37u);
  }
  puts("DPMI_FAULT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 registers=full");
  /* Nested return retains the locked stack; the last return constructs the
   * original 32-bit EIP/CS/EFLAGS frame on the restored application stack. */
  for (mode=0;mode<2;++mode) for (depth=1;depth<=2;++depth) {
    ULONG offset=mode ? 0x10020u : 0x20u;
    ULONG original=mode ? 0x10180u : 0x180u;
    memset(memory,0,sizeof(memory));
    *(ULONG *)(memory+offset+4)=0x2345u;
    *(ULONG *)(memory+offset+8)=0x202u;
    descriptors[2].HighWord.Bits.Default_Big=mode;
    descriptors[3].HighWord.Bits.Default_Big=mode;
    descriptors[3].HighWord.Bits.LimitHi=1;
    descriptors[3].LimitLow=0xffff;
    ss=16;sp=0x10020u;ip=0;cs=0;flags=0;
    CurrentAppFlags=1;LockedPMStackCount=(USHORT)depth;
    PMLockOrigEIP=0xabcdef80u;PMLockOrigSS=0x18u;PMLockOrigESP=original;
    DosxIretd=0x24680042u;
    DpmiIntHandlerIret32();
    CHECK(LockedPMStackCount==depth-1 && cs==0x2468u && ip==0x42u);
    if(depth==1){
      CHECK(ss==0x18u && sp==original-12u);
      CHECK(*(ULONG *)(memory+original-12u)==0xabcdef80u &&
            *(ULONG *)(memory+original-8u)==0x2345u &&
            *(ULONG *)(memory+original-4u)==0x202u);
    } else CHECK(ss==16 && sp==0x10020u);
  }
  CHECK(iret_reports==4);
  puts("DPMI_INT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 frame=full");
  return 0;
}
