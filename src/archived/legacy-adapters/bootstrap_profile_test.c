#include "bootstrap_profile.h"

#include <stdio.h>
#include <string.h>

int wmain(void)
{
    uint8_t ntio[] = { 0x90u, 0xc4u, 0xc4u, 0x50u, 0x11u,
        0x90u, 0xc4u, 0xc4u, 0x50u, 0x11u, 0xf4u };
    uint8_t before[sizeof(ntio)];
    nvtdm_bootstrap_plan plan;

    memcpy(before, ntio, sizeof(ntio));
    if (!nvtdm_plan_opennt_ntio_bootstrap(ntio, sizeof(ntio), &plan) ||
        plan.rewrite_count != 2u || plan.rewrites[0].offset != 1u ||
        plan.rewrites[1].offset != 6u || memcmp(ntio, before, sizeof(ntio)) != 0 ||
        plan.private_image[1] != 0xe6u || plan.private_image[2] != 0xe9u ||
        plan.private_image[6] != 0xe6u || plan.private_image[7] != 0xe9u) return 1;
    nvtdm_bootstrap_plan_destroy(&plan);
    if (nvtdm_plan_opennt_ntio_bootstrap(ntio, 3u, &plan)) return 1;
    wprintf(L"bootstrap-profile-test: manifest-gated private NTIO bootstrap rewrite plan verified\n");
    return 0;
}
