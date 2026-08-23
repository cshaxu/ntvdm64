/* Direct source mirror of src/opennt/base/mvdm/xms.486/xmsdisp.c.
 * DIVERGENCE: historical xms/SoftPC/SAS/private-NT headers are replaced only
 * by the fixed bx-vdm compatibility surface in xms_shim.h.  Service order
 * and original function bodies remain below; each later code divergence is
 * documented at its exact point. */
#include "../../shim/xms_shim.h"

/*
 *  xmsdisp.c - SVC dispatch module for XMS
 *
 *  Modification History:
 *
 *  Sudeepb 15-May-1991 Created
 *
 *  williamh 25-Sept-1992 Added UMB support
 */

#include <stdio.h>
PFNSVC	apfnXMSSvc [] = {
     xmsA20,		    // XMS_A20
     xmsMoveBlock,	    // XMS_MOVEBLOCK
     xmsAllocBlock,	    // XMS_ALLOCBLOCK
     xmsFreeBlock,	    // XMS_FREEBLOCK
     xmsSysPageSize,	    // XMS_SYSTEMPAGESIZE
     xmsQueryExtMem,	    // XMS_EXTMEM
     xmsInitUMB,	    // XMS_INITUMB
     xmsRequestUMB,	    // XMS_REQUESTUMB
     xmsReleaseUMB,         // XMS_RELEASEUMB
     xmsNotifyHookI15,      // XMS_NOTIFYHOOKI15
     xmsQueryFreeExtMem,    // XMS_QUERYEXTMEM
     xmsReallocBlock        // XMS_REALLOCBLOCK
};

/* XMSDispatch - Dispatch SVC call to right handler.
 *
 * Entry - iSvc (SVC byte following SVCop)
 *
 * Exit  - None
 *
 * Note  - Some mechanism has to be worked out to let the emulator know
 *	   about DOSKRNL code segment and size. Using these it will figure
 *	   out whether SVCop (hlt for the moment) has to be passed to
 *	   DEM or to be handled as normal invalid opcode.
 */

BOOL XMSDispatch (ULONG iSvc)
{

#if DBG

    if (iSvc >= XMS_LASTSVC){
	printf("XMS:Unimplemented SVC index %x\n",iSvc);
	setCF(1);
	return FALSE;
    }

#endif

    (apfnXMSSvc [iSvc])();
    return TRUE;
}
