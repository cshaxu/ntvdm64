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
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif

#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_BIOS.seg"
#endif


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
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */

#if defined(NTVDM) && defined(MONITOR)
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif

void illegal_bop()
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 61
}
