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
static BOOL mapping_unavailable,selector_resolves;
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
void *Sim32pGetVDMPointer(ULONG address,ULONG mode){(void)address;(void)mode;mappings++;return mapping_unavailable ? NULL : memory;}
BOOL selector_outside_GDT_LDT(USHORT selector,ULONG *address)
{*address=0x1000;return (selector & ~3u)==0 || !selector_resolves;}
void mvdm_softpc_report_dpmi_swint(ULONG a,USHORT b,USHORT c,USHORT d,USHORT e,ULONG f)
{(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;}
void mvdm_softpc_report_dpmi_swint_target(ULONG a,USHORT b,ULONG c,USHORT d)
{(void)a;(void)b;(void)c;(void)d;targets++;}
void mvdm_softpc_report_dpmi_iret16(USHORT bits,USHORT a,USHORT b,USHORT c,USHORT d,ULONG e,ULONG f)
{(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;if(bits==32)++iret_reports;}
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(void){
  unsigned mode, depth, order;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==0 && targets==0 && flags==0x4321);
  descriptors[1].HighWord.Bits.Pres=1; DpmiInterruptHandlers[0x31].CsSelector=15;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==1 && dispatches==1);
  CHECK(sp==0xfa && *(USHORT *)(memory+0xfa)==0x30);
  msw=1; sp=2; descriptors[2].LimitLow=0xffff;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==2 && dispatches==1);
  puts("DPMI_SWINT_SELECTED_OBJECT_OK absent=1 v86=1 stack_failure=1");
  /* NT4 PushInt selects frame width from the installed handler, independently
   * of the interrupted application's 16/32-bit flag. */
  descriptors[1].HighWord.Bits.Type=0x1a; /* present readable code, DPL 3 */
  descriptors[1].HighWord.Bits.Dpl=3;
  descriptors[2].HighWord.Bits.Type=0x12; /* writable stack data, DPL 3 */
  descriptors[2].HighWord.Bits.Dpl=3;
  descriptors[2].HighWord.Bits.Pres=1;
  for (order=0;order<8;++order) {
    mode=order ^ 2u;
    ULONG top=(mode & 4) ? 0x10100u : 0x100u;
    BOOL wide=(mode & 2) != 0;
    memset(memory,0,sizeof(memory));
    descriptors[2].LimitLow=0xffff;
    descriptors[2].HighWord.Bits.LimitHi=(mode & 4) ? 1 : 0;
    descriptors[2].HighWord.Bits.Default_Big=(mode & 4) != 0;
    descriptors[1].LimitLow=0xffff;
    descriptors[1].HighWord.Bits.LimitHi=1;
    descriptors[1].HighWord.Bits.Default_Big=1;
    ss=0x17;cs=0x27;sp=(mode & 4) ? top : (0xabcd0000u | top);
    ip=0x12345678;flags=0x202;msw=1;
    CurrentAppFlags=(USHORT)(mode & 1);
    DpmiInterruptHandlers[0x31].Flags=(USHORT)(wide ? 2 : 0);
    DpmiInterruptHandlers[0x31].Eip=0x14321;
    CHECK(DpmiSwIntHandler(0x31));
    if (wide) {
      CHECK(sp==top-12);
      CHECK(*(ULONG *)(memory+top-12)==0x12345678 &&
            *(ULONG *)(memory+top-8)==0x12340027 && *(ULONG *)(memory+top-4)==0x202 &&
            ip==0x14321);
    } else {
      CHECK(sp==top-6);
      CHECK(*(USHORT *)(memory+top-6)==0x5678 &&
            *(USHORT *)(memory+top-4)==0x27 && *(USHORT *)(memory+top-2)==0x202 &&
            ip==0x14321);
    }
    CHECK(cs==15);
  }
  memset(memory,0x5a,sizeof(memory));
  ss=0x17;cs=0x27;sp=8;ip=0x1234;flags=0x302;msw=1;
  descriptors[2].LimitLow=0xffff;descriptors[2].HighWord.Bits.LimitHi=0;
  descriptors[2].HighWord.Bits.Default_Big=0;
  CurrentAppFlags=0;DpmiInterruptHandlers[0x31].Flags=2;
  CHECK(!DpmiSwIntHandler(0x31));
  CHECK(sp==8 && cs==0x27 && ip==0x1234 && flags==0x302 && memory[0]==0x5a);
  flags=0x202;
  CurrentAppFlags=1;DpmiInterruptHandlers[0x31].Flags=0;
  CHECK(DpmiSwIntHandler(0x31));
  CHECK(sp==2 && *(USHORT *)(memory+2)==0x1234);
  puts("DPMI_SWINT_HANDLER_WIDTH_OK app=2 handler=2 stack=2");
  /* NT4 PushInt compares the new SP with the expanded SS limit. */
  ss=0x17;cs=0x27;sp=0x10b;ip=0x1234;flags=0x202;msw=1;
  descriptors[2].HighWord.Bits.Default_Big=1;
  descriptors[2].HighWord.Bits.Type=0x12;
  descriptors[2].HighWord.Bits.Granularity=0;
  descriptors[2].HighWord.Bits.LimitHi=0;descriptors[2].LimitLow=0x100;
  DpmiInterruptHandlers[0x31].Flags=2;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(sp==0xff);
  ss=0x17;cs=0x27;sp=0x10c;ip=0x1234;flags=0x202;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(sp==0x10c);
  descriptors[2].HighWord.Bits.Type=0x16; /* expand-down */
  ss=0x17;cs=0x27;sp=0x10c;ip=0x1234;flags=0x202;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(sp==0x10c);
  ss=0x17;cs=0x27;sp=0x10d;ip=0x1234;flags=0x202;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(sp==0x101);
  descriptors[2].HighWord.Bits.Type=0x12;
  descriptors[2].HighWord.Bits.Granularity=1;descriptors[2].LimitLow=0x10;
  ss=0x17;cs=0x27;sp=0x1100a;ip=0x1234;flags=0x202;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(sp==0x10ffe);
  ss=0x17;cs=0x27;sp=0x1100c;ip=0x1234;flags=0x202;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(sp==0x1100c);
  descriptors[2].HighWord.Bits.Granularity=0;
  puts("DPMI_SWINT_STACK_LIMIT_OK normal=2 expand=2 granular=2");
  /* A SIM32 selector mapping failure must not become a host access fault. */
  {
    BOOL handled=TRUE, host_fault=FALSE;
    descriptors[2].LimitLow=0xffff;
    ss=0x17;cs=0x27;sp=0x200;ip=0x1234;flags=0x302;
    mapping_unavailable=TRUE;
    __try { handled=DpmiSwIntHandler(0x31); }
    __except(EXCEPTION_EXECUTE_HANDLER) { host_fault=TRUE; }
    mapping_unavailable=FALSE;
    fprintf(stderr,"null_mapping host_fault=%d handled=%d sp=%lx cs=%x ip=%lx flags=%lx\n",
            host_fault,handled,sp,cs,ip,flags);
    CHECK(!host_fault && !handled && sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x302);
  }
  puts("DPMI_SWINT_NULL_MAPPING_OK fallback=1 state=unchanged");
  {
    BOOL host_fault=FALSE,handled=TRUE;
    descriptors[0].LimitLow=0xffff;
    ss=0;cs=0x27;sp=0x200;ip=0x1234;flags=0x302;
    mapping_unavailable=TRUE;selector_resolves=TRUE;
    __try { handled=DpmiSwIntHandler(0x31); }
    __except(EXCEPTION_EXECUTE_HANDLER) { host_fault=TRUE; }
    mapping_unavailable=FALSE;selector_resolves=FALSE;
    CHECK(!host_fault && !handled && sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x302);
  }
  /* A resolvable selector and NULL alias may instead be allocation failure;
   * retain the existing host-failure path instead of reporting a bad selector. */
  {
    BOOL host_fault=FALSE;
    ss=0x17;cs=0x27;sp=0x200;ip=0x1234;flags=0x302;
    mapping_unavailable=TRUE;selector_resolves=TRUE;
    __try { (void)DpmiSwIntHandler(0x31); }
    __except(EXCEPTION_EXECUTE_HANDLER) { host_fault=TRUE; }
    mapping_unavailable=FALSE;selector_resolves=FALSE;
    CHECK(host_fault);
  }
  puts("DPMI_SWINT_NONSELECTOR_NULL_OK prior_failure=preserved");
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
