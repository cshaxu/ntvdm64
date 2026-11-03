/*[

bswap.c

LOCAL CHAR SccsID[]="@(#)bswap.c	1.6 11/30/94";

BSWAP CPU functions.
--------------------

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
#include <c_reg.h>
#include <bswap.h>
#include <stdio.h>

/*
   =====================================================================
   EXTERNAL ROUTINES STARTS HERE.
   =====================================================================
 */


#line 35
#ifdef SPC486
#line 36
#error Excluded unsupported SPC486 profile
#endif /* SPC486 */
#line 65
