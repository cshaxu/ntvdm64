#include "insignia.h"
#include "host_def.h"

/*
 *    O/S include files.
 */
#include <stdio.h>
#include TypesH

/*
 * SoftPC Revision 2.0
 *
 * Title	: ica.c
 *
 * Description	: Interrupt Controller Adapter
 *
 * Author	: Jim Hatfield
 *                (Upgraded to Rev. 2 by David Rees)
 *
 * Notes	: The ICA is responsible for maintaining a mapping
 *		  between an Interrupt Request line and a vector
 *		  number defining an entry in the Interrupt Vector
 *		  table. On reciept of a hardware interrupt, it
 *		  passes the appropriate vector number to the cpu.
 *
 *		  The following functions are provided:
 *
 *		  ica0_init()	- Initialise the first ICA (0 = Master)
 *		  ica1_init()	- Initialise the first ICA (1 = Slave)
 *		  ica_inb()	- Read a byte from an ICA register
 *		  ica_outb()	- Write a command (byte) to the ICA
 *
 *		  ica_hw_interrupt()	- Raise a hardware interrupt line
 *		  ica_clear_int()	- Drop an interrupt line
 *		  ica_intack()		- Acknowledge an interrupt
 *
 *                If DEBUG is defined, the following function
 *                is provided:
 *
 *                ica_dump()    - printd out contents of one element
 *                                of adapter_state[]
 *
 * Restrictions	: This software emulates an Intel 8259A Priority Interrupt
 *		  controller as defined in the Intel Specification pp 2-95 to
 *		  2-112 and pp 2-114 to 2-181, except for the following:
 *
 *		  1) Cascade mode is not supported at all. This mode requires
 *		     that there is more than one 8259A in a system, whereas
 *		     the PC/XT has only one.
 *
 *		  2) 8080/8085 mode is not supported at all. In this mode the
 *		     8259A requires three INTA pulses from the CPU, and an 8088
 *		     only gives two. This would cause the device to lock up and
 *		     cease to function.
 *
 *		  3) Level triggered mode is not supported. The device is
 *		     assumed to operate in edge triggered mode. A call of
 *		     ica_hw_interrupt by another adapter will cause a bit to
 *		     be latched into the Interrupt Request Register. A subsequent
 *		     call of ica_clear_int will cause the bit to be unlatched.
 *
 *		  4) Buffered mode has no meaning in a software emulation and
 *		     so is ignored.
 *
 *		  5) An enhancement is provided such that an adapter may raise
 *		     more than one interrupt in one call of ica_hw_interrupt.
 *		     The effect of this is that as soon as an INTACK is called
 *		     another interrupt is requested. If the chip is in Automatic
 *		     EOI mode then all of the interrupts will be generated in
 *		     one burst.
 *		  
 *                5a) A further enhancement is provided such that a delay 
 *                   (a number of Intel instructions) can be requested before
 *		     the interrupt takes effect. This delay applies to every
 *		     interrupt if more than one is requested.
 *
 *		  6) Special Fully Nested mode is not supported, since it is
 *		     a submode of Cascade Mode.
 *
 *		  7) Polling is not completely implemented. When a Poll is
 *		     received and there was an interrupt request, the CPU INT
 *		     line (which must have been high) is pulled low. This
 *		     software does NOT reset the jump table address since there
 *		     may be a software interrupt outstanding. However it does
 *		     remove the evidence of a hardware interrupt, which will
 *		     cause the CPU to reset the table address itself.
 *
 *		  When an unsupported mode is set, it is remembered for
 *		  diagnostic purposes, even though it is not acted upon.
 *
 * Modifications for Revision 2.0 : 
 *                1) Restrictions 1 and 6 are lifted. The PC-AT contains two
 *                   8259A interrupt controllers. The first (ICA 0) is in Master
 *                   mode, and the second (ICA 1) is in slave mode, and its
 *                   interrupts are directed to IR2 on the master chip. Hence
 *                   cascade mode must be supported to the extent necessary
 *                   to emulate this situation. Also, Special Fully Nested
 *                   Mode must work too. NB. The AT BIOS does NOT initialise
 *                   the Master 8259A to use Special Fully Nested Mode.
 *
 *                2) Restriction 5a (which is an enhancement) has been
 *                   eliminated. Apparently this never really achieved
 *                   its aim.
 *
 *                3) All the static variables declared in this module
 *                   have been placed within a structure, ADAPTER_STATE,
 *                   which is used as the type for a two-element array.
 *                   This allows the code to emulate two 8259As.
 *
 *                4) The routine ica_standard_vector_address() has been
 *                   eliminated, because it is not used anymore.
 *
 *                5) The function ica_init() has been split into two:
 *                   ica0_init() and ica1_init(). The initialization
 *                   via ICWs will now be done by a BIOS POST routine.
 *
 *                6) In the PC-AT, an 8259A determines its Master/Slave
 *                   state by examining the state of the SP/EN pin. We
 *                   simulate this by setting a flag 'ica_master' to
 *                   the appropriate value in the ica_init() routines.
 *
 *                7) The guts of the exported function ica_intack()
 *                   have been placed in an internal routine,
 *                   ica_accept(). This change allows for the INTAs
 *                   to work for both the master and slave 8259As.
 *
 *                8) Added debug function (ica_dump) to allow module
 *                   testing.
 *
 */

