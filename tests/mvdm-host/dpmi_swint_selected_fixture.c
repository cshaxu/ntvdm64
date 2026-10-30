#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#pragma pack(push,1)
typedef struct { USHORT CsSelector, Flags; ULONG Eip; } HANDLER;
#pragma pack(pop)
extern HANDLER DpmiInterruptHandlers[256];
extern BOOL __cdecl DpmiSwIntHandler(ULONG);
static LDT_ENTRY descriptors[4]; LDT_ENTRY *Ldt=descriptors;
static BYTE memory[65536]; ULONG IntelBase=(ULONG)(ULONG_PTR)memory;
static USHORT ss=16,sp=0x100,cs=0x20,ip=0x30;
static ULONG flags=0x4321,msw,mappings,targets,dispatches;
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
UNREACHED(setSS) UNREACHED(setSTATUS) UNREACHED(getCR0) UNREACHED(setCR0)
UNREACHED(VdmInstallHardwareIntHandler) UNREACHED(VdmInstallSoftwareIntHandler)
UNREACHED(VdmInstallFaultHandler) UNREACHED(mvdm_softpc_report_dpmi_iret16)
UNREACHED(mvdm_softpc_report_dpmi_iret16_state) UNREACHED(mvdm_softpc_report_dpmi_fault_dispatch)
UNREACHED(mvdm_softpc_report_dpmi_fault_stack)
USHORT getAX(void){return 1;} USHORT getBX(void){return 2;}
USHORT getCX(void){return 3;} USHORT getCS(void){return cs;}
USHORT getSS(void){return ss;} USHORT getSP(void){return sp;}
USHORT getIP(void){return ip;} ULONG getEIP(void){return ip;}
ULONG getEAX(void){return 0;} ULONG getEBP(void){return 0;}
ULONG getEBX(void){return 0;} ULONG getECX(void){return 0;}
ULONG getEDX(void){return 0;} ULONG getESP(void){return sp;}
ULONG getEFLAGS(void){return flags;} ULONG getMSW(void){return msw;}
void setEFLAGS(ULONG x){flags=x;} void setEAX(ULONG x){(void)x;}
void setCF(ULONG x){(void)x;} void setCS(USHORT x){cs=x;dispatches++;}
void setIP(USHORT x){ip=x;} void setEIP(ULONG x){ip=(USHORT)x;}
void setSP(USHORT x){sp=x;} void setESP(ULONG x){sp=(USHORT)x;}
LONG DpmiSetWowLdtEntry(ULONG a,ULONG b,ULONG c){(void)a;(void)b;(void)c;abort();}
void *Sim32pGetVDMPointer(ULONG address,ULONG mode){(void)address;(void)mode;mappings++;return memory;}
void mvdm_softpc_report_dpmi_swint(ULONG a,USHORT b,USHORT c,USHORT d,USHORT e,ULONG f)
{(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;}
void mvdm_softpc_report_dpmi_swint_target(ULONG a,USHORT b,ULONG c,USHORT d)
{(void)a;(void)b;(void)c;(void)d;targets++;}
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(void){
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==0 && targets==0 && flags==0x4321);
  descriptors[1].HighWord.Bits.Pres=1; DpmiInterruptHandlers[0x31].CsSelector=8;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==1 && dispatches==1);
  CHECK(sp==0xfa && *(USHORT *)(memory+0xfa)==0x30);
  msw=1; sp=2; descriptors[2].LimitLow=0xffff;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==2 && dispatches==1);
  puts("DPMI_SWINT_SELECTED_OBJECT_OK absent=1 v86=1 stack_failure=1");return 0;
}
