/*[
 *
 *  Name:	    nt_inthk.c
 *
 *  Derived From:   (original)
 *
 *  Author:	    Dave Bartlett
 *
 *  Created On:     11 Jan 1995
 *
 *  Coding Stds:    2.4
 *
 *  Purpose:        This module implements the memory management functions
 *                  required for 486 NT.
 *
 *  Include File:   nt_inthk.h
 *
 *  Copyright Insignia Solutions Ltd., 1994. All rights reserved.
 *
]*/


#ifdef CPU_40_STYLE

/* Need all of the following to include nt.h and windows.h in the same file. */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"
#include CpuH

#include "gdpvar.h"
#include "nt_inthk.h"
#include "debug.h"
#include "ios.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_effective_address.h"
#ifdef CCPU
#include <c_xcptn.h>
#include <c_reg.h>
#endif

/* Make local symbols visible if debugging. */
#ifndef PROD
#define LOCAL
#endif /* not PROD */

/* Macros and typedefs. */


/* Hardware interrupt handler */
LOCAL BOOL (*HWIntHandler)(ULONG) = NULL;

#if defined(CCPU) || !defined(PROD)
/* Software interrupt handler */
LOCAL BOOL (*SWIntHandler)(ULONG) = NULL;

/* Exception interrupt handler */
LOCAL BOOL (*EXIntHandler)(ULONG,ULONG) = NULL;

#endif /* CCPU */

/* Global Functions. */

/*(
============================= host_hwint_hook =================================
PURPOSE:
	This function is called from the ICA during the process of ACKing an
	hardware interrupt. This function will call a hardware interrupt handler
	if one is defined.

INPUT:
	Interrupt vector number generated from hardware interrupt
OUTPUT:
	Return value - TRUE    hardware interrupt processed
		       FALSE   process hardware interrupt in normal way
================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
GLOBAL BOOL host_hwint_hook IFN1(IS32, int_no)
{
    BOOL returnStatus = FALSE;

    /* hardware interrupt handler defined ? */
    if(HWIntHandler)
    {
	returnStatus = (HWIntHandler)((ULONG) int_no);

#ifndef PROD
	if(!returnStatus)
	    always_trace0("Hardware interrupt handler failed");

#endif /* PROD */
    }

    return( returnStatus );
}
#endif

/*(
======================= VdmInstallHardwareIntHandler ===========================
PURPOSE:
	Register a hardware interrupt handler called before the CPU dispatches
	the interrupt.

INPUT:
	Hardware interrupt handler function

OUTPUT:
	Return value - NTSTATUS

================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallHardwareIntHandler IFN1(PVOID, HardwareIntHandler)
{
#ifdef CCPU
    HWIntHandler = HardwareIntHandler;
#else
    GLOBAL_VDM_HwIntHandler = HardwareIntHandler;
#endif
    return(STATUS_SUCCESS);
}

/*(
============================= host_swint_hook =================================
PURPOSE:
	This function is called from the CCPU prior to provessing a software
	interrupt. This function will call a software interrupt handler
	if one is defined.

INPUT:
	Interrupt number
OUTPUT:
	Return value - TRUE    software interrupt processed
		       FALSE   process software interrupt in normal way

================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
#ifdef CCPU
/* A decoded V86 INT is distinct from x87 exceptions that also call INTx. */
GLOBAL BOOL host_v86_software_int IFN2(IS32, int_no, BOOL, prefixed)
{
    extern BOOL DpmiV86SoftwareInt(ULONG, BOOL);
    extern BOOL DpmiSwIntHandler(ULONG);
    return SWIntHandler == DpmiSwIntHandler && DpmiV86SoftwareInt((ULONG)int_no, prefixed);
}
GLOBAL BOOL host_v86_software_int_at IFN3(IS32, int_no, BOOL, prefixed, IU16, return_ip)
{
    extern BOOL DpmiV86SoftwareIntAt(ULONG, BOOL, USHORT);
    extern BOOL DpmiSwIntHandler(ULONG);
    return SWIntHandler == DpmiSwIntHandler &&
        DpmiV86SoftwareIntAt((ULONG)int_no, prefixed, return_ip);
}
#endif
GLOBAL BOOL host_swint_hook IFN1(IS32, int_no)
{
    BOOL returnStatus = FALSE;

    /* DIVERGENCE(MVDM-HOST-DIV-307): observation only for the original
     * WOW_x86 INT 2Ah fast LDT publication protocol.  NT4's kernel trap
     * recognizes EAX==EBP==F0F0F0F1 and calls NtSetLdtEntries with EBX,
     * ECX and EDX.  CPU40 deliberately retains the normal software-
     * interrupt dispatch here; this witness records the existing protocol
     * before any standalone carrier is selected. */
    if (int_no == 0x2a && getEAX() == 0xf0f0f0f1UL &&
            getEBP() == 0xf0f0f0f1UL) {
        mvdm_softpc_report_wow_int2a_ldt((unsigned short)getEBX(),
            (unsigned long)getECX(), (unsigned long)getEDX());
    }
    if (int_no == 0x2a) {
        /* DIVERGENCE(MVDM-HOST-DIV-309): default-off scalar witness for the
         * original WOW_x86 kernel-LDT carrier.  It precedes no decision and
         * cannot alter the existing software-interrupt dispatch. */
        mvdm_softpc_report_wow_int2a((unsigned long)getEAX(),
            (unsigned long)getEBX(), (unsigned long)getECX(),
            (unsigned long)getEDX(), (unsigned long)getEBP());
    }

    /* software interrupt handler defined ? */
    if(SWIntHandler)
    {
	returnStatus = (SWIntHandler)((ULONG) int_no);

#ifndef PROD

	if(!returnStatus)
	    always_trace0("Software interrupt handler failed");

#endif /* PROD */
    }

    return( returnStatus );
}
#ifdef CCPU
/* An installed exception owner retains the original decline/fallback
 * contract; only absence permits the separate V86 software reflector. */
