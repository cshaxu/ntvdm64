/* Boundary unit test: compile the unchanged production function body with
 * observed CPU/descriptor dependencies. This is not a CCPU execution test. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MSW_PE 1u
#define EFLAGS_NT_MASK 0x4000u
#define LDT_SIZE 8192u
static BYTE frame[16];
static ULONG flags, msw, cs, ss, ds, es, fs, gs, eip, esp, cpl;
static ULONG Cpu40GdtShadowAddress, gdt, idt;
static BOOL valid_idt;
static unsigned writes, syncs, reentries, bad_order;
#define getSS() ss
#define getSP() ((USHORT)esp)
#define getMSW() msw
#define getEFLAGS() flags
#define getCS() cs
#define getIP() ((USHORT)eip)
#define getCR0() msw
#define setEFLAGS(v) (writes++, flags=(v))
static void setMSW(ULONG value) {
    if (flags & EFLAGS_NT_MASK) ++bad_order;
    ++writes; msw=value;
}
#define setCPL(v) (writes++, cpl=(v))
#define setCS(v) (writes++, cs=(v))
#define setSS(v) (writes++, ss=(v))
#define setDS(v) (writes++, ds=(v))
#define setES(v) (writes++, es=(v))
#define setFS(v) (writes++, fs=(v))
#define setGS(v) (writes++, gs=(v))
#define setEIP(v) (writes++, eip=(v))
#define setESP(v) (writes++, esp=(v))
static PCHAR Sim32GetVDMPointer(ULONG address, ULONG size, UCHAR pm) {
    if (address != 0x10000020u || size != 0 || pm != 0) ++bad_order;
    return (PCHAR)frame;
}
static void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit) {
    if (msw & MSW_PE || limit != 65535) ++bad_order;
    gdt=base;
}
static BOOL DpmiCpu40GetDosxIdtDescriptor(PULONG base, PUSHORT limit) {
    *base=0x22000; *limit=0x7ff; return valid_idt;
}
static void c_setIDT_BASE_LIMIT(ULONG base, USHORT limit) {
    if (msw & MSW_PE || limit != 0x7ff) ++bad_order;
    idt=base;
}
static void mvdm_softpc_fast_bop_sync_mode(void) {
    if (!(msw & MSW_PE)) ++bad_order;
    ++syncs;
}
static BOOL mvdm_softpc_wow_page_domain_reenter_protected(void) {
    if (!(msw & MSW_PE) || cpl != 3 || cs != 0xcf || ss != 0x17f)
        ++bad_order;
    ++reentries; return TRUE;
}
#define c_getCR3() 0u
#define c_getDS_BASE() 0u
#define c_getDS_LIMIT() 0xffffu
#define mvdm_softpc_report_dpmi_mode_state(...) ((void)0)
#include "pm_entry_body.inc"

int main(void) {
    ULONG input;
    unsigned test, failed=0;
    USHORT ds0=0x167, ss0=0x17f, cs0=0xcf;
    ULONG ip0=0x1101, sp0=0xff0;
    memcpy(frame,&ds0,2); memcpy(frame+2,&sp0,4);
    memcpy(frame+6,&ss0,2); memcpy(frame+8,&ip0,4);
    memcpy(frame+12,&cs0,2);
    /* Exhaust every low FLAGS combination, including NT absent/present,
     * both IF states, every IOPL and arithmetic/direction flags. */
    for(test=0;test<65536+2;test++) {
        input=test<65536 ? test : 0x7297u;
        flags=input; msw=0x10; ss=0x1000; esp=0x20;
        cs=ds=es=fs=gs=0x1000; cpl=0; eip=0;
        writes=syncs=reentries=bad_order=0; gdt=idt=0;
        valid_idt=test!=65536;
        Cpu40GdtShadowAddress=test==65537 ? 0 : 0x11000;
        DpmiCpu40SwitchToProtectedMode();
        if (!valid_idt) {
            failed += flags!=input || msw!=0x10 || writes || syncs || reentries;
        } else {
            failed += flags!=(input & ~EFLAGS_NT_MASK) || msw!=0x11 ||
                cs!=cs0 || ss!=ss0 || ds!=ds0 || es || fs || gs ||
                eip!=ip0 || esp!=sp0 || cpl!=3 || syncs!=1 || reentries!=1 ||
                idt!=0x22000 || gdt!=Cpu40GdtShadowAddress;
        }
        failed += bad_order!=0;
    }
    printf("PM_ENTRY_CONTRACT cases=%u failed=%u (mock-boundary, not CPU execution)\n",test,failed);
    return failed!=0;
}