#ifdef SCCSID
#line 133
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 135

#ifdef SEGMENTATION
#line 137
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 144

/* these ports have iret hooks enabled automatically - others will have to
 * define hooked_irets as parts of their configuration
 */
#if !defined(NTVDM)
#line 149
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 153

/*
 * SoftPC include files
 */
#include "xt.h"
#include "trace.h"
#include "ios.h"
#include CpuH
#include "ica.h"
#include "host.h"
#include "yoda.h"
#include "debug.h"

#line 166
#ifdef NOVELL
#line 167
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.
#endif /* NOVELL */
#line 169

/*
 * ============================================================================
 * Local Data
 * ============================================================================
 */

#if !defined(NTVDM)
#line 177
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 186

#if defined (CPU_40_STYLE) || defined (NTVDM)
#define ICA_INTACK_REJECT       -1
#endif

#ifndef PROD
#line 192
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 194

/*
 * ============================================================================
 * Data relating to 8259A is replicated in two element array.
 * ============================================================================
 */

#ifdef NTVDM

#include <nt_eoi.h>

/*
 *  Risc 486 ntvdm does not use iret hooks
 *  x86 ntvdm (monitor) uses iret hooks only on comms
 *  and does not require protect mode enable\disable
 *  control over selectors, as app code cannot switch to
 *  protect mode without going thru dpmi.
 *
 *
 *  NTVDM has to export ICA definition for host and for X86 kernel.
 *  To make the namings of these externally referenced type/var more
 *  sensible but leaving the code substantially as the SoftPC base,
 *  use macros to 'edit' the adapter_state variable and type.
 *
 */
VDMVIRTUALICA VirtualIca[2];
#define adapter_state VirtualIca
#define ADAPTER_STATE VDMVIRTUALICA
#define EOI_HOOKS       /* switch on EOI hooks */





/*
 * for ntvdm and x86 build, this variable is shared between ntvdm and
 * the kernel(the kernel dispatches h/w interrupt and handles iret
 * hook inside kernel)
 */

#ifdef MONITOR
#line 235
#error Excluded original conditional branch
#endif
#line 238


/*
 * noop the ica_lock_set fns, since ntvdm uses a real critical section
 */

#define ica_lock_set(x)
#define ica_lock_inc()
#define ica_lock_dec()


/*
 *  noop the swpic fnptrs
 */

#define SWPIC_inb          ica_inb
#define SWPIC_outb         ica_outb
#define SWPIC_hw_interrupt ica_hw_interrupt
#define SWPIC_clear_int    ica_clear_int


//
// to be removed:
//
// ica_hw_interrupt_func is referenced by emulator libs, so depsite the
// fact that we have removed the swpic function ptrs, we still
// have to provide a fn ptr for the emulator.
//
void
(*ica_hw_interrupt_func)(
    IU32 adapter,
    IU32 line_no,
    IS32 call_count
    )
    = ica_hw_interrupt;



#else   /* !NTVDM */
#line 277
#error Excluded unsupported non-NTVDM alternative
#endif  /* NTVDM */
#line 304

#ifdef HOOKED_IRETS		/* iret hook related variables */
#line 306
#error Unsupported excluded original configuration
#endif /* HOOKED_IRETS */
#line 310


/*
 * ============================================================================
 * Local defines
 * ============================================================================
 */

#define ICA_BASE_MASK	0xf8	/* Mask to get relevant bits out	*/

/*
 * The following defines describe the usage of the mode bits
 */

#define ICA_IC4		0x0001	/* 0 -> no ICW4, 1 -> ICW4 will be sent	*/
#define ICA_SINGL	0x0002	/* 0 -> cascade, 1 -> single mode	*/
#define ICA_ADI		0x0004	/* 0 -> 8 byte,  1 -> 4 byte interval	*/
#define ICA_LTIM	0x0008	/* 0 -> edge,    1 -> level trigger	*/
#define ICA_ICW1_MASK	0x000f	/* Mask to select above bits in mode	*/

#define ICA_MPM		0x0010	/* 0 -> 8080,	 1 -> 8086/8088 mode	*/
#define ICA_AEOI	0x0020	/* 1 -> Automatic End-Of-Int Mode is on	*/
#define ICA_MS		0x0040	/* 0 -> slave,	 1 -> master mode	*/
#define ICA_BUF		0x0080	/* 1 -> Buffered Mode is on		*/
#define ICA_SFNM	0x0100	/* 1 -> Special Fully Nested Mode is on	*/
#define ICA_ICW4_MASK	0x01f0	/* Mask to select above bits in mode	*/

#define ICA_SMM		0x0200	/* 1 -> Special Mask Mode is on		*/
#define ICA_RAEOI	0x0400	/* 1 -> Rotate on Auto EOI Mode is on	*/
#define ICA_RIS		0x0800	/* 0 -> deliver IRR, 1 -> deliver ISR	*/
#define ICA_POLL	0x1000	/* 1 -> Polling is now in progress	*/

/*
 * ============================================================================
 * Macros
 * ============================================================================
 */
