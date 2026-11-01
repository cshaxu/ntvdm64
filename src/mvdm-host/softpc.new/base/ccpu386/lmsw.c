/*[

lmsw.c

LOCAL CHAR SccsID[]="@(#)lmsw.c	1.5 02/09/94";

LMSW CPU functions.
-------------------

]*/


#include <insignia.h>

#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_addr.h>
#include <c_bsic.h>
#include <c_prot.h>
#include <c_seg.h>
#include <c_stack.h>
#include <c_xcptn.h>
#include	<c_reg.h>
#include <lmsw.h>
#include "mvdm_softpc_termination.h"


/*
   =====================================================================
   EXTERNAL ROUTINES STARTS HERE.
   =====================================================================
 */


GLOBAL VOID
LMSW
       	          
IFN1(
	IU32, op1	/* src operand */
    )


   {
   IU32 temp;
   IU32 no_clear = 0xfffffff1;  /* can't clear top 28-bits or PE */
   IU32 no_set   = 0xfffffff0;  /* can't set top 28-bits */

   /* kill off bits which can not be set */
   op1 = op1 & ~no_set;

   /* DIVERGENCE(MVDM-HOST-DIV-327): S4's default-off Windows 3.1 Setup
    * witness records the operand at the original mode-switch boundary.  It
    * does not select the transition or alter CCPU state. */
   mvdm_softpc_report_setup_mode_switch("lmsw-before", op1,
       GET_CR(CR_STAT));

   /* retain bits which can not be cleared */
   temp = GET_CR(CR_STAT) & no_clear;

   /* thus update only the bits allowed */
   SET_CR(CR_STAT, temp | op1);

   mvdm_softpc_report_setup_mode_switch("lmsw-after", op1,
       GET_CR(CR_STAT));
   }
