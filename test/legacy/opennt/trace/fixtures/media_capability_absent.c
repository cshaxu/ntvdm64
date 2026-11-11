/*
 * Trace-only no-media policy. This translation unit is admitted solely by
 * explicitly named bounded fixtures, never by a runtime/library target.
 * It intentionally does not enumerate or open A:/B:. A future runtime policy
 * must distinguish auto, disabled, host-drive, and image-backed media; auto
 * must accept every accessible Windows A:/B: DOS volume.
 */
#include "insignia.h"

SHORT host_gfi_rdiskette_active(UTINY host_id, BOOL active, CHAR *err) {
    (void)host_id;
    (void)err;

    return active ? -1 : 0; /* C_CONFIG_NOT_VALID / C_CONFIG_OP_OK */
}
