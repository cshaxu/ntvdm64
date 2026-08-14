#include "bx_ntvdm_native_bop_composition_v1.h"

/* The boot-namespace composition fixture deliberately has no mantle-backed
 * native root.  Its bridge-order regression needs the ordinary unbound-root
 * decline, not a second machine lifecycle. */
int bx_ntvdm_native_bop_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void)event;
    (void)outcome;
    return 0;
}
