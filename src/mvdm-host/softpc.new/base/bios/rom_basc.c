#include "insignia.h"
#include "host_def.h"
/*[
	Name:		rom_basic.c
	Derived From:	Base 1.0
	Author:		David Rees.
	Created On:	Unknown
	Sccs ID:	08/03/93 @(#)rom_basic.c	1.7
	Purpose:	A function which reports an error when an attempt
			is made to run ROM BASIC.

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

#ifdef SEGMENTATION
#error Excluded unsupported SEGMENTATION configuration





#endif


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
#include "host.h"
#include "error.h"
#include "sas.h"


void rom_basic()
{
	host_error(EG_NO_ROM_BASIC, ERR_CONT, NULL);
}
