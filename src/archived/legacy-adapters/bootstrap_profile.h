#ifndef BOOTSTRAP_PROFILE_H
#define BOOTSTRAP_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "bop_rewrite.h"

#define NVDTM_BOOTSTRAP_MAX_REWRITES 128u

typedef struct nvtdm_bootstrap_plan {
    uint8_t *private_image;
    size_t private_image_bytes;
    bop_rewrite_entry rewrites[NVDTM_BOOTSTRAP_MAX_REWRITES];
    size_t rewrite_count;
} nvtdm_bootstrap_plan;

/* Plans the fixed OpenNT NTIO bootstrap service on a private byte copy only. */
int nvtdm_plan_opennt_ntio_bootstrap(const uint8_t *ntio_bytes,
    size_t ntio_size, nvtdm_bootstrap_plan *plan);
void nvtdm_bootstrap_plan_destroy(nvtdm_bootstrap_plan *plan);

#endif