GLOBAL BOOL host_v86_trap IFN2(IS32, int_no, BOOL, prefixed)
{
    BOOL handlerInstalled = EXIntHandler != NULL;
    BOOL handled = host_exint_hook(int_no, 0);
    return handled || (!handlerInstalled && host_v86_software_int(int_no, prefixed));
}
#endif
#endif /* CCPU */

/*(
======================= VdmInstallSoftwareIntHandler ===========================
PURPOSE:
	Register a software interrupt handler called before the CPU dispatches
	the software interrupt.

INPUT:
	Software interrupt handler function

OUTPUT:
	Return value - NTSTATUS
================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallSoftwareIntHandler IFN1(PVOID, SoftwareIntHandler)
{
#ifdef CCPU
    SWIntHandler = SoftwareIntHandler;
#else
    GLOBAL_VDM_SoftIntHandler = SoftwareIntHandler;
#endif
    return(STATUS_SUCCESS);
}

/*(
============================= host_exint_hook =================================
PURPOSE:
	This function is called from the CPU prior to processing a CPU
	exception interrupt. This function will call a exception interrupt
	handler if one is defined.

INPUT:
	Exception number
	Exception error code
OUTPUT:
	Return value - TRUE    hardware interrupt processed
		       FALSE   process hardware interrupt in normal way

================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
#ifdef CCPU
/* The NT VDM fixed-state IF bit is shared with ICA and guest code. */
GLOBAL void host_cpu40_set_virtual_if IFN1(BOOL, enabled)
{
    extern unsigned char *c_GetPhyAdd(unsigned long);
    extern void host_ica_lock(void), host_ica_unlock(void);
    volatile LONG *state=(volatile LONG *)c_GetPhyAdd(0x714);
    host_ica_lock();
    if(enabled) InterlockedOr(state,0x200);
    else InterlockedAnd(state,~0x200);
    host_ica_unlock();
}
/* Non-faulting fetch for the CPU40 monitor path. The CPU's cached decision
 * remains authoritative; a cache miss uses its page-protection rules. */
static BOOL monitor_read_ram(ULONG physical, void *value, ULONG size)
{
    extern unsigned long c_sas_memory_size(void);
    extern int c_sas_memory_type(unsigned long);
    extern unsigned char *c_GetPhyAdd(unsigned long);
    extern unsigned long SasWrapMask;
    ULONG available=c_sas_memory_size();
    int first, last;
    if (!size) return FALSE;
    physical &= SasWrapMask;
    if (size-1 > SasWrapMask-physical) return FALSE;
    if (physical>=available || size>available-physical) return FALSE;
    first=c_sas_memory_type(physical);last=c_sas_memory_type(physical+size-1);
    if ((first!=0 && first!=2) || (last!=0 && last!=2)) return FALSE;
    CopyMemory(value,c_GetPhyAdd(physical),size);
    return TRUE;
}

