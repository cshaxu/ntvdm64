#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: Global variable definitions
 *
 * Description	: Contains definitions for registers and general
 *		  variables required by all modules.
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 *
 */

/*
 * static char SccsID[]="@(#)xt.c	1.22 01/23/95 Copyright Insignia Solutions Ltd.";
 */


/*
 *    O/S include files.
 */
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#include CpuH
#include "sas.h"

/*
 * ============================================================================
 * Global data 
 * ============================================================================
 */

/*
   specific CPU variables.
 */
word  cpu_interrupt_map;                /* Bit map of ints outstanding  */

half_word cpu_int_translate[16];        /* this will go very soon!      */
/*
 *
 * The current usage is:
 *
 *              0       - Hardware interrupt
 *              1-7     - Not Used
 *              8       - Software Int - set by cpu_sw_interrupt() - REMOVED!
 *              9       - Trap
 *              10      - Reset IP after a POP/MOV CS.
 *              11      - Trap flag changed - this has delay.
 *              12-15   - Not Used
 */

word cpu_int_delay;                     /* Delay before pending interrupt */

int trap_delay_count;
 
/*
 * The lock flag prevents the Interrupt Controller Adapter from being
 * called from a signal handler while it is already active in the mainline.
 */
 
half_word ica_lock;
 
/*
 * The actual CCPU registers
 */

/*
 * DIVERGENCE(MVDM-HOST-DIV-147): this declaration block is the legacy
 * x86 monitor register store.  The selected CPU_40_STYLE CCPU executor owns
 * its register, descriptor and machine-status state privately in c_main.c;
 * defining this second store would neither bind to it nor preserve event
 * delivery.  Keep the original shared support data and routines below, but
 * exclude only this obsolete CPU-state carrier from the CPU40 build.
 */
#if defined(CCPU) && !defined(CPU_30_STYLE)
#if defined(CCPU) && !defined(CPU_40_STYLE)
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#endif /* defined(CCPU) && !defined(CPU_40_STYLE) */
#endif /* defined(CCPU) && !defined(CPU_30_STYLE) */
#line 147

/*
 * Global Flags and variables
 */

int verbose;			/* TRUE => trace instructions   */

/*
 * Misc. Prot Mode support routines which are independant of CPU type
 *                                  ---------------------------------
 */


#ifdef CPU_30_STYLE
#error Excluded original conditional branch
#endif /* CPU_30_STYLE */
#line 340
/*
 * The following is a table lookup for finding parity of a byte
 */

/*
 * DIVERGENCE(MVDM-HOST-DIV-155): CPU40 CCPU owns this table in c_main.c.
 * The historical support copy serves the non-CCPU monitor path; retaining it
 * in the selected CCPU40 product produces a second provider with no distinct
 * consumer. Unsupported non-CPU40 profiles are rejected below.
 */
#if !defined(MAC_LIKE) && !defined(CPU_30_STYLE)
#if !defined(MAC_LIKE) && !defined(CPU_40_STYLE)
#error Excluded unsupported non-CPU40 parity-table profile
#endif /* !defined(MAC_LIKE) && !defined(CPU_40_STYLE) */
#endif /* !defined(MAC_LIKE) && !defined(CPU_30_STYLE) */
#line 607


/*(
 *========================== CsIsBig ===================================
 * CsIsBig
 *
 * Purpose
 *	This function returns true if the indicated code segment is a 32 bit
 *	one, and false if it isn't.
 *
 * Input
 *	csVal	The selector to check
 *
 * Outputs
 *	None.
 *
 * Description
 *	Look at the descriptor.
)*/

GLOBAL IBOOL
CsIsBig IFN1(IU16, csVal)
{
#ifdef SPC386
	LIN_ADDR base, offset;	/* Of the descriptor to use */

	if(getVM() || !getPE()) {
		return(FALSE);	/* no 32 bit CS in V86 or real modes */
	} else {
		offset = csVal & (~7);	/* remove the RPL and TI gives offset */
		if (csVal & 4) {	/* check TI bit */
			base = getLDT_BASE();
		} else {
			base = getGDT_BASE();
		}

		/*
		 * Return true if the big bit in the descriptor is set.
		 */

		return(sas_hw_at(base + offset + 6) & 0x40);
	}
#else /* SPC386 */
#error DIVERGENCE MVDM-HOST-DIV-539: non-SPC386 branch is not selected; see mirror register.
#endif /* SPC386 */
}
