#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Version 2.0
 *
 * Title	: Microsoft Bus Mouse Adapter
 *
 * Description	: This package contains a group of functions that provide
 *               an interface between the Microsoft Bus Mouse Card and the cpu.
 *
 *		mouse_init()	Initialise the bus mouse adapter
 *		mouse_inb() 	Supports IN's from ports in the bus mouse range
 *		mouse_outb()	Supports OUT's to ports in the bus mouse range
 *		mouse_send()	Queues data sent from the host mouse
 *
 * Author       : Henry Nash
 *
 * Notes        : See the Microsoft Inport Technical Reference Manual for
 *                further information on the hardware interface.
 *                The Bus Mouse ports are jumper selectable on the real card
 *                between (1) 023C - 023F & (2) 0238 - 023B. We only support
 *                the primary range 023C - 023F.
 *						
 *       (r3.5) : The system directory /usr/include/sys is not available
 *                on a Mac running Finder and MPW. Bracket references to
 *                such include files by "#ifdef macintosh <Mac file> #else
 *                <Unix file> #endif".
 */

#ifdef SCCSID
#line 31
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 33

#ifdef SEGMENTATION
#line 35
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 42


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
#include "cga.h"
#include "bios.h"
#include "error.h"
#include "ios.h"
#include "ica.h"
#include "trace.h"
#include "video.h"
#include "mouse.h"
#include "mouse_io.h"


/*
 * The following globals are also used by xxx_input.c,
 * where xxx is the host machine name.
 */

int button_left  = 0;	/* Current state of left button */
int button_right = 0;	/* Current state of right button */
int delta_x = 0, delta_y = 0;	/* Current mouse delta moves */

#ifndef PROD
#line 77
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 79

static half_word data1_reg = 0,
		 data2_reg = 0,
		 mouse_status_reg = 0;

#if defined(NEC_98)
#line 85
#error Unsupported platform configuration
#endif    //NEC_98
#line 96
static half_word 
		 last_button_left  = 0,
		 last_button_right = 0;

static half_word
                 mouse_mode_reg          = 0, /* mode register 		*/
		 address_reg       = 0; /* address pointer register	*/

static int
		loadsainterrupts = 5;
/* count of how many times we will send bursts of interrupts to please Windows, per reset */


static int mouse_inb_toggle = 0;

void mouse_inb IFN2(io_addr, port, half_word *, value)
{
#if defined(NEC_98)
#line 114
#error Unsupported platform configuration
#else   //NEC_98
#line 139
    if (port == MOUSE_PORT_1) {		/* data register */

	/*
	 * Internal registers
	 */

	switch (address_reg & 0x07) {
	    case 0x00 :  /* mouse status register */
		*value = mouse_status_reg;
		break;
	    case 0x01 : /* data 1 register horizontal */
		*value = data1_reg;
		break;
	    case 0x02 : /* data 2 register vertical */
		*value = data2_reg;
		break;
	    case 0x07 : /* mode register */
		*value = mouse_mode_reg;
		break;
	    default :
#ifndef PROD
#line 160
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 165
		break;
	}
    }

	/*
	 * ID register, alternates between value 1 and value 2
	 * value 1 is the chip signature: DE hex
	 * value 2 is the chip revision: 1  and version: 0
	 */

    else
    if (port == MOUSE_PORT_2) {
	if (mouse_inb_toggle = 1 - mouse_inb_toggle)
		*value = 0xDE;
	else
		*value = 0x10;
    }

#ifndef PROD
#line 184
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#endif    //NEC_98
#line 190
}


