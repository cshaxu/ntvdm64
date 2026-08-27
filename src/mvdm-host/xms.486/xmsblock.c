/* xmsblock.c - XMS Extended block related routines
 *
 * xmsAllocBlock
 * xmsFreeBlock
 * xmsReallocBlock
 * xmsMoveBlock
 * xmsQueryExtMem
 *
 * Modification History:
 *
 * Sudeepb 15-May-1991 Created
 */

#include "xms.h"
#include <memory.h>
#include <string.h>
#include <xmssvc.h>
#include <softpc.h>
#include <mvdm.h>
#include "mvdm-host-overlay/xms.486/xms_block_move.h"


/* xmsAllocBlock - Commit Memory for an EMB.
 *
 *
 * Entry - DX    - Size in K to allocate 
 *
 * Exit
 *	   SUCCESS
 *	     Client (AX) - Start address of the EMB (in K)
 *
 *	   FAILURE
 *	     Client (AX) = 0
 */

VOID xmsAllocBlock (VOID)
{
BOOL Success;
ULONG BaseAddress;
ULONG size;

    size = getDX() * 1024;
    if(size) {

        //
        // Ask for a chunk of memory
        //
        Success = SAAllocate(
            ExtMemSA,
            size,
            &BaseAddress
            );
            
        if (!Success) {
            DbgPrint("xmsAllocBlock:SAAlloc failed !!!!\n");
            setAX(0);
            return;
        }
    }

    ASSERT((USHORT)(BaseAddress / 1024) < 65535);
    setAX((USHORT)(BaseAddress / 1024));
    return;
}

/* xmsFreeBlock - Free Memory for an EMB.
 *
 *
 * Entry - AX	 - Start address of the EMB (in K)
 *	   DX	 - Size in K to free 
 *
 * Exit
 *	   SUCCESS
 *	     Client (AX) = 1
 *
 *	   FAILURE
 *	     Client (AX) = 0
 */

VOID xmsFreeBlock (VOID)
{
BOOL Success;
ULONG BaseAddress;
ULONG size;

    BaseAddress = (getAX() * 1024);
    size = getDX() * 1024;

    Success = SAFree(
        ExtMemSA,
        size,
        BaseAddress
        );
        
    if (!Success) {
        DbgPrint("xmsFreeBlock:SAFree failed !!!!");
        setAX(0);
        return;
    }

    setAX(1);
    return;
}

/* xmsReallocBlock - Change the size of an EMB.
 *
 *
 * Entry - AX       - Start address of the EMB (in K)
 *	   DX       - Original Size in K
 *         BX       - New size in K
 *
 * Exit
 *	   SUCCESS
 *	     Client (CX) = New base of block
 *
 *	   FAILURE
 *	     Client (AX) = 0
 */

VOID xmsReallocBlock (VOID)
{
BOOL Success;
ULONG BaseAddress;
ULONG NewAddress;
ULONG size;
ULONG NewSize;

    size = getDX() * 1024;
    NewSize = getBX() * 1024;
    BaseAddress = getAX() * 1024;
    if(size != NewSize) {

        //
        // Realloc the chunk of memory
        //
        Success = SAReallocate(
            ExtMemSA,
            size,
            BaseAddress,
            NewSize,
            &NewAddress
            );
            
        if (!Success) {
            DbgPrint("xmsReallocBlock:SARealloc failed !!!!\n");
            setCX(0);
            return;
        }
    }

    ASSERT((NewAddress / 1024) < 65535);
    setCX((USHORT)(NewAddress / 1024));
    return;
}

/* xmsMoveBlock - Process Move Block Functions
 *
 *
 * Entry - Client (SS:BP) Pointer to Ext. Memory Move Structure
 *	    SS:BP-4	= DWORD Transfer Count in words (guaranteed even)
 *	    SS:BP-8	= DWORD Src Linear Address
 *	    SS:BP-12	= DWORD Dst Linear Address
 *
 * Exit
 *	   SUCCESS
 *	     Client (AX) = 1
 *
 *	   FAILURE
 *	     Client (AX) = 0
 *	     Client (BL) = error code
 *
 *  NOTE: For Overlapping regions XMS spec says "If the Source and
 *	  Destination blocks overlap, only forward moves (i.e. where
 *	  the destination base is less than the source base) are
 *	  guaranteed to work properly"
 */

VOID xmsMoveBlock (VOID)
{
    /* DIVERGENCE MVDM-HOST-DIV-011: the source body retains SS:BP and two
       DWORD linear operands through persistent host aliases.  The matching
       private overlay reads the same twelve bytes under one bounded lease and
       invokes the original-shaped numeric xmsMoveMemory seam. */
    if (!mvdm_xms_move_block_from_stack()) {
        setAX(0);
        return;
    }
    setAX(1);
    return;
}

/* xmsQueryExtMem - Process query extended memory
 *
 *
 * Entry - None
 *
 * Exit
 *	   SUCCESS
 *	     Client (AX) = Largest Free Block in K
 *	     Client (DX) = Free Memory in K
 *
 *	   FAILURE
 *	     Client (AX) = 0
 *	     Client (DX) = 0
 *
 */
VOID xmsQueryFreeExtMem(VOID)
{
    ULONG LargestFree = 0;
    ULONG TotalFree = 0;
    
    //
    // Find out how much memory remains
    //
    SAQueryFree(
        ExtMemSA,
        &TotalFree,
        &LargestFree
        );
        
    ASSERT((TotalFree / 1024) < 65534);
    setAX((USHORT)(TotalFree / 1024));
    ASSERT((LargestFree / 1024) < 65534);
    setDX((USHORT)(LargestFree / 1024));

}