#define ICA_PORT_0							\
	(adapter ? ICA1_PORT_0 : ICA0_PORT_0)

#define ICA_PORT_1							\
	(adapter ? ICA1_PORT_1 : ICA0_PORT_1)

#define adapter_for_port(port)						\
	((port >= ICA0_PORT_START && port <= ICA0_PORT_END)		\
		? ICA_MASTER						\
		: ICA_SLAVE						\
	)

/*
 * ============================================================================
 * Internal functions
 * ============================================================================
 */

IS32 ica_accept IPT1(IU32, adapter);

#ifdef HOOKED_IRETS
#line 368
#error Unsupported excluded original configuration
#endif
#line 370

#line 371
#if !defined (NTVDM)
#line 372
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 384

/*
 *	Please note that ica_eoi is called by SUN_VA code and thus needs to
 *	be global.
 */

GLOBAL void
ica_eoi IFN3(IU32, adapter, IS32 *, line, IBOOL, rotate)
{
    /*
     * End Of Interrupt. If *line is -1, this is a non-specific EOI
     * otherwise it is the number of the line to clear. If rotate is
     * TRUE, then set the selected line to lowest priority.
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];
    IS32 i, j;
    IU8 bit;
    IS32 EoiLineNo = -1;	/* if EOI_HOOKS defined this is used otherwise */
				/* rely on compiler elimination as not read */

    if (*line == -1)		/* non specific EOI */
    {
	/*
	 * Clear the highest priority bit in the ISR
	 */
	for(i = 0; i < 8; i++)
	{
	    j = (asp->ica_hipri + i) & 7;
	    bit = (1 << j);
	    if (asp->ica_isr & bit)
	    {
		asp->ica_isr &= ~bit;
		*line = j;
		EoiLineNo = (IS32)*line;
		break;
	    }
	}
    }
    else			/* EOI on specific line */
    {
	bit = 1 << *line;
	if (asp->ica_isr & bit)
		EoiLineNo = *line;
	asp->ica_isr &= ~bit;
    }

#ifndef PROD
#line 432
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 438

    if (rotate && (*line >= 0))
	asp->ica_hipri = (*line + 1) & 0x07;

#ifdef EOI_HOOKS
    /*
     * CallOut to device registered EOI Hooks
     */
    if (EoiLineNo != -1)
	host_EOI_hook(EoiLineNo + (adapter << 3), asp->ica_count[EoiLineNo]);
#endif /* EOI_HOOKS */

    /*
     * There may be a lower priority interrupt pending, so check
     */
    if ((i = ica_scan_irr(adapter)) & 0x80)
	   ica_interrupt_cpu(adapter, i & 0x07);

}

GLOBAL IU8
ica_scan_irr IFN1(IU32, adapter)
{
    /*
     * This is the routine which will decide whether an interrupt should
     * be generated. It scans the IRR, the IMR and the ISR to determine
     * whether one is possible. It is also called when the processor has
     * accepted the interrupt to see which one to deliver.
     *
     * A bit set in the IRR will generate an interrupt if:
     *
     * 1) The corresponding bit in the IMR is clear
     *    AND
     * 2) The corresponding bit and all higher priority bits in the ISR are
     *     clear (unless Special Mask Mode, in which case ISR is ignored)
     *
     * The highest priority set bit which meets the above conditions (if any)
     * will be returned with an indicator bit (in the style needed by a Poll)
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];
    IU32 i, j;
    IUM8 bit, irr_and_not_imr, active_isr;
    IUM8 iret_hook_mask;

#if defined (NTVDM)

#ifdef MONITOR
#line 486
#error Excluded unsupported monitor PIC interrupt ownership
#else
#line 458
    iret_hook_mask = (IU8)(DelayIrqLine >> (adapter << 3));
#endif
#line 490

#else  /* !NTVDM */
#line 492
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 495

    /*
     * A bit can only cause an int if it is set in the IRR
     * and clear in the IMR. Generate a set of such bits
     */

    irr_and_not_imr = asp->ica_irr & ~(asp->ica_imr | iret_hook_mask);

    /*
     * Does the current mode require the ica to prevent
     * interrupts if that line is still active (i.e. in the isr)?
     */
    if (asp->ica_mode & (ICA_SMM|ICA_SFNM))
    {
	/* Neither Special Mask Mode nor Special Fully Nested Mode
	 * block interrupts using bits in the isr.
	 *
	 * SMM is the mode used by Windows95 and Win3.1/E
	 *
	 * Note that "Undocumented PC" says SFNM is not used by PCs and
	 * is only intended for larger systems with > 2 ICAs
	 */
	active_isr = 0;
    }
    else
    {
	/* Normal Case: Used by DOS and Win3.1/S
	 * In this mode the isr prevents interrupts.
	 */
	active_isr = asp->ica_isr;
    }

    /*
     * Check the trivial case first: no bits set
     */

    if (irr_and_not_imr == 0)
	return(7);

    for(i = 0; i < 8; i++)
    {
	j = (asp->ica_hipri + i) & 7;
	bit = (1 << j);
	if (active_isr & bit)
	    return(7);		/* No nested interrupt possible */

	if (irr_and_not_imr & bit)
            return((IU8)(0x80 + j));   /* Return line no. + indicator */
    }
    /* Strange. We should not have got here.  */
    return(7);
}


