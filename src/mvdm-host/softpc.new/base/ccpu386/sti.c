/*[

sti.c

LOCAL CHAR SccsID[]="@(#)sti.c	1.5 02/09/94";

STI CPU functions.
------------------

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
#include <sti.h>
#include "mvdm_softpc_termination.h"


/*
   =====================================================================
   EXTERNAL ROUTINES STARTS HERE.
   =====================================================================
 */


GLOBAL VOID
STI()
   {
   SET_IF(1);
   /* DIVERGENCE(MVDM-HOST-DIV-220): default-off scalar-only witness after
    * the unchanged original STI state update. It is gated by the existing
    * DOS CON boundary and cannot alter CPU flags, PIC, BIOS, or guest flow. */
   mvdm_softpc_record_cpu_interrupt_enable();
   }
