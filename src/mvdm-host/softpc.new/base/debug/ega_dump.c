#include "insignia.h"
#include "host_def.h"
/*			INSIGNIA (SUB)MODULE SPECIFICATION
			-----------------------------


	THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
	CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
	NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
	AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.

 	(see /vpc/1.0/Master/src/hdrREADME for help)

REVISION HISTORY	:
First version		: 22 Aug 1988, J.Roper`

SOURCE FILE NAME	: ega_dump.c

PURPOSE			: dump the EGA global varaibles stored in structures in an intelligent manner

SccsID = "@(#)ega_dump.c	1.12 3/9/94 Copyright Insignia Solutions Ltd."
		
[3.1 INTERMODULE IMPORTS]						*/


/* [3.1.1 #INCLUDES]                                                    */


#include	<stdio.h>
#include	TypesH
#include	FCntlH

#ifdef	EGG
#ifndef	PROD
#line 35
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
#line 226
#endif	/* EGG */
#line 227
