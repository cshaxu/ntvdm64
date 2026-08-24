/* Direct source mirror of src/opennt/base/mvdm/xms.486/xmsa20.c.
 * DIVERGENCE(BOP-DIV-027): historical xms/SoftPC/SAS/private-NT headers are replaced only
 * by the fixed bx-vdm compatibility surface in xms_shim.h.  Service order
 * and original function bodies remain below; each later code divergence is
 * documented at its exact point. */
#include "../../opennt-host/xms/xms_shim.h"

/* xmsa20.c - A20 related XMS routines
 *
 * XMSA20
 *
 * Modification History:
 *
 * Sudeepb 15-May-1991 Created
 */

void sas_enable_20_bit_wrapping(void);
void sas_disable_20_bit_wrapping(void);
BOOL sas_twenty_bit_wrapping_enabled(void);


/* xmsA20 - Handle A20 requests
 *
 *
 * Entry - Client (AX) 0 - Disable A20
 *		       1 - Enable A20
 *		       2 - Query
 *
 * Exit
 *	   SUCCESS
 *	     Client (AX) = 1
 *	     if on entry AX=2 Then
 *		Cleint (AX) =1 means was enabled
 *		Cleint (AX) =0 means was disabled
 *
 *	   FAILURE
 *	     Client (AX) = 0
 */

VOID xmsA20 (VOID)
{
    int reason;

    reason = getAX();

    setAX(1);

    if (reason == 0)
	xmsEnableA20Wrapping();
    else if (reason == 1)
	    xmsDisableA20Wrapping();
	 else if (reason == 2) {
		if (sas_twenty_bit_wrapping_enabled())
		    setAX(0);
		setBL(0);
	      }
	      else
		setAX(0);
}
// function to enable 1MB wrapping (turn off A20 line)
VOID xmsEnableA20Wrapping(VOID)
{
    sas_enable_20_bit_wrapping();
    /* DIVERGENCE (T237 S5): original code dereferenced its saved host VDM
     * pointer here.  The shim publishes through a checked guest-byte seam. */
    bx_ntvdm_xms_write_himem_a20_state(0);

#if 0 // this is not necessay because the intel space(pointed by
      // HimemA20State) doesn't contain instruction
      // doesn't contain instruction
#ifdef MIPS
	Sim32FlushVDMPointer
	 (
	  (((ULONG)pHimemA20State >> 4) << 16) | ((ULONG)pHimemA20State & 0xF),
	  1,
	  pHimemA20State,
	  FALSE
	 );

#endif
#endif

}

// function to disable 1MB wrapping(turn on A20 line)
VOID xmsDisableA20Wrapping(VOID)
{

    sas_disable_20_bit_wrapping();
    /* DIVERGENCE (T237 S5): see xmsEnableA20Wrapping. */
    bx_ntvdm_xms_write_himem_a20_state(1);
#if 0 // this is not necessay because the intel space(pointed by
      // HimemA20State) doesn't contain instruction
      // doesn't contain instruction
#ifdef MIPS
	Sim32FlushVDMPointer
	 (
	  (((ULONG)pHimemA20State >> 4) << 16) | ((ULONG)pHimemA20State & 0xF),
	  1,
	  pHimemA20State,
	  FALSE
	 );

#endif
#endif

}
