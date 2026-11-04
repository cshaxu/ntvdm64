#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: NT 3.0 CPU initialization
 *
 * Description	: Initialize the CPU and its registers.
 *
 * Author	: Paul Huckle / Henry Nash
 *
 * Notes	: None
 */

static char SccsID[]="@(#)sun4_a3cpu.c	1.2 5/24/91 Copyright Insignia Solutions Ltd.";

#include <stdio.h>
#include <sys/types.h>
#include <malloc.h>
#include "xt.h"
#include CpuH
#include "sas.h"
#include "ios.h"
#include "bios.h"
#include "trace.h"
#include "yoda.h"
#include "debug.h"

#include "sim32.h"

#ifdef CPU_40_STYLE
#include "cpu_c.h"
/* DIVERGENCE(MVDM-HOST-DIV-048): fm_c.h is
 * the selected generated definition of CleanedREC, which this host carrier
 * needs to name the original EIP member rather than treating the record as
 * an untyped 32-bit word. */
#include "fm_c.h"
#endif

/* The original CCPU40 disk/FDC controller uses this independent timing
 * table.  It is not a SAS-memory global and therefore remains available in
 * the CCPU profile. */
GLOBAL	quick_event_delays	host_delays;

/* DIVERGENCE(MVDM-HOST-DIV-030): CCPU SAS owns Start_of_M_area and
 * Length_of_M_area in ccpusas4.c.  These two historical duplicate host
 * definitions are unused by the selected CCPU path and collide at a modern
 * static link. The unselected non-CCPU body is now marked by DIV-505. */
#ifndef CCPU
#error DIVERGENCE MVDM-HOST-DIV-505: non-CCPU host branch is not selected; see mirror register.
#endif
#line 42
#ifdef A2CPU
#line 43
#error DIVERGENCE MVDM-HOST-DIV-520: A2CPU platform branch is not selected; see mirror register.
#endif
#line 47

#line 48
#ifdef I286
#line 49
#error Unsupported excluded original configuration
#endif
#line 56

#define setBASE(base, value) base = (0xff000000 | (value & 0xffffff))
#define setLIMIT(limit, value) limit = value

#line 60
#ifndef PROD
#line 61
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 63




/*
	Host_start_cpu: This function starts up the cpu emulation if
	we are running a software emulation, or if a 486 is present,
	starts up this emulation.
*/
/* DIVERGENCE(MVDM-HOST-DIV-356): original host initialization is complete;
 * only now may service instructions enter its original cleanup callbacks. */
extern void NtvdmActivateLifecycle(void);
void	host_start_cpu()
{
  NtvdmActivateLifecycle();
  cpu_simulate ();
}


/*
	host_simulate: This function starts up the cpu emulation
	for recursive CPU calls from the Insignia BIOS
*/
void	host_simulate()
{
    ASSERT(IcaLock.OwningThread != NtCurrentTeb()->ClientId.UniqueThread);

#line 86
#ifdef _ALPHA_
#line 87
#error Unsupported excluded original configuration
#endif
#line 101

    cpu_simulate ();

    ASSERT(IcaLock.OwningThread != NtCurrentTeb()->ClientId.UniqueThread);
}

/* DIVERGENCE(MVDM-HOST-DIV-153): BIOS BOP FE keeps the historical
 * host_unsimulate public entry.  The selected CCPU40 implementation is the
 * original c_cpu_unsimulate provider; publish the thin original-name bridge
 * rather than inventing an alternate stop mechanism. */
#ifdef CCPU
extern void c_cpu_unsimulate(void);
void host_unsimulate()
{
    c_cpu_unsimulate();
}
#endif /* CCPU */


/*
	Host_set_hw_int: Cause a hardware interrupt to be generated. For
	software cpu this just means setting a bit in cpu_interrupt_map.
*/
void	host_set_hw_int()
{
	cpu_interrupt(CPU_HW_INT, 0);
}

