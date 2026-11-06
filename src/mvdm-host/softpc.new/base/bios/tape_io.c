#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: cassette_io
 *
 * Description	: Cassette i/o functions - interrupt 15H.
 *
 * Notes	: None
 *
 */

/*
 * static char SccsID[]="@(#)tape_io.c	1.26 06/28/95 Copyright Insignia Solutions Ltd.";
 */


#ifdef SEGMENTATION
#line 20
#error Excluded unsupported SEGMENTATION configuration
#endif

/*
 *    O/S include files.
 */
#line 31
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#include CpuH
#include "sas.h"
/* DIVERGENCE(MVDM-HOST-DIV-085): preserve the original XMS-mediated A20
 * transition by consuming the original XMS interface contract directly. */
#include "xms.h"
#line 40
#include "ios.h"
#include "bios.h"
#include "tape_io.h"
#include "ica.h"
#include "cmos.h"
#include "rtc_bios.h"
#include "cmosbios.h"
#include "trace.h"
#include "debug.h"
#include "quick_ev.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"


#line 52
#define ONE_MEGABYTE    (1024 * 1024)
#define SIXTY_FOUR_K    (64 * 1024)
#define	WRAP_AREA(addr) (addr) >= ONE_MEGABYTE && (addr) < ONE_MEGABYTE + SIXTY_FOUR_K

LOCAL q_ev_handle wait_event_handle = (q_ev_handle)0;

/* Call back routine that needs to set a user's flag byte */
LOCAL void wait_event IFN1(long, parm)
{
	LIN_ADDR addr = (LIN_ADDR)parm;

	note_trace1(CMOS_VERBOSE, "INT15_EVENT_WAIT: delay complete: flag at %05x\n", addr);

	sas_store( addr, sas_hw_at( addr ) | 0x80 );
	wait_event_handle = (q_ev_handle)0;
}

