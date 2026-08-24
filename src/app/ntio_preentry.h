#ifndef RUNTIME_NTIO_PREENTRY_V1_H
#define RUNTIME_NTIO_PREENTRY_V1_H

#include "byob_image.h"
#include "adapter-softpc/finite_run.h"
#include "preentry_input.h"

/* Adapter-owned conversion of an already identity-checked NTIO image into
 * the mantle's opaque v2 request.  It owns the source-derived placement and
 * entry contract; the mantle sees no NTIO or VDM vocabulary. */
int runtime_ntio_preentry_v1_prepare(const byob_image *ntio,
    const byob_component_descriptor *identity,
    const struct runtime_preentry_input_v1 *preentry_input,
    runtime_finite_run_request *request, Bit64u tick_budget, Bit32u ips);

#endif