static BOOL monitor_translate(ULONG linear, BOOL write, ULONG *physical)
{
    extern unsigned long c_getCR0(void), c_getCR3(void);
    extern int xtrn2phy(unsigned long, unsigned char, unsigned long *);
    extern int ccpu_monitor_cached_access(unsigned long, unsigned char, unsigned long *);
    UCHAR access=(getCPL()==3 ? 2 : 0) | (write ? 1 : 0);
    *physical=linear;
    if (c_getCR0() & 0x80000000UL) {
        ULONG pde,pte,dir=(c_getCR3() & 0xfffff000UL)+((linear>>22)<<2);
        ULONG table;
        int cached=ccpu_monitor_cached_access(linear,access,physical);
        if(cached<0) return FALSE;
        if(cached>0) return TRUE;
        if (!monitor_read_ram(dir,&pde,sizeof(pde)) || !(pde&1)) return FALSE;
        table=(pde&0xfffff000UL)+(((linear>>12)&0x3ffUL)<<2);
        if (!monitor_read_ram(table,&pte,sizeof(pte)) || !(pte&1)) return FALSE;
        if (!xtrn2phy(linear,access,physical)) return FALSE;
    }
    return TRUE;
}

static BOOL monitor_mark_access(ULONG linear, BOOL write, ULONG expected)
{
    extern unsigned long c_getCR0(void);
    extern unsigned long SasWrapMask;
    extern int xtrn2phy(unsigned long, unsigned char, unsigned long *);
    ULONG physical;
    UCHAR access=(getCPL()==3 ? 2 : 0) | (write ? 1 : 0) | 4;
    if (!(c_getCR0() & 0x80000000UL)) return (linear & SasWrapMask)==expected;
    return xtrn2phy(linear,access,&physical) && (physical & SasWrapMask)==expected;
}

typedef struct { ULONG ip, base, limit; } MONITOR_CODE;

static BOOL monitor_segment(USHORT selector, ULONG *base, ULONG *limit, ULONG *access)
{
    BOOL was_contributory=doing_contributory, resolved;
    /* A nested CPU fault can leave through c_cpu_continue without returning. */
    doing_contributory=FALSE;
    resolved=mvdm_softpc_protected_segment(selector,base,limit,access);
    doing_contributory=was_contributory;
    return resolved;
}

static BOOL monitor_fetch_byte(const MONITOR_CODE *code, ULONG displacement, UCHAR *value)
{
    extern unsigned long SasWrapMask;
    ULONG offset=code->ip+displacement, linear, physical;
    if (offset>code->limit) return FALSE;
    /* NT4 forms the guest linear address with 32-bit addition. */
    linear=code->base+offset;
    if (!monitor_translate(linear,FALSE,&physical)) return FALSE;
    if (!monitor_read_ram(physical,value,1)) return FALSE;
    /* The original host read participates in paging accessed accounting. */
    return monitor_mark_access(linear,FALSE,physical & SasWrapMask);
}

/* KiVdmSetUserCR0 accepts only the guest's MP/EM bits. CPU40 owns the
 * virtual CR0; do not touch PE, paging, TS or host processor state. */
static void monitor_set_npx_control(ULONG value)
{
    extern unsigned long c_getCR0(void);
    extern void c_setCR0(unsigned long);
    c_setCR0((c_getCR0() & ~6UL) | (value & 6UL));
}