void cassette_io()
{
        unsigned long setup_int15_function = (unsigned long)getAH();
        unsigned long setup_int15_argument = (unsigned long)getAL();
#ifdef PM
#ifndef	CPU_30_STYLE
#line 73
IMPORT void retrieve_descr_fields IPT4(half_word *, AR, sys_addr *, base,
	word *, limit, sys_addr, descr_addr);
#endif	/* not CPU_30_STYLE */
#line 76
half_word cmos_u_m_s_hi;
half_word cmos_u_m_s_lo;
sys_addr gdt;
sys_addr  source;
sys_addr  source_base;
sys_addr  target;
#if (!defined(PROD) || !defined(CPU_30_STYLE))
#line 83
word      source_limit;
word      target_limit;
#endif
#line 86
sys_addr  target_base;
sys_addr byte_count;   /* Max size is 0x8000 * 2 = 10000 */
#ifdef CPU_30_STYLE
#line 89
#error Excluded unsupported CPU30 descriptor path
#else
#line 92
half_word source_AR;
half_word target_AR;
#endif /* CPU_30_STYLE */
#line 95
#endif /* PM */
#line 96

	half_word	mask,		/* interrupt mask			*/
			alarm;		/* value read from alarm register	*/	

#line 100
#if defined(NTVDM) && defined(MONITOR)
#line 101
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif /* NTVDM & MONITOR */
#line 100
        /*
         *	Determine function
         */
	/* DIVERGENCE(MVDM-HOST-DIV-325): register-only diagnostics. */
	mvdm_softpc_report_setup_hardware_probe("int15-enter",
		setup_int15_function, setup_int15_argument);
	switch ( getAH() )
	{
	case INT15_DEVICE_OPEN:
        case INT15_DEVICE_CLOSE:
        case INT15_PROGRAM_TERMINATION:
        case INT15_REQUEST_KEY:
        case INT15_DEVICE_BUSY:
		setAH( 0 );   
                setCF( 0 );
                break;

	case INT15_EMS_DETERMINE:
#line 118
#if 0 /* I'm sure we've all had enough of this one */
#line 119
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 121
#ifdef PM
                outb(CMOS_PORT, CMOS_U_M_S_LO);
                inb(CMOS_DATA, &cmos_u_m_s_lo);
                outb(CMOS_PORT, CMOS_U_M_S_HI);
                inb(CMOS_DATA, &cmos_u_m_s_hi);
                setAH(cmos_u_m_s_hi);
                setAL(cmos_u_m_s_lo);
#else
#line 129
#error Unsupported excluded original configuration
#endif /* PM */
#line 131
		break;
        case INT15_MOVE_BLOCK:
#ifdef PM
               /* Unlike the real PC we don't have to go into protected
                  mode in order to address memory above 1MB, thanks to
                  the wonders of C this function becomes much simpler
                  than the contortions of the bios */

               gdt = effective_addr(getES(), getSI());
               source = gdt + 0x10;   /* see layout in bios listing */
               target = gdt + 0x18;

#line 143
#ifdef CPU_30_STYLE
#line 144
#error Excluded unsupported CPU_30_STYLE configuration
#else /* CPU_30_STYLE */
#line 157
		/* retrieve descriptor information for source */
		retrieve_descr_fields(&source_AR, &source_base, &source_limit, source);

		/* retrieve descriptor information for target */
		retrieve_descr_fields(&target_AR, &target_base, &target_limit, target);
#endif /* CPU_30_STYLE */
#line 163

		/* make word count into a byte count */
		byte_count = getCX() << 1;

		assert1( byte_count <= 0x10000, "Invalid byte_count %x", byte_count );

		/* Check count not outside limits of target
			 and source blocks. */

		assert0( byte_count <= source_limit + 1, "Count outside source limit" );
		assert0( byte_count <= target_limit + 1, "Count outside target limit" );

		/* TO DO: Check base addresses of target and source
			 fall within the area of extended memory
			 that we support */

		/* Go to it */
		if (sas_twenty_bit_wrapping_enabled())
		{
#ifdef NTVDM
			/* call xms functions to deal with A20 line */
			xmsDisableA20Wrapping();
			sas_move_words_forward ( source_base, target_base, byte_count >> 1);
			xmsEnableA20Wrapping();
#else
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 192
		}
		else
			sas_move_words_forward ( source_base, target_base, byte_count >> 1);

		/* set for good completion, just like bios after reset */
		setAH(0);
		setCF(0);
		setZF(1);
		setIF(1);
#else
#line 202
#error Unsupported excluded original configuration
#endif
#line 205
		break;

        case INT15_VIRTUAL_MODE:
                always_trace0("INT15 Virtual Mode (Go into PM)");
#ifdef	PM
		/*
		 * This function returns to the user in protected mode.
		 *
		 * See BIOS listing 5-174 AT Tech Ref for full details
		 *
		 * Upon entry the following is expected to be set up:-
		 *
		 *		ES	- GDT segment
		 *		SI	- GDT offset
		 *		BH	- hardware int level 1 offset
		 *		BL	- hardware int level 2 offset
		 *
		 * Also
		 *
		 *	(ES:SI)	->	0 +-------------+
		 *			  |	 DUMMY	|
		 *			8 +-------------+
		 *			  |	 GDT	|
		 *			16+-------------+
		 *			  |	 IDT	|
		 *			24+-------------+
		 *			  |	 DS	|
		 *			32+-------------+
		 *			  |	 ES	|
		 *			40+-------------+
		 *			  |	 SS	|
		 *			48+-------------+
		 *			  |	 CS	|
		 *			52+-------------+
		 *			  |  (BIOS CS)	|
		 *			  +-------------+
		 */
		
		/* Clear interrupt flag - no ints allowed in this mode. */
		setIF(0);
		 		
		/* Enable a20. */
		sas_disable_20_bit_wrapping();

		/* Reinitialise ICA0 to the offset given in BH. */
		outb(ICA0_PORT_0, (half_word)0x11);
		outb(ICA0_PORT_1, (half_word)getBH());
		outb(ICA0_PORT_1, (half_word)0x04);
		outb(ICA0_PORT_1, (half_word)0x01);
		outb(ICA0_PORT_1, (half_word)0xff);

		/* Reinitialise ICA1 to the offset given in BL. */
		outb(ICA1_PORT_0, (half_word)0x11);
		outb(ICA1_PORT_1, (half_word)getBL());
		outb(ICA1_PORT_1, (half_word)0x02);
		outb(ICA1_PORT_1, (half_word)0x01);
		outb(ICA1_PORT_1, (half_word)0xff);
		
		/* Set DS to the ES value for the bios rom to do the rest. */
		setDS(getES());

#else
#line 267
#error Unsupported excluded original configuration
#endif	/* PM */
#line 270
                break;

	case INT15_INTERRUPT_COMPLETE:
		break;
	case INT15_CONFIGURATION:
#if defined(NTVDM) && defined(MONITOR)
#line 276
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#else
#line 279
		setES( getCS() );
		setBX( CONF_TABLE_OFFSET );
#endif
		setAH( 0 );
		setCF( 0 );
		break;

#line 286
#ifdef	SPC486
#line 287
#error Excluded unsupported SPC486 profile
#endif	/* SPC486 */
#line 294

	/* Keyboard intercept 0x4f, wait_event 83, wait 86 are all no longer
	 * passed through from ROM.
	 */

	default:
		/*
		 *	All other functions invalid.
		 */			
#line 303
#ifndef	PROD
#line 304
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
#line 313

		/* Fall through */

	case INT15_JOYSTICK:
	case 0x24: /* A20 wrapping control */
	case 0xd8: /* EISA device access */
	case 0x41: /* Laptop wait event */
 		setCF(1);
		setAH(INT15_INVALID);
		break;
	}
	mvdm_softpc_report_setup_hardware_probe("int15-leave",
		setup_int15_function, (unsigned long)getAH());
#line 324
}

