/*[
*************************************************************************

	Name:		profile.c
	Author:		Simon Frost
	Created:	September 1993
	Derived from:	Original
	Sccs ID:	@(#)profile.c	1.19 01/31/95
	Purpose:	Support for Profiling system

	(c)Copyright Insignia Solutions Ltd., 1993. All rights reserved.

*************************************************************************
]*/

/*(
 * Glossary:
 * EOI - Event of Interest
 * SOI - Section of Interest (between two events)
 * Raw Data Buffer - captures minimum data from event when triggered.
 * Raw Data Buffer flush - process the buffer when full or time available
 *			 to summarise in EOI & SOI lists.
 )*/

/*
 * NB: heap unfriendly in places - could get several structs at a go.
 */


#include "insignia.h"
#include "host_def.h"

#include <stdio.h>
#include StringH

#include CpuH
#include "debug.h"
#include "trace.h"
#include "profile.h"

#ifdef PROFILE		/* Don't want this all the time */
#error Unsupported excluded original configuration
#else /* PROFILE */
#line 3298
GLOBAL void EnableAllEOIs IFN0() { ; }
GLOBAL void DisableAllEOIs IFN0() { ; }
GLOBAL void ProcessProfBuffer IFN0() { ; }
#endif /* PROFILE */
#line 3302
