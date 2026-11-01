/*[
 *	Name:		pigyoda.c
 *
 *	Author:	William Charnell
 *
 *	Created:	May 1991
 *
 *	Sccs ID:	@(#)pigyoda.c	1.9 06/30/94
 *
 *	Purpose:	This module contains the yoda interface routines for
 *			an application pigger.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.
]*/

/* Standard Includes */
#include "insignia.h"
#include "host_def.h"

/* System Includes */
#include <stdio.h>
#include <ctype.h>

/* SoftPC Includes */
#include "xt.h"
#include "bios.h"
#include "sas.h"
#include "trace.h"

#ifdef PIG
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#ifdef ANSI
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif /* PIG */
#line 33