void mouse_outb IFN2(io_addr, port, half_word, value)
{
#if defined(NEC_98)
#line 196
#error Unsupported platform configuration
#else    //NEC_98
#line 282
#ifndef PROD
#line 283
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 293

	/*
	 * Out to an internal register
	 */

	if (port == MOUSE_PORT_1) {	/* data register */

		/*
		 * Out to Mode register
		 */
		if ( (address_reg & 0x07) == INTERNAL_MODE_REG) {

			/*
			 * Check hold bit (5) for 0 to 1 transition
			 * - Data Interrupt Enable bit is set (Mode 0)
			 * - counter values saved in Data1 & Data2 registers (Mode 0)
			 * - counters cleared
			 * - status register updated
			 */

			if ((value & 0x20) && ((mouse_mode_reg & 0x20) == 0)) {
#ifndef PROD
#line 315
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 320
				/* clear the interrupt */
				ica_clear_int(AT_CPU_MOUSE_ADAPTER, AT_CPU_MOUSE_INT);

				/* 
				 * read next mouse deltas & buttons 
				 * into the inport registers 
				 */
				data1_reg = (half_word)delta_x;
				data2_reg = (half_word)delta_y;
				mouse_status_reg = 0;
				mouse_status_reg = (button_left << 2) + (button_right);
				if (delta_x!=0 || delta_y!=0) {
					mouse_status_reg |= MOVEMENT;
				}
				if (last_button_right != button_right) {
					mouse_status_reg |= RIGHT_BUTTON_CHANGE;
					last_button_right = (half_word)button_right;
				}

				if (last_button_left != button_left) {
					mouse_status_reg |= LEFT_BUTTON_CHANGE;
					last_button_left = (half_word)button_left;
				}
				delta_x = delta_y = 0;
			}
			/* 1 -> 0 transition on mode register's hold bit */
			/* ready for next read from queue, so send interrupt */
			else if ((mouse_mode_reg & 0x20) && ((value & 0x20)==0)){
#ifndef PROD
#line 349
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 354
			}
			/*
			 * Check timer select value (bits 210)
			 */

			switch (value & 0x7) {
			case 0x0 :		/* 0 Hz INTR low */
#ifndef PROD
#line 362
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 366
				ica_clear_int(AT_CPU_MOUSE_ADAPTER, AT_CPU_MOUSE_INT);
				break;

	/*
	 * In the following cases the application code is expecting to see
	 * interrupts at the requested rate. However in practice this is only
	 * required during initialisation(mouse_mode_reg = 0), and then a short burst
	 * appears to be sufficient. The 15 interupts generated comes from 
	 * tests with the "WINDOWS" package, which receives about 15 during
	 * initialising, but is happy as long as it gets more than 3. The delay
	 * is necessary otherwise the interupts are generated before the 
	 * application starts looking for them.

	 * Mark 2 bodge:
	   Windows 1.02 needs the burst of interrupts to occur even when mouse_mode_reg != 0,
	   but Windows 2.03 needs them not to keep happening even when it asks for them.
	   So now there's a counter called loadsainterrupts set to 5 on resets, which is
	   how many bursts will be allowed. This makes both Windows work.
	 */  
			case 0x1: /* 30 Hz */
			case 0x2: /* 50 Hz */
			case 0x3: /* 100 Hz */
			case 0x4: /* 200 Hz */
/* used to be if mouse_mode_reg == 0 too, removed to make Windows 1.02 work */
			    if( (value & 0x10) && loadsainterrupts > 0)	
			    {
				loadsainterrupts--;
#ifndef PROD
#line 394
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 398
				/*
				** AT version is asking for a 100 interrupts.
				** The AT ica does not handle delayed ints so
				** the IRET has been modified to not allow an
				** int to go off before the next instruction.
				*/
				ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT,100);
			     }
				break;
			case 0x5: /* reserved */
#ifndef PROD
#line 409
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 413
				break;
			case 0x6 :		/* 0 Hz INTR hi */
#ifndef PROD
#line 416
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 420
				ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT,1);
				break;
			case 0x7: /* externally controlled */
				break;
			default:
#ifndef PROD
#line 426
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 431
				break;

			}
		mouse_mode_reg = value;

		/*
	 	 * Interface control register
	 	 */
		}
		else
		if ((address_reg & 0x07) == INTERFACE_CONTROL_REG){
#ifndef PROD
#line 443
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 448
		}

	}
	else if(port == MOUSE_PORT_0)	/* address pointer register */
	{
	    if (value & 0x80)  /* is it  a reset */
	    {
		mouse_mode_reg = 0;
		loadsainterrupts = 5;	/* lets Windows initialise its mouse happily */
		address_reg = value & 0x7F;	/* clear reset bit*/
		ica_clear_int( AT_CPU_MOUSE_ADAPTER, AT_CPU_MOUSE_INT );
	    }
	    else
		address_reg = value;
	}
#endif    //NEC_98
#line 464
}

#ifdef SEGMENTATION
#line 467
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 474

void mouse_send(Delta_x,Delta_y,left,right)
int	Delta_x,Delta_y,left,right;
{
	if(Delta_x != 0 || Delta_y != 0 || button_left != left || button_right != right)
	{
#if defined(NEC_98)
#line 481
#error Unsupported platform configuration
#else    //NEC_98
#line 484
		delta_x += Delta_x;
		delta_y += Delta_y;
#endif   //NEC_98
#line 487

		/***
		Mouse inport registers can only handle one byte
		***/
		if (delta_x < -128)
			delta_x = -128;
		else if (delta_x > 127)
			delta_x = 127;

		if (delta_y < -128)
			delta_y = -128;
		else if (delta_y > 127)
			delta_y = 127;

		button_left = left;
		button_right = right;
#if defined(NEC_98)
#line 504
#error Unsupported platform configuration
#else    //NEC_98
#line 512
		ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER,AT_CPU_MOUSE_INT,1);
#endif   //NEC_98
#line 514
	}
#if defined(NEC_98)
#line 516
#error Unsupported platform configuration
#endif    //NEC_98
#line 528
}

#ifdef SEGMENTATION
#line 531
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 538


void mouse_init IFN0()
{
    IU16 p;

#ifndef PROD
#line 545
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 550

    mouse_inb_toggle = 0;

    io_define_inb(MOUSE_ADAPTOR, mouse_inb);
    io_define_outb(MOUSE_ADAPTOR, mouse_outb);

#if defined(NEC_98)
#line 557
#error Unsupported platform configuration
#else    //NEC_98
#line 565
    for(p = MOUSE_PORT_START; p <= MOUSE_PORT_END; p++) {
#endif   //NEC_98
#line 567
	io_connect_port(p, MOUSE_ADAPTOR, (IU8)IO_READ_WRITE);

#ifdef KIPPER
#line 570
#error Unsupported excluded original configuration
#endif
#line 575

#ifndef PROD
#line 577
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 582
    }
    host_deinstall_host_mouse();
}
