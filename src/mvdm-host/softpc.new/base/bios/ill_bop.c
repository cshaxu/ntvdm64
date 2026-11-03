#include "insignia.h"
#include "host_def.h"
/*
 * VPC-XT Revision 1.0
 *
 * Title	: illegal_bop.c
 *
 * Description	: A bop instuction has been executed for which
 *		  no VPC function exists.
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 *
 */

#ifdef SCCSID
#line 18
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 20

#ifdef SEGMENTATION
#line 22
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 29


/*
 *    O/S include files.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include TypesH
#include StringH

/*
 * SoftPC include files
 */
#include "xt.h"
#include "sas.h"
#include CpuH
#include "debug.h"

#ifndef	PROD
#line 49
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
#line 57

#line 58
#if defined(NTVDM) && defined(MONITOR)
#line 59
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif
#line 61

void illegal_bop()
{
#ifndef PROD
#line 65
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 154
}
