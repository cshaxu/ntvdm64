/*
 * SoftPC Version 2.0
 *
 * Title	: Interrupt Controller Adapter definitions
 *
 * Description	: Include file for users of the ICA
 *
 * Author	: Jim Hatfield / David Rees
 *
 * Notes	: Rewritten from an original by Henry Nash
 */

/* SccsID[]="@(#)ica.h	1.25 10/19/95 Copyright Insignia Solutions Ltd."; */


/*
 * ============================================================================
 * External declarations and macros
 * ============================================================================
 */

#define	ICA_MASTER	0
#define ICA_SLAVE	1

#define	EGA_VTRACE_INT	2
#define	AT_EGA_VTRACE_INT	1
#define	AT_EGA_VTRACE_ADAPTER	1

/*
 * Allow the host to override the default values if its wants to
 */
#ifndef HOST_CPU_MOUSE_INT
#define HOST_AT_CPU_MOUSE_INT           1
#define HOST_AT_CPU_MOUSE_ADAPTER       1
#define HOST_AT_MOUSE_INT_VEC           0x0a
#define HOST_CPU_MOUSE_INT              2
#endif /* HOST_CPU_MOUSE_INT */
/*
 * CPU hardware interrupt definitions
 */
/* For the XT */
#define CPU_TIMER_INT   	0
#define CPU_KB_INT      	1
#define CPU_MOUSE_INT      	HOST_CPU_MOUSE_INT
#define AT_CPU_MOUSE_INT      	HOST_AT_CPU_MOUSE_INT
#define AT_CPU_MOUSE_ADAPTER	HOST_AT_CPU_MOUSE_ADAPTER
#define	MOUSE_VEC		HOST_AT_MOUSE_INT_VEC
#define CPU_RS232_SEC_INT   	3
#define CPU_RS232_PRI_INT   	4
#define CPU_DISK_INT   		5
#define CPU_DISKETTE_INT   	6
#define CPU_PRINTER_INT   	7
 
/* Different lines for the AT */
#define CPU_PRINTER2_INT	5

/*
 * For the Slave Chip on the AT
 */
#define CPU_RTC_INT		0

#if defined (NOVELL) || defined (NOVELL_IPX)
#error Unsupported excluded original configuration
#endif
#line 65

#if defined (SWIN_HAW) 
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HAW platform branch is not selected; see mirror register.
#endif

#line 70
#if defined (ASPI)
#line 71
#error Unsupported excluded original configuration
#endif
#line 73

#define CPU_AT_NPX_INT		5	/* NPX exception */
#define CPU_AT_DISK_INT		6

#ifndef CPU_30_STYLE
/* DIVERGENCE(MVDM-HOST-DIV-146): the historical combined CCPU build defined
 * CPU_30_STYLE even when it selected CPU_40_STYLE, solely to keep this ICA
 * bit-number macro from replacing the CCPU interrupt enum.  CPU30 is retired
 * in this product; the unselected non-CCPU body is now marked by DIV-506. */
#if !defined(CCPU)
#error DIVERGENCE MVDM-HOST-DIV-506: non-CCPU host branch is not selected; see mirror register.
#endif /* 3.0 CPU or CCPU */
#endif /* CPU_30_STYLE */
#define DIVIDE_OVERFLOW_INT     0

#define	END_INTERRUPT	0x20

extern void ica0_init IPT0();
extern void ica1_init IPT0();
extern void ica0_post IPT0();
extern void ica1_post IPT0();

extern void ica_hw_interrupt_cancel IPT2(IU32, adapter, IU32, line_no);
extern IU8 ica_scan_irr IPT1(IU32, adapter);
extern void ica_interrupt_cpu IPT2(IU32, adapter, IU32, line);
extern void ica_eoi IPT3(IU32, adapter, IS32 *, line, IBOOL, rotate);

#if defined (NTVDM)

#ifdef MONITOR
extern void ica_iret_hook_called IPT1(IU32, line);
extern void ica_iret_hook_control IPT3(IU32, adapter, IU32, line, IBOOL, enable);
#endif

VOID ica_RestartInterrupts(ULONG);
extern IS32 ica_intack IPT1(IU32 *, hook_addr);

extern void ica_clear_int(IU32 adapter, IU32 line);
extern void ica_inb(io_addr port, IU8 *val);
extern void ica_outb(io_addr port, IU8 val);
extern void ica_hw_interrupt(IU32 adapter, IU32 line_no, IS32 call_count);




/*
 *  The NTVDM ica adapter structure has been moved to \nt\private\inc\vdm.h
 *  and is almost identical to the standard softpc ica adapter structure.
 *  It was Extracted to make it visible clearly from the monitor\kernel on
 *  x86
 *
 *  Notable differences:
 *  1. added ica_delayedints field for ntvdm's implementaion of delayed ints
 *  2. type definitions have change to match win32
 *  3. ADAPTER_STATE has been renamed to VDMVIRTUALICA
 *  4. Does not implement CPU_40 iret hooks
 *
 */
#include <vdm.h>
extern VDMVIRTUALICA VirtualIca[];

#else   /* ndef NTVDM */
#error Unsupported excluded original configuration
#endif  /* NTVDM */
#line 206


#if !defined(NTVDM)
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* !NTVDM */
#line 205

#line 243
#ifdef GISP_CPU
#line 244
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_CPU platform branch is not selected; see mirror register.
#endif /* GISP_CPU */
#line 255