IS32 ica_accept IFN1(IU32, adapter)
{
    /*
     * NOTE: There is no need to set the lock here, since we are called
     *       either from the cpu interrupt code, or from ica_inb, both of
     *       which will have set it for us.
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];
    IU32	line1;
    IS32	line2;
    IU8		bit;

    /*
     * Drop the INT line
     */

    asp->ica_cpu_int = FALSE;

    /*
     * Scan the IRR to find the line which we will use.
     * There should be one set, but check anyway
     * It there isn't, use line 7.
     */

    if (!((line1 = (IU32)ica_scan_irr(adapter)) & 0x80))
    {
	note_trace1(ICA_VERBOSE, "ica_int_accept(%c): No interrupt found!!", (adapter == ICA_MASTER? 'M': 'S'));

	/* we should really return a spurious interrupt - ie interrupt
	 * on line 7, but this is not neccessary and can cause problems
	 * for some programs (eg DOOM with a mouse)
	 */
#line 582
#if 0
#line 583
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 585

	/* Skip spurious ints. These are any that are caused by clearing an
	 * int when the cpu has already registered that there is an int to
	 * service. 
	 *
	 * This used to be "#if defined(NTVDM) && defined(MONITOR)"
	 * and tried to remove the performance impact on the monitor.
	 */
	return(-1);

    }
    else
    {
        line1 &= 0x07;

#line 600
#if defined(CPU_40_STYLE) && !defined (NTVDM)
#line 601
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif /* CPU_40_STYLE */
#line 611

        bit = (1 << line1);
        asp->ica_isr |= bit;

        if (--(asp->ica_count[line1]) <= 0)
        {				/* If count exhausted for this line */
            asp->ica_irr &= ~bit;	/* Then finally clear IRR bit	*/
            asp->ica_count[line1] = 0;  	/* Just in case		*/
        }
    }

    /*
     * If we are in Automatic EOI mode, then issue a non-specific EOI
     */

    if (asp->ica_mode & ICA_AEOI)
    {
        line2 = -1;
        ica_eoi(adapter, &line2, (asp->ica_mode & ICA_RAEOI) == ICA_RAEOI);
    }

#ifndef PROD
#line 633
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 639

    return((IS32)line1);
}

GLOBAL void
ica_interrupt_cpu IFN2(IU32, adapter, IU32, line)
{
    /*
     * This routine actually interrupts the CPU. The method it does this
     * is host specific, and is done in host_cpu_interrupt().
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];

    /*
     * If the INT line is already high, do nothing.
     */

    if (asp->ica_cpu_int)
    {
#ifndef PROD
#line 660
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 666
        asp->ica_int_line = line;

	return;
    }

    /*
     * Set the ICA internal flags
     */

    asp->ica_int_line = line;
    asp->ica_cpu_int = TRUE;

    if (asp->ica_master)		/* If ICA is Master */
    {
#ifndef PROD
#line 681
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 687

        /*
         *  Set the 'hardware interrupt' bit in cpu_interrupt_map
         */

#ifndef CPU_40_STYLE	/* No globals in the 4.0 I/F! */
#line 693
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
#line 695

        host_set_hw_int();

#line 698
#ifdef A2CPU
#line 699
#error DIVERGENCE MVDM-HOST-DIV-520: A2CPU platform branch is not selected; see mirror register.
#endif
#line 701

#ifdef NTVDM
        /* call wow routine to check for application unable to service ints */
        WOWIdle(FALSE);
#endif

    }
    else
    {				/* If ICA is Slave */
#ifndef PROD
#line 711
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 717
        /*
         * Signal the Master ICA.
         * NB. A kludge is used here. We know that we have
         *     been called from ica_hw_interrupt(), and
         *     therefore ica_lock will be at least 1. To
         *     get the effect we want, it is necessary to
         *     reduce the value of ica_lock for the duration
         *     of the call to ica_hw_interrupt.
	 *
	 * If the host has implemented critical section style locking
	 * then the above kludge does not apply.
         */

        ica_lock_dec();

        ica_hw_interrupt(ICA_MASTER, asp->ica_ssr, 1);

        ica_lock_inc();
    }
}

/*
 * ============================================================================
 * External functions 
 * ============================================================================
 */
