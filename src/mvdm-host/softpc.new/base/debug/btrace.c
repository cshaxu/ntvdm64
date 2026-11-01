#include "insignia.h"
#include "host_def.h"
/*
 * VPC-XT Revision 1.0
 *
 * Title	: Buffer Trace function
 *
 * Description	: This function will output a trace to the trace buffer
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
 * The following #include define specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_ERROR.seg"
#endif



/*
 *    O/S include files.
 */
#include <stdio.h>
#include TypesH
#include StringH

/*
 * SoftPC include files
 */
#include "xt.h"
#include CpuH
#include "sas.h"
#include "trace.h"

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#ifdef ANSI
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#ifdef DELTA
#error Excluded original conditional branch
#endif
#ifdef DELTA
#error Excluded original conditional branch
#endif
#endif
#line 49
