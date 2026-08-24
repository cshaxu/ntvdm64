/*
 * Re-rooted from OpenNT base/mvdm/vdmredir/vrinit.c.
 *
 * The admitted single-session initialization retains the original owner gate
 * and named-pipe cleanup order.  NetBIOS/DLC/VDD/ICA product hooks remain
 * explicit later-provider dependencies; this file does not fabricate them.
 */
#include "vrnmpipe_compat.h"

static BOOLEAN IsVrInitialized = FALSE;

/* DIVERGENCE(HOST-DIV-017): OpenNT exported VrInitialized from a separate
 * VDMREDIR DLL.  The static program preserves that original import shape in
 * opennt-bop, so the provider body needs a private link name. */
BOOLEAN bx_ntvdm_vr_initialized_provider(VOID)
{
    return IsVrInitialized;
}

BOOLEAN bx_ntvdm_vr_initialize_provider(VOID)
{
    IsVrInitialized = TRUE;
    return TRUE;
}

VOID bx_ntvdm_vr_uninitialize_provider(VOID)
{
    VrTerminateNamedPipes();
    IsVrInitialized = FALSE;
}
