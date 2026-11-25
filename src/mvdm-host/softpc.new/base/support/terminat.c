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
#line 33
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.
#endif
#line 35
#ifdef GISP_SVGA
#line 36
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 39
#ifdef LICENSING
#line 40
#error DIVERGENCE MVDM-HOST-DIV-531: LICENSING branch is not selected; see mirror register.
#endif
#line 42
#include "emm.h"
#include "sndblst.h"

#ifdef SEGMENTATION
#line 46
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 53

IMPORT VOID host_applClose IPT0();
IMPORT VOID host_terminate IPT0();
IMPORT VOID SbTerminate IPT0();

#line 57
void terminate()
{
	SAVED BOOL already_called_terminate = FALSE;
	UTINY i;

	IMPORT VOID ntvdm_worker_lifecycle_authorize_exit IPT0();
	ntvdm_worker_lifecycle_authorize_exit();
#line 62
	if (already_called_terminate)
	{
		assert0( NO, "Error: terminate called twice - exiting" );
		exit(0);
	}
	else
		already_called_terminate = TRUE;

#ifdef MSWDVR
#line 71
#error Unsupported excluded original configuration
#endif
#line 73

#ifdef SWIN_SNDBLST_NULL
#line 75
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_SNDBLST_NULL platform branch is not selected; see mirror register.
#else
        SbTerminate();
#endif /* SWIN_SNDBLST_NULL */
#line 79

#ifdef GISP_SVGA
#line 81
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 85

	/* terminate COM and LPT devices */
#ifdef  PC_CONFIG
#line 88
#error DIVERGENCE MVDM-HOST-DIV-531: PC_CONFIG branch is not selected; see mirror register.
#else
#line 92
	for (i = 0 ; i < NUM_PARALLEL_PORTS; i++)
                config_activate((IU8)(C_LPT1_NAME + i), FALSE);

	for (i = 0 ; i < NUM_SERIAL_PORTS; i++)
                config_activate((IU8)(C_COM1_NAME + i), FALSE);
#endif
#line 98

	/* Update the cmos.ram file */
	cmos_update();

	host_fdisk_term();

	gvi_term();	/* close down the video adaptor */

#line 106
#ifndef NTVDM
#line 107
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 109

#ifdef LIM
	host_deinitialise_EM(); /* free memory or file used by EM */
#endif

	config_activate(C_FLOPPY_A_DEVICE, FALSE);
#ifdef FLOPPY_B
#line 116
#error Unsupported excluded original configuration
#endif /* FLOPPY_B */
#line 118
#ifdef SLAVEPC
#line 119
#error DIVERGENCE MVDM-HOST-DIV-528: SLAVEPC platform branch is not selected; see mirror register.
#endif /* SLAVEPC */
#line 121

	/*
	 * Do any cpu-specific termination bits.
	 */
#ifdef CPU_30_STYLE
#line 126
#error Excluded original CPU_30_STYLE branch
#endif
#line 128

#ifdef NOVELL
#line 130
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.
#endif
#line 132

#ifdef LICENSING
#line 134
#error DIVERGENCE MVDM-HOST-DIV-531: LICENSING branch is not selected; see mirror register.
#endif
#line 136
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
