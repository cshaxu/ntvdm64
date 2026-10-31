#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#pragma pack(push,1)
typedef struct { USHORT CsSelector, Flags; ULONG Eip; } HANDLER;
#pragma pack(pop)
extern HANDLER DpmiInterruptHandlers[256];
typedef struct { USHORT CsSelector, SsSelector; ULONG Eip, Esp, Flags; } FAULT_HANDLER;
extern FAULT_HANDLER DpmiFaultHandlers[32];
extern BOOL __cdecl DpmiSwIntHandler(ULONG);
extern BOOL __cdecl DpmiV86SoftwareInt(ULONG);
extern BOOL __cdecl DpmiFaultHandler(ULONG, ULONG);
extern void __cdecl DpmiFaultHandlerIret32(void);
extern void __cdecl DpmiIntHandlerIret32(void);
static LDT_ENTRY descriptors[4]; LDT_ENTRY *Ldt=descriptors;
static BYTE memory[131072]; ULONG IntelBase=(ULONG)(ULONG_PTR)memory;
static USHORT ss=16,cs=0x20;
static ULONG sp=0x100,ip=0x30;
static ULONG flags=0x4321,msw,mock_cr0,mappings,targets,dispatches,iret_reports;
static ULONG mock_eax,mock_ebp,ldt_publications,mock_cf;
static BOOL mapping_unavailable,selector_resolves;
USHORT CurrentAppFlags, DosxRmCodeSelector, LockedPMStackSel;
ULONG LockedPMStackCount;
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
UNREACHED(setSTATUS) UNREACHED(setCR0)
UNREACHED(VdmInstallHardwareIntHandler) UNREACHED(VdmInstallSoftwareIntHandler)
UNREACHED(VdmInstallFaultHandler)
UNREACHED(mvdm_softpc_report_dpmi_iret16_state)
static ULONG fault_dispatch_reports, fault_stack_reports;
void mvdm_softpc_report_dpmi_fault_dispatch(ULONG number, ULONG error,
    USHORT handler_cs, ULONG handler_ip, USHORT active_cs, ULONG active_ip,
    USHORT active_ss, ULONG active_sp, ULONG frame32, USHORT source_cs, ULONG source_ip)
{
    (void)number;(void)error;(void)handler_cs;(void)handler_ip;(void)active_cs;
    (void)active_ip;(void)active_ss;(void)active_sp;(void)frame32;(void)source_cs;(void)source_ip;
    ++fault_dispatch_reports;
}
void mvdm_softpc_report_dpmi_fault_stack(const char *stage, USHORT saved_ss,
    ULONG saved_sp, USHORT active_ss, ULONG active_sp, const USHORT *words, ULONG count)
{
    (void)stage;(void)saved_ss;(void)saved_sp;(void)active_ss;(void)active_sp;(void)words;(void)count;
    ++fault_stack_reports;
}
USHORT getAX(void){return 1;} USHORT getBX(void){return 2;}
USHORT getCX(void){return 3;} USHORT getCS(void){return cs;}
USHORT getSS(void){return ss;} USHORT getSP(void){return (USHORT)sp;}
USHORT getIP(void){return (USHORT)ip;} ULONG getEIP(void){return ip;}
ULONG getEAX(void){return mock_eax;} ULONG getEBP(void){return mock_ebp;}
ULONG getEBX(void){return 0;} ULONG getECX(void){return 0;}
ULONG getEDX(void){return 0;} ULONG getESP(void){return sp;}
ULONG getEFLAGS(void){return flags;} ULONG getMSW(void){return msw;}
ULONG getCR0(void){return mock_cr0;}
void setEFLAGS(ULONG x){flags=x;} void setEAX(ULONG x){mock_eax=x;}
void setCF(ULONG x){mock_cf=x;} void setCS(USHORT x){cs=x;dispatches++;}
void setIP(USHORT x){ip=x;} void setEIP(ULONG x){ip=x;}
void setSP(USHORT x){sp=(sp & 0xffff0000u)|x;} void setESP(ULONG x){sp=x;}
void setSS(USHORT x){ss=x;}
LONG DpmiSetWowLdtEntry(ULONG a,ULONG b,ULONG c){(void)a;(void)b;(void)c;++ldt_publications;return 0;}
static ULONG v86_reads, v86_writes;
ULONG spr_read_dword(ULONG address){++v86_reads; return *(ULONG *)(memory+address);}
void spr_write_word(ULONG address,USHORT value){++v86_writes; *(USHORT *)(memory+address)=value;}
void host_cpu40_set_virtual_if(BOOL enabled)
{if(enabled) *(ULONG *)(memory+0x714)|=0x200; else *(ULONG *)(memory+0x714)&=~0x200u;}
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
int main(int argc, char **argv){
  unsigned mode, depth, order;
  ULONG before_fault_maps, before_fault_dispatches;
  if(argc==2 && !strcmp(argv[1],"--wow-signature-mode")) {
    msw=1; flags=0x23302; mock_eax=mock_ebp=0xf0f0f0f1u;
    CHECK(!DpmiSwIntHandler(0x2a));
    CHECK(ldt_publications==0 && mock_eax==0xf0f0f0f1u && flags==0x23302);
    msw=0; flags=0x3302;
    CHECK(!DpmiSwIntHandler(0x2a));
    CHECK(ldt_publications==0 && mock_eax==0xf0f0f0f1u && flags==0x3302);
    msw=1;
    flags=0x3302; mock_cf=1;
    CHECK(DpmiSwIntHandler(0x2a));
    CHECK(ldt_publications==1 && mock_eax==0 && mock_cf==0 && flags==0x3302);
    puts("DPMI_INT2A_MODE_OWNER_OK v86=declined real=declined protected=handled");
    return 0;
  }
  if(argc==2 && (!strcmp(argv[1],"--pm-fault-failure") ||
                 !strcmp(argv[1],"--pm-fault-absent") ||
                 !strcmp(argv[1],"--pm-fault-first") ||
                 !strcmp(argv[1],"--pm-fault-nested"))) {
    BOOL all=!strcmp(argv[1],"--pm-fault-failure");
    msw=1; flags=0x3302; sp=0x200; ss=0x27; cs=0x37; ip=0x1234;
    DpmiFaultHandlers[14].CsSelector=0;
    if(all || !strcmp(argv[1],"--pm-fault-absent")) {
      CHECK(!DpmiFaultHandler(14,2));
      CHECK(flags==0x3302 && sp==0x200 && ss==0x27 && cs==0x37 && ip==0x1234);
    }
    descriptors[1].HighWord.Bits.Pres=1; DpmiFaultHandlers[14].CsSelector=0x0f;
    for(depth=0;depth<2;++depth) {
      if(!all && strcmp(argv[1],depth ? "--pm-fault-nested" : "--pm-fault-first")) continue;
      flags=0x3302; sp=depth ? 0x21 : 0x200; ss=depth ? 0x17 : 0x27;
      cs=0x37; ip=0x1234; LockedPMStackSel=0x17; LockedPMStackCount=depth;
      descriptors[2].LimitLow=depth ? 0xffff : 1;
      CHECK(!DpmiFaultHandler(14,2));
      printf("DPMI_PM_FAULT_REJECT depth=%u flags=%lx ss=%x sp=%lx count=%lu\n",
             depth,flags,ss,sp,LockedPMStackCount);
      CHECK(flags==0x3302 && sp==(depth ? 0x21u : 0x200u) &&
            ss==(depth ? 0x17 : 0x27) && cs==0x37 && ip==0x1234 && LockedPMStackCount==depth);
    }
    if(all) puts("DPMI_PM_FAULT_FAILURE_ROLLBACK_OK absent=1 first=1 nested=1");
    else printf("DPMI_PM_FAULT_CASE_OK %s\n",argv[1]);
    return 0;
  }
  if(argc==2 && strcmp(argv[1],"--pm-fault-vector14")==0) {
    msw=1; flags=0x3302; sp=0x200; ss=0x27; cs=0x37; ip=0x1234;
    descriptors[1].HighWord.Bits.Pres=1; descriptors[2].LimitLow=0xffff;
    LockedPMStackSel=0x17; LockedPMStackCount=0; CurrentAppFlags=0;
    DpmiFaultHandlers[14].CsSelector=0x0f; DpmiFaultHandlers[14].Eip=0x555;
    CHECK(DpmiFaultHandler(14, 2));
    CHECK(flags==0x3002 && ss==0x17 && sp==0xfd0 && cs==0x0f && ip==0x555);
    CHECK(*(USHORT *)(memory+0xfd4)==2 && *(USHORT *)(memory+0xfd6)==0x1234 &&
          *(USHORT *)(memory+0xfd8)==0x37 && *(USHORT *)(memory+0xfda)==0x3302 &&
          *(USHORT *)(memory+0xfdc)==0x200 && *(USHORT *)(memory+0xfde)==0x27);
    CHECK(fault_dispatch_reports==1 && fault_stack_reports==1);
    puts("DPMI_PM_VECTOR14_FRAME_OK");
    return 0;
  }
  if(argc==2 && strcmp(argv[1],"--v86-nt4-vector7")==0) {
    /* NT4 PushException: V86 exceptions 0–7 use the real-mode IVT and
     * 16-bit stack, even without a protected-mode selector. */
    for(mode=0;mode<8;++mode) for(order=0;order<2;++order) {
      msw=1; mock_cr0=0; flags=0x23302; ss=0; sp=0x200; cs=0x27; ip=0x1234;
      *(ULONG *)(memory+0x714)=0xa203;
      DpmiFaultHandlers[mode].Flags=order;
      *(USHORT *)(memory+mode*4)=0x456; *(USHORT *)(memory+mode*4+2)=0x300;
      CHECK(DpmiFaultHandler(mode, 0));
      CHECK(sp==0x1fa && cs==0x300 && ip==0x456);
      CHECK(*(USHORT *)(memory+0x1fa)==0x1234 &&
            *(USHORT *)(memory+0x1fc)==0x27 && *(USHORT *)(memory+0x1fe)==0x3302);
      CHECK(flags==(order ? 0x23002u : 0x23202u));
      CHECK(*(ULONG *)(memory+0x714)==(order ? 0xa003u : 0xa203u));
    }
    /* Paged and non-direct-IF cases remain on the existing path: do not
     * mistake that path for proof of NT4 reflection semantics. */
    DpmiFaultHandlers[7].Flags=0; DpmiFaultHandlers[7].CsSelector=0;
    before_fault_maps=mappings; mock_cr0=0x80000000u; flags=0x23202;
    sp=0x200; cs=0x27; ip=0x1234; *(ULONG *)(memory+0x714)=0xa200;
    CHECK(!DpmiFaultHandler(7, 0));
    CHECK(mappings==before_fault_maps && *(ULONG *)(memory+0x714)==0xa200);
    mock_cr0=0; flags=0x22202;
    CHECK(!DpmiFaultHandler(7, 0));
    CHECK(mappings==before_fault_maps && *(ULONG *)(memory+0x714)==0xa200);
    puts("DPMI_V86_LOW_FAULT_REFLECTION_OK vectors=8 gates=2");
    return 0;
  }
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==0 && targets==0 && flags==0x4321);
  descriptors[1].HighWord.Bits.Pres=1; DpmiInterruptHandlers[0x31].CsSelector=15;
  CHECK(DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==1 && dispatches==1);
  CHECK(sp==0xfa && *(USHORT *)(memory+0xfa)==0x30);
  msw=1; sp=2; descriptors[2].LimitLow=0xffff;
  CHECK(!DpmiSwIntHandler(0x31)); CHECK(mappings==1 && targets==2 && dispatches==1);
  puts("DPMI_SWINT_SELECTED_OBJECT_OK absent=1 v86=1 stack_failure=1");
  flags=0x27302; ss=0; sp=0x200; cs=0x27; ip=0x1234;
  *(USHORT *)(memory+0x31*4)=0x456; *(USHORT *)(memory+0x31*4+2)=0x300;
  DpmiInterruptHandlers[0x31].Flags=0;
  CHECK(DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x1fa && cs==0x300 && ip==0x456 && flags==0x23002);
  CHECK(*(USHORT *)(memory+0x1fa)==0x1234 &&
        *(USHORT *)(memory+0x1fc)==0x27 && *(USHORT *)(memory+0x1fe)==0x7302);
  CHECK(v86_reads==1 && v86_writes==3);
  flags=0x23202; sp=0x200; cs=0x27; ip=0x1234;
  DpmiInterruptHandlers[0x31].Flags=4;
  CHECK(!DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x23202);
  DpmiInterruptHandlers[0x31].Flags=0; flags=0x20202;
  CHECK(!DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x20202);
  flags=0x21202; CHECK(!DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x21202);
  flags=0x22202; CHECK(!DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x22202);
  flags=0x3202; CHECK(!DpmiV86SoftwareInt(0x31));
  CHECK(sp==0x200 && cs==0x27 && ip==0x1234 && flags==0x3202);
  CHECK(v86_reads==1 && v86_writes==3);
  puts("DPMI_V86_INT_OWNER_OK ivt=1 hooked=untouched virtual_if=untouched pm=untouched");
  /* NT4 PushException refuses V86 vectors above 7 before touching state,
   * regardless of whether a protected-mode fault vector is installed. */
  before_fault_maps=mappings; before_fault_dispatches=dispatches;
  msw=1; flags=0x23302; sp=0x200; cs=0x27; ip=0x1234;
  CHECK(!DpmiFaultHandler(14, 2));
  CHECK(flags==0x23302 && sp==0x200 && cs==0x27 && ip==0x1234);
  CHECK(mappings==before_fault_maps && dispatches==before_fault_dispatches);
  CHECK(!DpmiFaultHandler(8, 0));
  CHECK(flags==0x23302 && sp==0x200 && cs==0x27 && ip==0x1234);
  CHECK(!DpmiFaultHandler(31, 0));
  CHECK(flags==0x23302 && sp==0x200 && cs==0x27 && ip==0x1234);
  descriptors[1].HighWord.Bits.Pres=1; DpmiFaultHandlers[14].CsSelector=0x0f;
  CHECK(!DpmiFaultHandler(14, 2));
  CHECK(flags==0x23302 && sp==0x200 && cs==0x27 && ip==0x1234);
  CHECK(mappings==before_fault_maps && dispatches==before_fault_dispatches);
  DpmiFaultHandlers[14].CsSelector=0;
  flags=0x3302; CHECK(!DpmiFaultHandler(14, 2));
  CHECK(flags==0x3302 && sp==0x200 && cs==0x27 && ip==0x1234);
  puts("DPMI_V86_HIGH_FAULT_DECLINED_OK installed=2 protected=unchanged");
  msw=0;
  flags=0x4321; ss=16;
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
