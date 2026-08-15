#include "bootstrap_profile.h"

#include "transition_registry.h"

#include <stdlib.h>
#include <string.h>

int nvtdm_plan_opennt_ntio_bootstrap(const uint8_t *ntio_bytes,
    size_t ntio_size, nvtdm_bootstrap_plan *plan)
{
    static const uint8_t dem_load_dos[] = { 0xc4u, 0xc4u, 0x50u, 0x11u };
    transition_registry registry;

    if (ntio_bytes == NULL || ntio_size == 0u || plan == NULL) return 0;
    memset(plan, 0, sizeof(*plan));
    plan->private_image = (uint8_t *)malloc(ntio_size);
    if (plan->private_image == NULL) return 0;
    memcpy(plan->private_image, ntio_bytes, ntio_size);
    plan->private_image_bytes = ntio_size;
    transition_registry_init(&registry);
    if (!transition_registry_register(&registry, dem_load_dos,
        sizeof(dem_load_dos), 0xe9u) || !transition_registry_freeze(&registry) ||
        !transition_registry_apply(&registry, plan->private_image, ntio_size,
        plan->rewrites, NVDTM_BOOTSTRAP_MAX_REWRITES, &plan->rewrite_count) ||
        plan->rewrite_count == 0u) {
        nvtdm_bootstrap_plan_destroy(plan);
        return 0;
    }
    return 1;
}

void nvtdm_bootstrap_plan_destroy(nvtdm_bootstrap_plan *plan)
{
    if (plan == NULL) return;
    free(plan->private_image);
    memset(plan, 0, sizeof(*plan));
}
