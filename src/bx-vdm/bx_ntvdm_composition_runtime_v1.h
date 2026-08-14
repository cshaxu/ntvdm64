#ifndef BX_NTVDM_COMPOSITION_RUNTIME_V1_H
#define BX_NTVDM_COMPOSITION_RUNTIME_V1_H

#include <stdint.h>

#include "bx_ntvdm_startup_plan_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Installs the one process-local whole-package composition from the immutable
 * CLI launch environment.  Returns 1 when bound, 0 when no CLI environment
 * is present, and -1 for a declared-but-invalid composition. */
int bx_ntvdm_composition_runtime_v1_install_from_environment(void);
/* Copied strings are borrowed only for this call; the installed composition
 * retains only its own validated/resource-owned state. */
int bx_ntvdm_composition_runtime_v1_install_from_copied_input(
    const uint16_t *profile, uint32_t profile_chars,
    const uint16_t *root, uint32_t root_chars,
    const uint16_t *launch, uint32_t launch_chars,
    uint32_t include_mask, uint32_t exclude_mask);

/* Produces the source-derived NTIO payload write and real-mode entry from the
 * currently installed direct composition.  Payload remains owned by that
 * composition and is invalid immediately after reset. */
int bx_ntvdm_composition_runtime_v1_prepare_startup_plan(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);
void bx_ntvdm_composition_runtime_v1_reset(void);

#ifdef __cplusplus
}
#endif

#endif
