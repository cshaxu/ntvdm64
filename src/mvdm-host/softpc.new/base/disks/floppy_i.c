#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 *
 * Title	: Primary SFD BIOS floppy diskette functions
 *
 *
 * Description	: This module defines the floppy diskette BIOS functions
 *		  that are invoked directly from BOP instructions:
 *
 *		  diskette_io()		floppy diskette access functions
 *
 *		  diskette_post()	floppy diskette POST function
 *
 *		  diskette_int()	floppy diskette interrupt handler
 *
 *
 * Author	: Ross Beresford
 *
 *
 * Notes	: 
 *
 */


/*
 * static char SccsID[]="@(#)floppy_io.c	1.14 03/02/94 Copyright Insignia Solutions Ltd.";
 */


#ifdef SEGMENTATION
#line 34
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 41

#include <stdio.h>
#include TypesH
#include "xt.h"
#include "bios.h"
#include "ios.h"
#include CpuH
#include "error.h"
#include "config.h"
#include "gfi.h"
#include "fla.h"
#include "sas.h"
#include "floppy.h"
#include "equip.h"
#include "trace.h"
#include "ica.h"
#include "debug.h"

#include "tape_io.h"

#ifdef NTVDM
extern UTINY number_of_floppy;

#endif	/* NTVDM */

#ifndef	PROD
#line 67
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */
#line 293


void diskette_io()
{
	/*
	 *	Check for valid call and use secondary functions to
	 *	perform the required operation
	 *
	 *	Register inputs:
	 *		AH	operation required
	 *		DL	drive number
	 */
	half_word diskette_status;
	int op = getAH(), drive = getDL();

#ifndef	PROD
#line 309
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 312

#ifndef	JOKER
	/*
	 *	Enable interrupts
	 */

	setIF(1);	
#endif	/* JOKER */

	/*
	 *	Check operation required, using known invalid function
	 *	if operation is out of range
	 */

	if (!fl_operation_in_range(op))
		op = FL_FNC_ERR;

	/*
	 *	If the drive number is applicable in the operation, check it
	 */

	if (op != FL_DISK_RESET && op != FL_DISK_STATUS && op != FL_DISK_PARMS)
#ifdef NTVDM
		if (drive >= number_of_floppy)
#else
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
			op = FL_FNC_ERR;


	/*
	 *	Save previous diskette status, initialise current diskette
	 *	status to OK
	 */

	sas_load(FLOPPY_STATUS, &diskette_status);
	setAH(diskette_status);
	sas_store(FLOPPY_STATUS, FS_OK);

	/*
	 *	Do the operation
	 */

	(*fl_fnc_tab[op])(drive);



#ifndef	PROD
#line 360
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 363
}


#ifndef	JOKER		/* Floppies handled v. weirdly on JOKER */

void diskette_int()
{
	/*
	 * The diskette interrupt service routine.  Mark the Seek Status to say
	 * the interrupt has occurred and terminate the interrupt.
	 */
	half_word seek_status;
	word savedAX, savedCS, savedIP;

	note_trace0(FLOPBIOS_VERBOSE, "diskette_int()");
	sas_load(SEEK_STATUS, &seek_status);
	sas_store(SEEK_STATUS, seek_status | SS_INT_OCCURRED);

	outb(ICA0_PORT_0, END_INTERRUPT);

	/*
	 *	Enable interrupts
	 */

	setIF(1);	

	/*
	 *	Notify OS that BIOS has completed a "wait" for
	 *	a diskette interrupt
	 */
	savedAX = getAX();
	savedCS = getCS();
	savedIP = getIP();

	setAH(INT15_INTERRUPT_COMPLETE);
	setAL(INT15_DEVICE_FLOPPY);
#ifdef NTVDM
	setCS(int15_seg);
	setIP(int15_off);
#else
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */

	host_simulate();

	setAX(savedAX);
	setCS(savedCS);
	setIP(savedIP);
}

#endif	/* ndef JOKER */


#ifdef SEGMENTATION
#line 418
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 425

void diskette_post()
{
	/*
	 *	This routine performs the diskette BIOS initialisation
	 *	functionality in the POST.
	 */
	half_word diskette_id_reg, hf_cntrl, interrupt_mask;
#line 433
#ifndef NTVDM
#line 434
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 436
	EQUIPMENT_WORD equip_flag;

	note_trace0(FLOPBIOS_VERBOSE, "diskette_post()");

	/*
	 *	Set the diskette data rate to 250 kbs (low density)
	 */

	outb(DISKETTE_DCR_REG, DCR_RATE_250);

	/*
	 *	If there are any diskettes installed, check whether
	 *	they are accessed via a DUAL card or the old-style
	 *	adapter card
	 */

	sas_loadw(EQUIP_FLAG, &equip_flag.all);
	if (equip_flag.bits.diskette_present)
	{
		/*
		 *	Enable diskette interrupts
		 */

		inb(ICA0_PORT_1, &interrupt_mask);
		interrupt_mask &= ~(1 << CPU_DISKETTE_INT);
		outb(ICA0_PORT_1, interrupt_mask);

		/*
		 *	If a DUAL diskette/fixed disk adapter is fitted,
		 *	set the drive indicator for drive 0 accordingly
		 */

		inb(DISKETTE_ID_REG, &diskette_id_reg);
		sas_load(DRIVE_CAPABILITY, &hf_cntrl);
		hf_cntrl &= ~DC_DUAL;
		if ((diskette_id_reg & IDR_ID_MASK) == DUAL_CARD_ID)
			hf_cntrl |= DC_DUAL;
		sas_store(DRIVE_CAPABILITY, hf_cntrl);

#line 475
#ifndef NTVDM	/* prevent floppies showing up in bios data area */
#line 476
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* NTVDM */
#line 495
	}
}
