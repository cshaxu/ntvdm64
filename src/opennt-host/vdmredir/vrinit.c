/*
 * Re-rooted from OpenNT base/mvdm/vdmredir/vrinit.c.
 *
 * The admitted single-session initialization retains the original owner gate
 * and named-pipe cleanup order.  NetBIOS/DLC/VDD/ICA product hooks remain
 * explicit later-provider dependencies; this file does not fabricate them.
 */
#include "vrnmpipe_compat.h"

static BOOLEAN IsVrInitialized = FALSE;

BOOLEAN VrInitialized(VOID)
{
    return IsVrInitialized;
}

BOOLEAN VrInitialize(VOID)
{
    IsVrInitialized = TRUE;
    return TRUE;
}

VOID VrUninitialize(VOID)
{
    VrTerminateNamedPipes();
    IsVrInitialized = FALSE;
}