/* Original VdmDecodeOperand's 16-bit memory-address forms for LMSW. */
static BOOL monitor_lmsw_operand(const MONITOR_CODE *code, ULONG *cursor, UCHAR modrm, ULONG *value)
{
    extern unsigned long SasWrapMask;
    ULONG offset=0, base, limit, access, linear, physical[4], i;
    UCHAR lo,hi,bytes[4],mode=modrm>>6,rm=modrm&7;
    BOOL stack=FALSE;
    switch(rm) {
        case 0: offset=getBX()+getSI();break;
        case 1: offset=getBX()+getDI();break;
        case 2: offset=getBP()+getSI();stack=TRUE;break;
        case 3: offset=getBP()+getDI();stack=TRUE;break;
        case 4: offset=getSI();break;
        case 5: offset=getDI();break;
        case 6: if(mode) {offset=getBP();stack=TRUE;} break;
        default: offset=getBX();break;
    }
    if(mode==1) {
            if(!monitor_fetch_byte(code,(*cursor)++,&lo)) return FALSE;
            offset+=(signed char)lo;
    } else if(mode==2 || (mode==0 && rm==6)) {
            if(!monitor_fetch_byte(code,(*cursor)++,&lo) ||
               !monitor_fetch_byte(code,(*cursor)++,&hi)) return FALSE;
            offset+=(ULONG)lo | ((ULONG)hi<<8);
    }
    /* VdmDecodeOperand accumulates zero-extended 16-bit registers and
     * signed displacement in EBX, without truncating or checking the
     * default selector's segment limit before adding its base. */
    if(!monitor_segment(stack ? getSS() : getDS(),&base,&limit,&access)) return FALSE;
    linear=base+offset;
    /* VdmOpcodeLmsw reads a dword before KiVdmSetUserCR0 masks it. */
    if(linear>0xfffffffcUL) return FALSE;
    for(i=0;i<4;i++) if(!monitor_translate(linear+i,FALSE,&physical[i]) ||
        !monitor_read_ram(physical[i],&bytes[i],1)) return FALSE;
    for(i=0;i<4;i++) if(!monitor_mark_access(linear+i,FALSE,physical[i]&SasWrapMask)) return FALSE;
    *value=(ULONG)bytes[0] | ((ULONG)bytes[1]<<8);
    return TRUE;
}

/* NT4 EventVdmStringIo's 16-bit count/index contract.
 * Decline unsupported mappings before any I/O. */
