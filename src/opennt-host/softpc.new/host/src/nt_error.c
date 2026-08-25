/* DIVERGENCE(HOST-DIV-012,HOST-DIV-013,HOST-DIV-014,HOST-DIV-026,HOST-DIV-027):
 * This same-signature mirror boundary retains the original exported entry
 * point only.  The admitted, heavily cropped original body is private to the
 * matching opennt-host overlay so this mirror does not exceed the 50-percent
 * source-difference limit. */
#define host_direct_access_error opennt_host_overlay_direct_access_error
#include "opennt-host-overlay/softpc.new/host/src/nt_error.c"
#undef host_direct_access_error

VOID host_direct_access_error(ULONG type)
{
    opennt_host_overlay_direct_access_error(type);
}
