#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: Trace function
 *
 * Description	: This function will output a trace to the log device.
 *		  The device is set up in the main function module.  Options
 *		  are provided to VPC memory/register data.
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 *
 * SccsID	: @(#)trace.c	1.36 06/02/95
 *
 * (c)Copyright Insignia Solutions Ltd., 1990-1994. All rights reserved.
 */


#ifdef SEGMENTATION
#line 23
#error Excluded unsupported SEGMENTATION configuration
#endif

/*
 *    O/S include files.
 */
#include <stdlib.h>
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#define CPU_PRIVATE	/* Request the CPU private interface as well */
#include CpuH
#undef CPU_PRIVATE
#include "sas.h"
#include "gvi.h"
#include "trace.h"

#ifdef CPU_40_STYLE
FORWARD CHAR *host_get_287_reg_as_string IPT2(IU32, reg_num, BOOL, in_hex);
FORWARD IU32 get_287_tag_word IPT0();
FORWARD IU32 get_287_control_word IPT0();
FORWARD IU32 get_287_status_word IPT0();
FORWARD IU32 get_287_sp IPT0();
#endif

FILE *trace_file;

#ifndef PROD
#line 60
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */
#line 62

static int trace_start = 0;

GLOBAL IU32 io_verbose = 0;
GLOBAL IU32 sub_io_verbose = 0;

#ifdef RDCHK
#include "egacpu.h"

void get_lar()

{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
}

#endif

#line 91
#if !defined(PROD) && defined(SPC386)
#line 92
#error Unsupported excluded original configuration
#endif /* !PROD && SPC386 */
#line 243

void trace(error_msg, dump_info)
char *error_msg;
int  dump_info;
{
#ifndef PROD
#line 249
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#else	/* PROD */
#line 241
	UNUSED(error_msg);
	UNUSED(dump_info);
#endif	/* PROD */
#line 244
}

void trace_init()
{
#line 468
#if !defined(PROD) || defined(HUNTER)
#line 469
#error Unsupported excluded original configuration
#endif /* !PROD || HUNTER */
#line 493
}

/* Get the code for 4.0 style CPUs */
#ifndef PROD
#line 497
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 279

