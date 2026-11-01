#include "insignia.h"
#include "host_def.h"
/*			INSIGNIA MODULE SPECIFICATION
			-----------------------------

MODULE NAME	: Bios code for accessing the cmos

	THIS PROGRAM SOURCE FILE IS SUPPLIED IN CONFIDENCE TO THE
	CUSTOMER, THE CONTENTS  OR  DETAILS  OF ITS OPERATION MAY 
	ONLY BE DISCLOSED TO PERSONS EMPLOYED BY THE CUSTOMER WHO
	REQUIRE A KNOWLEDGE OF THE  SOFTWARE  CODING TO CARRY OUT 
	THEIR JOB. DISCLOSURE TO ANY OTHER PERSON MUST HAVE PRIOR
	AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS INC.

DESIGNER	: J.P.Box
DATE		: September '88

PURPOSE		: To provide routines to initialise and access the 
		CMOS

The Following Routines are defined:
		1. set_tod
		2. cmos_read
		3. cmos_write

=========================================================================

AMENDMENTS	:

=========================================================================
*/

/*
 * static char SccsID[]="@(#)cmos_bios.c	1.7 08/25/93 Copyright Insignia Solutions Ltd.";
 */


#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_CMOS.seg"
#endif

#include "xt.h"
#include "bios.h"
#include "sas.h"
#include "cmos.h"
#include "cmosbios.h"
#include "ios.h"
#include "rtc_bios.h"

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif


/*
=========================================================================

FUNCTION	: cmos_read

PURPOSE		: Reads a byte from the cmos system clock configuration table
		from the CMOS address specified

RETURNED STATUS	: The value read from the CMOS

DESCRIPTION	:


=======================================================================
*/
half_word cmos_read(table_address)

/*   IN  */
half_word table_address;		/* cmos table address to be read	*/

{
	half_word	value;		/* value read from cmos			*/
	
	outb( CMOS_PORT, (table_address | NMI_DISABLE) );
	
	inb( CMOS_DATA, &value );
	
	/*
	 * Set bit 7 if it was previously set  in table_address
	 */
	outb( CMOS_PORT, (CMOS_SHUT_DOWN | (table_address & NMI_DISABLE)) );
	
	return ( value );
}

/*
=========================================================================

FUNCTION	: cmos_read

PURPOSE		: Writes a byte to the cmos system clock configuration table
		at the CMOS address specified

RETURNED STATUS	: none

DESCRIPTION	:


=======================================================================
*/
void cmos_write(table_address, value)
/*  IN  */
half_word table_address,		/* cmos table address to be written	*/
	  value;			/* value to be written			*/

{
	outb( CMOS_PORT, (table_address | NMI_DISABLE) );

	outb( CMOS_DATA, value );
	
	/*
	 * Set bit 7 if it was previously set  in table_address
	 */
	outb( CMOS_PORT, (CMOS_SHUT_DOWN | (table_address & NMI_DISABLE)) );
	
	return;
}