/*
	Host_clear_hw_int: Cause a hardware interrupt to be cleared. For
	software cpu this just means clearing a bit in cpu_interrupt_map.
        Monitor has it's own version, a3 cpu has it's own (differently named
        version).
*/
#ifndef MONITOR
void	host_clear_hw_int()
{
#ifndef CCPU
#line 127
#error DIVERGENCE MVDM-HOST-DIV-505: non-CCPU host branch is not selected; see mirror register.
#endif /* not CCPU */
#line 136
}
#endif
#line 138


#ifdef CCPU

void host_cpu_init() {}

#endif

#ifdef A3CPU
#line 147
#error Excluded unsupported A3CPU configuration
#endif /* A3CPU */
#line 197

void host_cpu_reset()
{
}


void host_cpu_interrupt()
{
}

#ifdef CPU_40_STYLE

typedef struct NT_CPU_REG {
    IU32 *nano_reg;         /* where the nano CPU keeps the register */
    IU32 *reg;              /* where the light compiler keeps the reg */
    IU32 *saved_reg;        /* where currently unused bits are kept */
    IU32 universe_8bit_mask;/* is register in 8-bit form? */
    IU32 universe_16bit_mask;/* is register in 16-bit form? */
} NT_CPU_REG;

typedef struct NT_CPU_INFO {
    /* Variables for deciding what mode we're in */
    BOOL *in_nano_cpu;      /* is the Nano CPU executing? */
    IU32 *universe;         /* the mode that the CPU is in */

    /* General purpose register pointers */
    NT_CPU_REG eax, ebx, ecx, edx, esi, edi, ebp;

    /* Variables for getting SP or ESP. */
    BOOL *stack_is_big;     /* is the stack 32-bit? */
    IU32 *nano_esp;         /* where the Nano CPU keeps ESP */
    IU8 **host_sp;          /* ptr to variable holding stack pointer as a
                               host address */
    IU8 **ss_base;          /* ptr to variables holding base of SS as a
                               host address */
    IU32 *esp_sanctuary;    /* top 16 bits of ESP if we're now using SP */

    IU32 *eip;

    /* Segment registers. */
    IU16 *cs, *ds, *es, *fs, *gs, *ss;

    IU32 *flags;

    /* CR0, mainly to let us figure out if we're in real or protect mode */
    IU32 *cr0;
} NT_CPU_INFO;

GLOBAL NT_CPU_INFO nt_cpu_info;

LOCAL void initNtCpuRegInfo IFN6(
    NT_CPU_REG *, info,
    IU32 *, nano_reg,
    IU32 *, main_reg,
    IU32 *, saved_reg,
    IU32, in_8bit_form,
    IU32, in_16bit_form
)
{
    info->nano_reg = nano_reg;
    info->reg = main_reg;
    info->saved_reg = saved_reg;
    info->universe_8bit_mask = in_8bit_form;
    info->universe_16bit_mask = in_16bit_form;
}



