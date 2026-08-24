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
    /* DIVERGENCE(HOST-DIV-022): OpenNT VrInitialize registers VDD hooks,
     * starts NetBIOS/DLC state, initializes ICA queues and writes VDM load
     * info. Those coupled product services are not a helper-level recovery;
     * this retained static single-session gate exposes no such fabricated
     * capability until the later Redirector/VDD lifecycle owner exists. */
    IsVrInitialized = TRUE;
    return TRUE;
}

VOID bx_ntvdm_vr_uninitialize_provider(VOID)
{
    VrTerminateNamedPipes();
    IsVrInitialized = FALSE;
}
