/*[

cmpxchg.c

LOCAL CHAR SccsID[]="@(#)cmpxchg.c	1.5 02/09/94";

CMPXCHG CPU functions.
----------------------

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
#include <cmpxchg.h>
#include <cmp.h>


/*
   =====================================================================
   EXTERNAL FUNCTIONS START HERE.
   =====================================================================
 */


#line 36
#ifdef SPC486
#line 37
#error Excluded unsupported SPC486 profile
#endif /* SPC486 */
#line 123