GLOBAL void InitNtCpuInfo IFN0()
{
    /* Variables for deciding what mode we're in, and hence where the */
    /* register values are kept. */

    /* Horrible hack, part 1. InNanoCpu is a BOOL, so GLOBAL_InNanoCpu */
#ifdef ALPHA
#line 275
#error Unsupported excluded original configuration
#else /* ALPHA */
#line 277
    /* DIVERGENCE(MVDM-HOST-DIV-048): the generated field is an lvalue in the
     * private GDP carrier; avoid a historical hard-coded native offset. */
    nt_cpu_info.in_nano_cpu = (BOOL *)&GLOBAL_InNanoCpu;
#endif /* ALPHA */
#line 279
#ifndef PROD
#line 280
#error Unsupported excluded original configuration
#endif
#line 286
    /* DIVERGENCE(MVDM-HOST-DIV-048): the historical non-PROD assertion
     * compared against GdpOffsetFromName(), an undeclared NT4 packed-GDP
     * debug helper.  The selected GDP is now a generated native-width
     * carrier, so that numeric packed-offset comparison has no valid target.
     * The source-named field above is the single checked representation. */

    /* DIVERGENCE(MVDM-HOST-DIV-048): these CCPU control/register cells are
     * fixed 32-bit Intel values.  The original IUH spelling happened to be
     * identical to IU32 on NT4; state that source contract explicitly. */
    nt_cpu_info.universe = (IU32 *)&GLOBAL_CurrentUniverse;

    /* Variables needed to get the value SP or ESP. */

#ifdef ALPHA
#line 293
#error Unsupported excluded original configuration
#else /* ALPHA */
#line 295
    /* DIVERGENCE(MVDM-HOST-DIV-048): use the generated source field rather
     * than an architecture-specific byte offset into the old packed GDP. */
    nt_cpu_info.stack_is_big = (BOOL *)&GLOBAL_stackIsBig;
#endif /* ALPHA */
#line 297
#ifndef PROD
#line 298
#error Unsupported excluded original configuration
#endif
#line 304
    /* DIVERGENCE(MVDM-HOST-DIV-048): see the InNanoCpu note above. */

#line 305
    nt_cpu_info.nano_esp = &GLOBAL_nanoEsp;
    nt_cpu_info.host_sp = &GLOBAL_HSP;
    nt_cpu_info.ss_base = &GLOBAL_notionalSsBase;
    nt_cpu_info.esp_sanctuary = &GLOBAL_ESPsanctuary;

    /* Pointers to the segment registers. */
    nt_cpu_info.cs = &GLOBAL_CsSel;
    nt_cpu_info.ds = &GLOBAL_DsSel;
    nt_cpu_info.es = &GLOBAL_EsSel;
    nt_cpu_info.fs = &GLOBAL_FsSel;
    nt_cpu_info.gs = &GLOBAL_GsSel;
    nt_cpu_info.ss = &GLOBAL_SsSel;

    /* EIP & flags, neither of which are likely to be very reliable. */
    nt_cpu_info.eip = &GLOBAL_CleanedRec.EIP;
    nt_cpu_info.flags = (IU32 *)&GLOBAL_EFLAGS;

    nt_cpu_info.cr0 = (IU32 *)&GLOBAL_R_CR0;
    /* General purpose registers. */
    initNtCpuRegInfo(&nt_cpu_info.eax, &GLOBAL_nanoEax, (IU32 *)&GLOBAL_R_EAX,
                     (IU32 *)&GLOBAL_EAXsaved, 1 << ConstraintRAL_LS8,
                     1 << ConstraintRAX_LS16);
    initNtCpuRegInfo(&nt_cpu_info.ebx, &GLOBAL_nanoEbx, (IU32 *)&GLOBAL_R_EBX,
                     (IU32 *)&GLOBAL_EBXsaved, 1 << ConstraintRBL_LS8,
                     1 << ConstraintRBX_LS16);
    initNtCpuRegInfo(&nt_cpu_info.ecx, &GLOBAL_nanoEcx, (IU32 *)&GLOBAL_R_ECX,
                     (IU32 *)&GLOBAL_ECXsaved, 1 << ConstraintRCL_LS8,
                     1 << ConstraintRCX_LS16);
    initNtCpuRegInfo(&nt_cpu_info.edx, &GLOBAL_nanoEdx, (IU32 *)&GLOBAL_R_EDX,
                     (IU32 *)&GLOBAL_EDXsaved, 1 << ConstraintRDL_LS8,
                     1 << ConstraintRDX_LS16);
    initNtCpuRegInfo(&nt_cpu_info.esi, &GLOBAL_nanoEsi, (IU32 *)&GLOBAL_R_ESI,
                     (IU32 *)&GLOBAL_ESIsaved, 0, 1 << ConstraintRSI_LS16);
    initNtCpuRegInfo(&nt_cpu_info.edi, &GLOBAL_nanoEdi, (IU32 *)&GLOBAL_R_EDI,
                     (IU32 *)&GLOBAL_EDIsaved, 0, 1 << ConstraintRDI_LS16);
    initNtCpuRegInfo(&nt_cpu_info.ebp, &GLOBAL_nanoEbp, (IU32 *)&GLOBAL_R_EBP,
                     (IU32 *)&GLOBAL_EBPsaved, 0, 1 << ConstraintRBP_LS16);
}

#endif /* CPU_40_STYLE */
