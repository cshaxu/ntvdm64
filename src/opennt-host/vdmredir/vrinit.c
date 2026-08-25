/* DIVERGENCE(HOST-DIV-017,HOST-DIV-022): this same-signature mirror boundary
 * retains the static-provider entry only.  Its reached original subset is
 * private overlay code because the crop exceeds the mirror threshold. */
#define runtime_vr_initialized_provider opennt_host_overlay_vr_initialized_provider
#include "opennt-host-overlay/vdmredir/vrinit.c"
#undef runtime_vr_initialized_provider

BOOLEAN runtime_vr_initialized_provider(VOID)
{
    return opennt_host_overlay_vr_initialized_provider();
}