void SWPIC_inb IFN2(io_addr, port, IU8 *, value)
{
#ifndef PROD
#line 746
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */    
#line 748
    IU32 adapter       = adapter_for_port(port);
    ADAPTER_STATE *asp = &adapter_state[adapter];

    /*
     * First check the validity of the port
     */

#ifndef PROD
#line 756
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 763

    /*
     * If we are in the middle of a Poll command, then respond to it
     */

    if (asp->ica_mode & ICA_POLL)
    {
	ica_lock_set(1);			/* Lock out signal handlers */
	host_ica_lock();			/* real lock if supported */

	asp->ica_mode &= ~ICA_POLL;

	if ((*value = ica_scan_irr(adapter)) & 0x80) /* See if there is one */
	{
	    (void) ica_accept(adapter);		/* Acknowledge it	*/
            host_clear_hw_int();
	    /*	cpu_int_call_count[0] = 0;         Not used anymore	*/
	}

	ica_lock_set(0);
	host_ica_unlock();			/* free lock if supported */

#ifndef PROD
#line 786
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 792
    }

    /*
     * If the address is ICA_PORT_0, then deliver either the IRR or the ISR,
     * depending on the setting of mode bit ICA_RIS. If the address is
     * ICA_PORT_1, then deliver the IMR
     */

    else
    {
	if (port == ICA_PORT_0)
	    if (asp->ica_mode & ICA_RIS)
	    {
		*value = asp->ica_isr;
#ifndef PROD
#line 807
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */		
#line 809
	    }
	    else
	    {
		*value = asp->ica_irr;
#ifndef PROD
#line 814
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */		
#line 816
	    }
	else
	{
	    *value = asp->ica_imr;
#ifndef PROD
#line 821
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */	    
#line 823
	}

#ifndef PROD
#line 826
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 832
    }
}

