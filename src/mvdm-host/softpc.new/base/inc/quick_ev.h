/*[
************************************************************************
	Name:			quick_ev.h
	Author:			WTG Charnell
	Created:		Unknown
	Sccs ID:		@(#)quick_ev.h	1.20 06/08/95

	(c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.

	Purpose:		Quick Event Manager definitions
	Description:		Definitions for users of the quick
				event manager

************************************************************************
]*/

#include	"host_qev.h"	/* for q_ev_handle typedef */

typedef void (*Q_CALLBACK_FN) IPT1(long, parm);

extern void q_event_init IPT0();
extern void delete_q_event IPT1(q_ev_handle, handle);
extern void dispatch_q_event IPT0();
extern void delete_tic_event IPT1(q_ev_handle, handle);
extern void tic_event_init IPT0();
extern q_ev_handle add_tic_event IPT3(Q_CALLBACK_FN, func, unsigned long, time, long, param);
extern q_ev_handle add_q_event_i IPT3(Q_CALLBACK_FN, func, unsigned long, time, long, param);
extern q_ev_handle add_q_event_t IPT3(Q_CALLBACK_FN, func, unsigned long, time, long, param);

#ifndef NTVDM
/* DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register. */
#endif

#ifndef CPU_40_STYLE
/* DIVERGENCE MVDM-HOST-DIV-514: non-CPU40 branch is not selected for these consumers; see mirror register. */
#else	/* CPU_40_STYLE */
extern IU32 calc_q_inst_for_time IPT1(IU32, time);
extern IU32 calc_q_time_for_inst IPT1(IU32, inst);
#define host_calc_q_ev_inst_for_time      calc_q_inst_for_time
#define host_calc_q_ev_time_for_inst      calc_q_time_for_inst
#endif	/* CPU_40_STYLE */

#ifndef NTVDM
/* DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register. */
#endif  /* !NTVDM */
