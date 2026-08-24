/*  cmdexit.c - Exit related SVC routines
 *
 *  cmdExit
 *
 *  Modification History:
 *
 *  Sudeepb 05-Jul-1991 Created
 */

/* OpenNT source: src/opennt/base/mvdm/dos/command/cmdexit.c.
 * DIVERGENCE(BOP-DIV-012): the historical include closure pulled CCPU/SoftPC product
 * headers; the directly imported original body needs only the existing typed
 * COMMAND stop seam. */
#include "../../opennt-host/command/command_misc_shim.h"

/* cmdExitVDM - Terminate the VDM
 *
 *
 * Entry - None
 *
 * Exit  - None
 *
 *
 *
 */

VOID cmdExitVDM (VOID)
{
    // Kill the VDM process
    TerminateVDM();
}