void SWPIC_outb IFN2(io_addr, port, IU8, value)
{

    /*
     * Data sent may either be ICWs or OCWs. All of the OCWs are recognisable
     * individually, but only ICW1 may be recognised directly. It will always
     * be followed by ICW2, and optionally by ICW3 and/or ICW4, depending upon
     * exactly what sort of ICW1 was sent. We use a sequence variable to track
     * this and make sure we interpret the data correctly. After power-on, we
     * ignore everything until we get an ICW1.
     */

    /*
     * Some defines to detect command types
     */
#define ICA_SMM_CMD	0x40
#define ICA_POLL_CMD	0x04
#define ICA_RR_CMD	0x02

    /*
     * Local variables
     */
    IU32 adapter               = adapter_for_port(port);
    ADAPTER_STATE *asp = &adapter_state[adapter];

    SAVED IS32 sequence[2]	/* -1 -> power is on but no ICWs received */
	 	  = { -1, -1 };	/*  0 -> fully initialised, OK to proceed */
				/*  2 -> ICW1 received, awaiting ICW2	  */
				/*  3 -> ICW2 received, awaiting ICW3     */
				/*  4 -> awaiting ICW4			  */

    IU32 i;		/* Counter				  */
    IS32 line;			/* Interrupt line number 		  */
#ifdef NTVDM
    IU8 canceled;		/* Device ownership retired by ICW1 reset */
#endif

    /*
     * First check the validity of the port
     */

    if ((port & 0xfffe) != ICA_PORT_0)
    {
#ifndef PROD
#line 876
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 882
	return;
    }

    /*
     * If we get an ICW1 then we are into initialisation
     */

    if (((port & 1) == 0) && (value & 0x10))		/****  ICW1  ****/
    {
	/* DIVERGENCE(MVDM-HOST-DIV-460): ICW1 retires the PIC's pending
	 * and in-service ownership, but the original standalone reset did not
	 * notify the NT device EOI gates. Snapshot before clearing, then send
	 * their existing cancellation notification while the ICA critical section
	 * excludes a heartbeat/device race. A callback can publish a new request;
	 * that request is deliberately not part of this retired snapshot. */
#ifdef NTVDM
	host_ica_lock();
	canceled = asp->ica_irr | asp->ica_isr;
#endif
        asp->ica_irr  = 0;	/* Clear all pending interrupts		*/
        asp->ica_isr  = 0;	/* Clear all in-progress interrupts	*/
        asp->ica_imr  = 0;	/* Clear the mask register		*/
	asp->ica_ssr  = 0;	/* No slaves selected			*/
        asp->ica_base = 0;	/* No base address			*/

	asp->ica_hipri = 0;	/* Line 0 is highest priority		*/

        asp->ica_mode = value & ICA_ICW1_MASK;
				/* Set supplied mode bits from ICW1	*/

	for(i = 0; i < 8; i++)
	    asp->ica_count[i] = 0;	/* Clear IRR extension		*/

	asp->ica_cpu_int = FALSE;	/* No CPU INT outstanding	*/
	sequence[adapter] = 2;		/* Prepare for the rest of the sequence	*/

#ifdef NTVDM
	for (i = 0; i < 8; ++i)
	    if (canceled & (1 << i))
		host_EOI_hook(i + (adapter << 3), -1);
	host_ica_unlock();
#endif

#line 908
#ifndef PROD
#line 909
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 912
        return;
    }

/**/

    /*
     * Lock out calls from signal handlers
     */

    ica_lock_set(1);
    host_ica_lock();			/* real lock if supported */

    /*
     * It wasn't an ICW1, so use the sequence variable to direct our activities
     */

    switch(sequence[adapter])
    {
    case  0:			/* We are expecting an OCW	*/
	if (port & 1)		/* Odd address -> OCW1		*/
	{
	    asp->ica_imr = value & 0xff;
#ifndef PROD
#line 935
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 941
	    if (asp->ica_cpu_int)
	    {
		/* We might have masked out a pending interrupt */
		if (asp->ica_imr & (1 << asp->ica_int_line))
		{
			asp->ica_cpu_int = FALSE;	/* No CPU INT outstanding	*/
			if (asp->ica_master)
				host_clear_hw_int();
			else
				ica_clear_int(ICA_MASTER,asp->ica_ssr);
		}
	    }
	    /*
	     * We might have unmasked a pending interrupt
	     */
	    if (!asp->ica_cpu_int && (line = ica_scan_irr(adapter)) & 0x80)
		ica_interrupt_cpu(adapter, line & 0x07); /* Generate interrupt */
	}
	else
/**/
	if ((value & 8) == 0)	/* Bit 3 unset -> OCW2		*/
	{
	    switch ((value >> 5) & 0x07)
	    {
	    case 0:		/* Clear rotate in auto EOI	*/
		asp->ica_mode &= ~ICA_RAEOI;
#ifndef PROD
#line 968
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 971
		break;

	    case 1:		/* Non-specific EOI		*/
		line = -1;	/* -1 -> highest priority	*/
#ifndef PROD
#line 976
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 979
		ica_eoi(adapter, &line, FALSE);
		break;

	    case 2:		/* No operation			*/
		break;

	    case 3:		/* Specific EOI command		*/
		line  = value & 0x07;
#ifndef PROD
#line 988
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 994
		ica_eoi(adapter, &line, FALSE);
		break;

	    case 4:		/* Set rotate in auto EOI mode	*/
		asp->ica_mode |= ICA_RAEOI;
#ifndef PROD
#line 1000
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1003
		break;

	    case 5:		/* Rotate on non-specific EOI	*/
		line = -1;	/* -1 -> non specific		*/
#ifndef PROD
#line 1008
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1011
		ica_eoi(adapter, &line, TRUE);
		break;

	    case 6:		/* Set priority			*/
		asp->ica_hipri = (value + 1) & 0x07;
#ifndef PROD
#line 1017
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1023
		break;

	    case 7:		/* Rotate on specific EOI	*/
		line  = value & 0x07;
#ifndef PROD
#line 1028
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1034
		ica_eoi(adapter, &line, TRUE);
		break;
	    }
	}
/**/
	else			/* Bit 3 set -> OCW3		*/
	{
	    if (value & ICA_SMM_CMD)	/* Set/unset SMM	*/
	    {
		asp->ica_mode = (asp->ica_mode & ~ICA_SMM) | (((IU16)value << 4) & ICA_SMM);
#ifndef PROD
#line 1045
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1051
	    }

	    if (value & ICA_POLL_CMD)	/* We are being polled	*/
	    {
		asp->ica_mode |= ICA_POLL;
#ifndef PROD
#line 1057
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1060
	    }
	    else
	    if (value & ICA_RR_CMD)	/* Select IRR or ISR	*/
	    {
		asp->ica_mode = (asp->ica_mode & ~ICA_RIS) | (((IU16)value << 11) & ICA_RIS);
#ifndef PROD
#line 1066
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1072
	    }
	}
	break;

/**/
    case  2:			/* We are expecting a ICW2		*/
	if (!(port & 1))	/* Should be odd address, so check	*/
	{
#ifndef PROD
#line 1081
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1085
	}
	else
	{
	    asp->ica_base = value & ICA_BASE_MASK;
#ifndef PROD
#line 1090
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1096
	    if (!(asp->ica_mode & ICA_SINGL))
		sequence[adapter] = 3;
	    else
	    if (asp->ica_mode & ICA_IC4)
		sequence[adapter] = 4;
	    else
		sequence[adapter] = 0;
	}
	break;

/**/
    case  3:			/* We are expecting a ICW3		*/
	if (!(port & 1))	/* Should be odd address, so check	*/
	{
#ifndef PROD
#line 1111
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1115
	}
	else
	{
	    asp->ica_ssr = value & 0xff;
#ifndef PROD
#line 1120
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1126
	    if (asp->ica_mode & ICA_IC4)
		sequence[adapter] = 4;
	    else
		sequence[adapter] = 0;
	}
	break;

/**/
    case  4:			/* We are expecting a ICW4		*/
	if (!(port & 1))	/* Should be odd address, so check	*/
	{
#ifndef PROD
#line 1138
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1142
	}
	else
	{
	    asp->ica_mode = (asp->ica_mode & ~ICA_ICW4_MASK)
		           | (((IU16)value << 4) &  ICA_ICW4_MASK);
#ifndef PROD
#line 1148
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1163
	}
	sequence[adapter] = 0;
	break;

    case -1:		/* Power on but so far uninitialised	*/
#ifndef PROD
#line 1169
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1173
	break;

    default:		/* This cannot happen			*/;
#ifndef PROD
#line 1177
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1179
    }

    ica_lock_set(0);
    host_ica_unlock();			/* free lock if supported */
}