static BOOL monitor_string_io(UCHAR op)
{
    extern unsigned long c_sas_memory_size(void);
    extern int c_sas_memory_type(unsigned long);
    extern unsigned char *c_GetPhyAdd(unsigned long);
    extern unsigned long SasWrapMask;
    extern void insb(io_addr,half_word *,word), insw(io_addr,word *,word);
    extern void outsb(io_addr,half_word *,word), outsw(io_addr,word *,word);
    BOOL input=(op==0x6c || op==0x6d), direct;
    ULONG width=(op==0x6d || op==0x6f) ? 2 : 1;
    ULONG count=1, index=input ? getDI() : getSI();
    ULONG bytes=count*width, address, base, limit, access, i, physical;
    ULONG available=c_sas_memory_size();
    ULONG mapping[2];
    union { USHORT aligned; UCHAR bytes[2]; } storage;
    UCHAR *buffer=storage.bytes;
    /* SIM32 uses the current selector-table descriptor, not the CPU cache. */
    if(!monitor_segment(input ? getES() : getDS(),&base,&limit,&access)) return FALSE;
    address=base+index;
    if (bytes && address+bytes-1<address) return FALSE;
    for(i=0;i<bytes;i++) {
        int type;
        if(!monitor_translate(address+i,input,&physical)) goto string_unhandled;
        physical &= SasWrapMask;
        if(physical>=available) goto string_unhandled;
        type=c_sas_memory_type(physical);
        if (input ? type!=0 : (type!=0 && type!=2)) goto string_unhandled;
        mapping[i]=physical;
        /* The original IOS routine receives the existing guest buffer. */
        buffer[i]=*c_GetPhyAdd(physical);
    }
    for(i=0;i<bytes;i+=0x1000UL-((address+i)&0xfffUL)) {
        if(!monitor_mark_access(address+i,input,mapping[i])) goto string_unhandled;
    }
    /* The original monitor passes its guest pointer to the device.  Use
     * that same live storage when the validated bytes are contiguous. */
    direct=(bytes==1 || c_GetPhyAdd(mapping[1])==c_GetPhyAdd(mapping[0])+1);
    if(direct) buffer=c_GetPhyAdd(mapping[0]);
    /* Reuse the original monitor IOS entries and their adapter routing. */
    if(input && width==1) insb(getDX(),(half_word *)buffer,(word)count);
    else if(input) insw(getDX(),(word *)buffer,(word)count);
    else if(width==1) outsb(getDX(),(half_word *)buffer,(word)count);
    else outsw(getDX(),(word *)buffer,(word)count);
    if(input && !direct)for(i=0;i<bytes;i++)*c_GetPhyAdd(mapping[i])=buffer[i];
    index=(USHORT)(getDF() ? index-bytes : index+bytes);
    if(input)setDI(index);else setSI(index);
    return TRUE;
string_unhandled:
    return FALSE;
}
#endif
GLOBAL BOOL host_exint_hook IFN2(IS32, exp_no, IS32, error_code)
{
    BOOL returnStatus = FALSE;
    BOOL handlerInstalled = EXIntHandler != NULL;
#ifdef CCPU
    /* DIVERGENCE(MVDM-HOST-DIV-473): provisional CPU40 host monitor dispatch. Further instruction, paging and lifecycle coverage is required before release. */
    /* DIVERGENCE(MVDM-HOST-DIV-480): NT4's protected-VDM Kt0d110 attempts
     * opcode dispatch for #GP without filtering its error code; unhandled
     * operations retain the original handler and its complete code below. */
    if (exp_no == 13 && getCPL() != 0 && !getVM()) {
        extern unsigned long c_getEIP(void);
        extern void IN8(unsigned long *,unsigned long), IN16(unsigned long *,unsigned long);
        extern void OUT8(unsigned long,unsigned long), OUT16(unsigned long,unsigned long);
        extern void ccpu_monitor_dispatch_irq(void);
        unsigned long ip=c_getEIP(), cs_access;
        MONITOR_CODE code;
        unsigned char op;
        code.ip=ip;
        if(!monitor_segment(getCS(),&code.base,&code.limit,&cs_access) ||
           (cs_access&0x98)!=0x98) goto candidate_unhandled;
        if (monitor_fetch_byte(&code,0,&op)) {
            unsigned long prefix=0;
            while (op==0x26 || op==0x2e || op==0x36 || op==0x3e ||
                   op==0x64 || op==0x65 || op==0x66 || op==0x67 || op==0xf0 || op==0xf2 || op==0xf3) {
                ++prefix;
                if(prefix>=128 || !monitor_fetch_byte(&code,prefix,&op))
                    goto candidate_unhandled;
            }
            if(op==0x0f) {
                unsigned char second, modrm;
                /* NT4 Opcode0F initializes RiEip from the original trap
                 * EIP and advances it once, so a leading prefix is not a
                 * supported 0F dispatch. Preserve its fallback path. */
                if(prefix) goto candidate_unhandled;
                if(!monitor_fetch_byte(&code,prefix+1,&second)) goto candidate_unhandled;
                /* NT4 VdmOpcodeClts intentionally performs no CR0 change. */
                if(second==0x06) {setEIP(ip+prefix+2);return TRUE;}
                if(second==0x20) {
                    ULONG value;
                    extern unsigned long c_getCR0(void);
                    if(!monitor_fetch_byte(&code,prefix+2,&modrm) || (modrm&0xc0)!=0xc0) goto candidate_unhandled;
                    value=(modrm&0x38) ? 0 : (c_getCR0() & ~8UL);
                    SET_GR(modrm & 7,value);
                    setEIP(ip+prefix+3);return TRUE;
                }
                if(second==0x21 || second==0x23) {
                    extern unsigned long CCPU_DR[8];
                    extern void ccpu_monitor_write_dr(unsigned long,unsigned long);
                    ULONG value, index;
                    if(!monitor_fetch_byte(&code,prefix+2,&modrm) || (modrm&0xc0)!=0xc0) goto candidate_unhandled;
                    index=(modrm>>3)&7;
                    if(second==0x21) {
                        value=(index==4 || index==5) ? 0 : CCPU_DR[index];
                        if(index==6)value&=0x0000e00fUL;
                        if(index==7)value&=0xffff0155UL;
                        SET_GR(modrm & 7,value);
                    } else if(index!=4 && index!=5) {
                        value=GET_GR(modrm & 7);
                        if(index<4)value&=0x00ffffffUL;
                        else if(index==6)value&=0x0000e00fUL;
                        else value=(value | ((value&0x2aaUL)>>1)) & 0xffff0155UL;
                        /* The breakpoint check observes the next instruction. */
                        setEIP(ip+prefix+3);
                        ccpu_monitor_write_dr(index,value);
                        /* A #DB handler may have changed EIP. */
                        return TRUE;
                    }
                    setEIP(ip+prefix+3);return TRUE;
                }
                if(second==0x22 || second==0x01) {
                    ULONG operand, next=prefix+3;
                    if(!monitor_fetch_byte(&code,prefix+2,&modrm)) goto candidate_unhandled;
                    if(second==0x22) {
                        if((modrm&0xc0)!=0xc0 || (modrm&0x38)!=0) goto candidate_unhandled;
                        operand=GET_GR(modrm & 7);
                    } else {
                        if((modrm&0x38)!=0x30) goto candidate_unhandled;
                        if((modrm&0xc0)==0xc0) operand=GET_GR(modrm & 7);
                        else if(!monitor_lmsw_operand(&code,&next,modrm,&operand)) goto candidate_unhandled;
                    }
                    monitor_set_npx_control(operand);
                    setEIP(ip+next);return TRUE;
                }
                goto candidate_unhandled;
            }
            /* Kt0d110 invokes the protected-VDM dispatcher before the
             * non-VDM IOPL diagnosis in Kt0d50. */
            if(op>=0x6c && op<=0x6f) {
                /* NT4's protected dispatcher passes the length in EBX;
                 * OpcodeINS/OUTS test EBX, not RiPrefixFlags, for REP. */
                if(!monitor_string_io(op)) goto candidate_unhandled;
                setEIP(ip+prefix+1);return TRUE;
            }
            if (op>=0xe4 && op<=0xe7) {
                unsigned long next=prefix+1;
                unsigned char immediate;
                unsigned long port,value=0;
                if(!monitor_fetch_byte(&code,next,&immediate)) goto candidate_unhandled;
                port=immediate;
                if(op==0xe4){IN8(&value,port);setAL(value);} else if(op==0xe5){IN16(&value,port);setAX(value);}
                else if(op==0xe6) OUT8(port,getAL()); else OUT16(port,getAX());
                setEIP(ip+prefix+2);return TRUE;
            }
            if (op>=0xec && op<=0xef) {
                unsigned long port=getDX(),value=0;
                if(op==0xec){IN8(&value,port);setAL(value);} else if(op==0xed){IN16(&value,port);setAX(value);}
                else if(op==0xee) OUT8(port,getAL()); else OUT16(port,getAX());
                setEIP(ip+prefix+1);return TRUE;
            }
            if (op==0xfa || op==0xfb) {
                host_cpu40_set_virtual_if(op==0xfb);
                /* NT4 OpcodeCLI/OpcodeSTI advance the trapped EIP once,
                 * including when its dispatcher encountered a prefix. */
                setIF(op==0xfb); setEIP(ip+1);
                if(op==0xfb) ccpu_monitor_dispatch_irq();
                return TRUE;
            }
        }
    }
candidate_unhandled:
#endif



    /* exception interrupt handler defined ? */
    if(EXIntHandler)
    {
	returnStatus = (EXIntHandler)((ULONG) exp_no, (ULONG) error_code);

#ifndef PROD

	if(!returnStatus)
	    always_trace0("Exception interrupt handler failed (%x)");

#endif /* PROD */
    }

    /* DIVERGENCE(MVDM-HOST-DIV-308): default-off observation of the
     * source-defined CCPU exception-hook result.  This locates a missing
     * DOSX fault-vector publication without changing the hook contract. */
    mvdm_softpc_report_wow_exception_hook((ULONG)exp_no, (ULONG)error_code,
        (ULONG)handlerInstalled, (ULONG)returnStatus);

    return( returnStatus );
}
#endif /* CCPU */

/*(
======================= VdmInstallFaultHandler ===========================
PURPOSE:
	Register a CPU exception interrupt handler called before the CPU
	dispatches the exceptioninterrupt.

INPUT:
	Exception interrupt handler function

OUTPUT:
	Return value - NTSTATUS
================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallFaultHandler IFN1(PVOID, FaultHandler)
{
#ifdef CCPU
    EXIntHandler = FaultHandler;
#else
    GLOBAL_VDM_FaultHandler = FaultHandler;
#endif
    return(STATUS_SUCCESS);
}

#endif /* CPU_40_STYLE */
