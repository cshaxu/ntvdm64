#ifndef BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_H
#define BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_H

#include "bx_ntvdm_startup_configuration_provider_v1.h"

/* Installation-time only.  The result contains copied OEM data and never
 * retains a host pathname or handle. */
int bx_ntvdm_startup_configuration_source_v1_from_environment(
    bx_ntvdm_startup_configuration_input_v1 *input,
    const bx_ntvdm_mutation_profile_v1 *profile);

#endif