void SWPIC_hw_interrupt IFN3(IU32, adapter, IU32, line_no, IS32, call_count)
{
    /*
     * This routine is called by an adapter to raise an interrupt line.
     * It may or may not interrupt the CPU. The CPU may or may not take
     * any notice.
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];
    IU8 bit;
    IU32 line;
#ifdef CPU_40_STYLE
    IS32 depth, *progress;
#endif	/* CPU_40_STYLE */

#ifndef PROD
#line 1202
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1226

    /*
     * vddsvc.h defines the NTVDM exported call to this function as:
     *    call_ica_hw_interrupt(int, BYTE, int);
     * So, the line_no, being defined here as IU32 is not compatible with
     * VDD's or other DLL's which call through here. Rather than change
     * all the references to it in the source, I'm just going to AND off
     * the potential garbage here.
     */
    line_no &= 0xff;

    host_ica_lock();

#ifndef PROD
#line 1240
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1247

    /*
     * If there is a request already outstanding on this line, then leave
     * the IRR alone, but make a pass through anyway to action previously
     * received but locked calls (see below for details).
     */

    bit = (1 << line_no);
    if (!(asp->ica_irr & bit))
    {
	asp->ica_irr |= bit;		/* Pray we don't get a signal here! */

    }
    asp->ica_count[line_no] += call_count;	/* Add the further requests */

#ifndef PROD
#line 1263
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1270

    /*
     * Check the lock flag. If it is set, then this routine is being called
     * from a signal handler while something else is going on. We can't just
     * ignore the call since we might lose a keyboard interrupt. What we do
     * is to set ica_irr and ica_count as normal (ie code above), then return.
     * The next interrupt which gets through this test will cause the stored
     * interrupt to be processed. This means that any code which plays around
     * with ica_irr and ica_count should take a copy first to prevent problems.
     *
     * If the host supports real (critical section style) locks, then we won't
     * get here in the above situation, so eliminate the following test. That
     * leaves both primitive & real lock styles intact.
     */

#ifndef NTVDM
#line 1286
#error DIVERGENCE MVDM-HOST-DIV-591: non-NTVDM branch is not selected
#endif
#line 1302

#line 1303
#if defined (CPU_40_STYLE) && !defined(NTVDM)
#line 1304
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* CPU_40_STYLE */
#line 1322

    /*
     * Now scan the IRR to see if we can raise a CPU interrupt.
     */

    if ((line = ica_scan_irr(adapter)) & 0x80)
	ica_interrupt_cpu(adapter, line & 0x07);

    ica_lock_set(0);
    host_ica_unlock();
}

void SWPIC_clear_int IFN2(IU32, adapter, IU32, line_no)
{
    /*
     * This routine is called by an adapter to lower an input line.
     * The line will then not interrupt the CPU, unless of course
     * it has already done so.
     */

    ADAPTER_STATE *asp = &adapter_state[adapter];
    IU8 bit, irr_check;

    host_ica_lock();
    /*
     * Decrement the call count and if zero clear the bit in the IRR
     */

    bit = (1 << line_no);
    if (--(asp->ica_count[line_no]) <= 0)
    {
	irr_check = asp->ica_irr;
	asp->ica_irr &= ~bit;
	asp->ica_count[line_no] = 0;		/* Just in case */
	if ((!asp->ica_master) && (ica_scan_irr(adapter)==7))
		{
		asp->ica_cpu_int=FALSE;
		ica_clear_int(ICA_MASTER,asp->ica_ssr);
		}
#ifdef EOI_HOOKS
	/*
	// If the line has a pending interrupt, call the eoi hook
	// to release any device waiting for an EoiHook.
	*/
	if ((irr_check & bit) != 0)
		host_EOI_hook(line_no + (adapter << 3), -1);
#endif	/* EOI_HOOKS */
    }

#ifndef PROD
#line 1372
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1378

    host_ica_unlock();
}

/*
 * The emulation code associated with this interrupt line has decided it
 * doesn't want to generate any more interrupts, even though the ICA may not
 * have got through all the interrupts previously requested.
 * Simply clear the corresponding interrupt count.
 */
void ica_hw_interrupt_cancel IFN2(IU32, adapter, IU32, line_no)
{
	host_ica_lock();
	adapter_state[adapter].ica_count[line_no] = 0;
	host_ica_unlock();
	ica_clear_int(adapter, line_no);
}

#if defined(HOOKED_IRETS) || defined(NTVDM)
GLOBAL IS32
ica_intack IFN1(IU32 *, hook_address)
#else
#line 1400
#error Unsupported excluded original configuration
#endif	/* HOOKED_IRETS */
#line 1403
{
    /*
     * This routine is called by the CPU when it wishes to acknowledge
     * an interrupt. It is equivalent to the INTA pulses from the real
     * device. The interrupt number is delivered.
     * It can also be called from ica_inb as a Poll.
     *
     * Modification for Rev. 2:
     *
     * It is now necessary to detect whether a slave interrupt controller
     * is attached to a particular interrupt request line on the master
     * ICA. If a slave exists, it must be accessed to discover the
     * interrupt vector.
     */
    IS32 line;		/* the IRQ line */
    IU8 bit;		/* bitmask for 'line' */
    IS32 int_no;	/* The interrupt number to return, 0-255 */
    ADAPTER_STATE *asp;	/* working pointer to adapter */

    host_ica_lock();	/* real lock if supported */

    line = ica_accept(ICA_MASTER);

#if defined (CPU_40_STYLE) || defined (NTVDM)
    if (line == -1)	/* skip any spurious ints */
    {
	host_ica_unlock();
	return ICA_INTACK_REJECT;
    }
#endif	/* CPU_40_STYLE */

    bit  = (1 << line);
    if (adapter_state[ICA_MASTER].ica_ssr & bit)
    {
        line = ica_accept(ICA_SLAVE);
	int_no = line + adapter_state[ICA_SLAVE].ica_base;

#if defined (CPU_40_STYLE) || defined (NTVDM)
	if (line == -1)	/* skip any spurious ints */
        {
            adapter_state[ICA_MASTER].ica_isr &= ~bit;
            host_ica_unlock();
	    return ICA_INTACK_REJECT;
	}
#endif	/* CPU_40_STYLE */

#if defined(CPU_40_STYLE) && !defined(NTVDM)
#line 1450
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif  /* CPU_40_STYLE */
#line 1462

        line += 8;      /* make in range 8 - 15 for iret hook */

    }
    else
    {
	asp = &adapter_state[ICA_MASTER];	/* also excuse to use asp */

#line 1470
#if defined(CPU_40_STYLE) &&  !defined(NTVDM)
#line 1471
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* CPU_40_STYLE */
#line 1482

	int_no = line + asp->ica_base;
    }


#if defined (NTVDM)

#ifdef MONITOR
#line 1490
#error Excluded unsupported monitor PIC IRET address
#else
#line 1302
    *hook_address = 0;
#endif
#line 1497


#else
#line 1500
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* !NTVDM */
#line 1523

    host_ica_unlock();		/* real lock if supported */

    return(int_no);
}


