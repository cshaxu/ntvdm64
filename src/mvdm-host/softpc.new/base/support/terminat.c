#include "insignia.h"
#include "host_def.h"
/*[
	Name:		terminate.c
	Derived From:	Base 2.0
	Author:		Rod MacGregor
	Created On:	Unknown
	Sccs ID:	@(#)terminate.c	1.23 06/15/94
	Purpose:	We are about to die, put the kernel back the way
			that it was.

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

#include <stdlib.h>
#include <stdio.h>
#include TypesH

#include "xt.h"
#include "error.h"
#include "config.h"
#include "host_lpt.h"
#include "rs232.h"
#include "host_com.h"
#include "timer.h"
#include "cmos.h"
#include "fdisk.h"
#include "debug.h"
#include "gvi.h"
#include CpuH
#ifdef NOVELL
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.
#endif
#ifdef GISP_SVGA
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#ifdef LICENSING
#error DIVERGENCE MVDM-HOST-DIV-531: LICENSING branch is not selected; see mirror register.
#endif
#include "emm.h"

#ifdef SEGMENTATION
#error Excluded unsupported SEGMENTATION configuration





#endif

IMPORT VOID host_applClose IPT0();
IMPORT VOID host_terminate IPT0();
/* DIVERGENCE(MVDM-HOST-DIV-356): standalone service permission precedes cleanup. */
IMPORT VOID ntvdm_worker_lifecycle_authorize_exit IPT0();

void terminate()
{
	SAVED BOOL already_called_terminate = FALSE;
	UTINY i;

	/* DIVERGENCE(MVDM-HOST-DIV-356): preserve the original cleanup thread/order. */
	ntvdm_worker_lifecycle_authorize_exit();

	if (already_called_terminate)
	{
		assert0( NO, "Error: terminate called twice - exiting" );
		exit(0);
	}
	else
		already_called_terminate = TRUE;

#ifdef MSWDVR
#error Unsupported excluded original configuration
#endif
#line 76

#ifdef SWIN_SNDBLST_NULL
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_SNDBLST_NULL platform branch is not selected; see mirror register.
#endif /* SWIN_SNDBLST_NULL */

#ifdef GISP_SVGA
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */

	/* terminate COM and LPT devices */
#ifdef  PC_CONFIG
#error DIVERGENCE MVDM-HOST-DIV-531: PC_CONFIG branch is not selected; see mirror register.
#else
	for (i = 0 ; i < NUM_PARALLEL_PORTS; i++)
		config_activate(C_LPT1_NAME + i, FALSE);

	for (i = 0 ; i < NUM_SERIAL_PORTS; i++)
		config_activate(C_COM1_NAME + i, FALSE);
#endif

	/* Update the cmos.ram file */
	cmos_update();

	host_fdisk_term();

	gvi_term();	/* close down the video adaptor */

#line 103
#ifndef NTVDM
#line 104
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 106

#ifdef LIM
	host_deinitialise_EM(); /* free memory or file used by EM */
#endif

	config_activate(C_FLOPPY_A_DEVICE, FALSE);
#ifdef FLOPPY_B
#error Unsupported excluded original configuration
#endif /* FLOPPY_B */
#line 115
#ifdef SLAVEPC
#error DIVERGENCE MVDM-HOST-DIV-528: SLAVEPC platform branch is not selected; see mirror register.
#endif /* SLAVEPC */

	/*
	 * Do any cpu-specific termination bits.
	 */
#ifdef CPU_30_STYLE
#error Excluded original CPU_30_STYLE branch
#endif
#line 122

#ifdef NOVELL
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.
#endif

#ifdef LICENSING
#error DIVERGENCE MVDM-HOST-DIV-531: LICENSING branch is not selected; see mirror register.
#endif
	/*
	 * Do any host-specific termination bits.
	 */
	host_applClose();
	host_terminate();

	/*
	 * Seppuku.
	 */
	exit(0);
}
