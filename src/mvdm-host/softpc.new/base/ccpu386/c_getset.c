#include "insignia.h"
#include "host_def.h"

/*[
 * ============================================================================
 *
 *	Name:		c_getset.c
 *
 *	Derived From:	pig/getsetc.c
 *
 *	Author:		Andrew Ogle
 *
 *	Created On:	9th Febuary 1993
 *
 *	Sccs ID:	@(#)c_getset.c	1.25 12/06/94
 *
 *	Purpose:
 *
 *		Defines procedures for getting and setting the complete
 *		C CPU status required for instruction and application testing
 *		against the assembler CPU.
 *		These routines are used by both the instruction and application
 *		piggers.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1993. All rights reserved.
 *
 * ============================================================================
]*/

#if defined(PIG)
/* DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register. */
#endif /* PIG */