#ifndef NTVDM
#line 1531
#error Excluded unsupported non-NTVDM PIC profile
#endif  /* !NTVDM */
#line 1761



#if defined(NTVDM)


#ifdef MONITOR
#line 1767
#error Excluded unsupported monitor PIC IRET hook
#else
#line 1545
/*
 *  obsolete, to be removed... but the current ntvdm emulator libs ref
 * */

GLOBAL void
ica_iret_hook_called IFN1(IU32, abs_line)
{
    return; // do nothing!
}
#endif
#line 1809

#endif   /* NTVDM */


#ifdef HOOKED_IRETS
#line 1814
#error Unsupported excluded original configuration
#endif	/* HOOKED_IRETS */
#line 1991


#ifdef SEGMENTATION
#line 1994
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 2001

#define INIT0_ICW1	(IU8)0x11	
#define INIT0_ICW2	(IU8)0x08	
#define INIT0_ICW3	(IU8)0x04	
#define INIT0_ICW4      (IU8)0x01

#ifdef NTVDM
#define INIT0_OCW1      (IU8)0x0

#else
#line 2011
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 2020

void ica0_post IFN0()
{
    ica_outb(ICA0_PORT_0, INIT0_ICW1);
    ica_outb(ICA0_PORT_1, INIT0_ICW2);
    ica_outb(ICA0_PORT_1, INIT0_ICW3);
    ica_outb(ICA0_PORT_1, INIT0_ICW4);
    ica_outb(ICA0_PORT_1, INIT0_OCW1);
}

void ica0_init IFN0()
{
    io_addr i;

    /*
     * Set up the IO chip select logic for adapter 0. (Master).
     */  

#ifdef NTVDM
    io_define_inb(ICA0_ADAPTOR, ica_inb);
    io_define_outb(ICA0_ADAPTOR, ica_outb);
#else
#line 2042
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 2045

    for(i = ICA0_PORT_START; i <= ICA0_PORT_END; i++)
        io_connect_port(i, ICA0_ADAPTOR, IO_READ_WRITE);

    adapter_state[ICA_MASTER].ica_master = TRUE;


#ifndef NTVDM
#line 2053
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* !NTVDM */
#line 2072

}


#define INIT1_ICW1	(IU8)0x11	
#define INIT1_ICW2	(IU8)0x70	
#define INIT1_ICW3	(IU8)0x02	
#define INIT1_ICW4      (IU8)0x01

#ifdef NTVDM
#define INIT1_OCW1      (IU8)0x0


/* POST leaves some int lines masked out (reserved lines and RTC) */
/* see the comment on POST setting mask bits for master ica */
#elif ASPI
#line 2088
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* ASPI */	
#line 2090
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* ASPI */
#line 2092




void ica1_post IFN0()
{
    ica_outb(ICA1_PORT_0, INIT1_ICW1);
    ica_outb(ICA1_PORT_1, INIT1_ICW2);
    ica_outb(ICA1_PORT_1, INIT1_ICW3);
    ica_outb(ICA1_PORT_1, INIT1_ICW4);
    ica_outb(ICA1_PORT_1, INIT1_OCW1);
}

void ica1_init IFN0()
{
    io_addr i;

    /*
     * Set up the IO chip select logic for adapter 1. (Slave).
     */  

#ifdef NTVDM
    io_define_inb(ICA1_ADAPTOR, ica_inb);
    io_define_outb(ICA1_ADAPTOR, ica_outb);
#else
#line 2117
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 2120

    for(i = ICA1_PORT_START; i <= ICA1_PORT_END; i++)
        io_connect_port(i, ICA1_ADAPTOR, IO_READ_WRITE);

    adapter_state[ICA_SLAVE].ica_master = FALSE;

#if defined(CPU_40_STYLE) && !defined(NTVDM)
#line 2127
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* CPU_40_STYLE */
#line 2133

}


#line 2137
#if !defined(PROD)
#line 2138
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 2244

