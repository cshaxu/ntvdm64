#ifndef BX_NTVDM_NTIO_PREENTRY_V1_H
#define BX_NTVDM_NTIO_PREENTRY_V1_H

#include "byob_image.h"
#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "bx_ntvdm_preentry_input_v1.h"

/* Adapter-owned conversion of an already identity-checked NTIO image into
 * the mantle's opaque v2 request.  It owns the source-derived placement and
 * entry contract; the mantle sees no NTIO or VDM vocabulary. */
int bx_ntvdm_ntio_preentry_v1_prepare(const byob_image *ntio,
    const byob_component_descriptor *identity,
    const struct bx_ntvdm_preentry_input_v1 *preentry_input,
    bx_ntvdm_finite_run_request *request, Bit64u tick_budget, Bit32u ips);

#endif
