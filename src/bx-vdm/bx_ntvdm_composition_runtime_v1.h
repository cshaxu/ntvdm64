#ifndef BX_NTVDM_COMPOSITION_RUNTIME_V1_H
#define BX_NTVDM_COMPOSITION_RUNTIME_V1_H

#include <stdint.h>

#include "bx_ntvdm_startup_plan_abi.h"
#include "bx_ntvdm_initial_state_abi.h"
#include "bx_ntvdm_initial_state_action_v1.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

enum bx_ntvdm_composition_install_stage_v1 {
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_PROFILE = 1u,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_IMAGES_AND_LAUNCH,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_HOST_CAPABILITY,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_NAMESPACE_INITIALIZE = 40u,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DRIVE_SNAPSHOT,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_VOLUME_SNAPSHOT,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_PROFILE,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_PROFILE,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_CONTEXT_CAPTURE,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_CONTEXT_BIND,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_NAMESPACE_BIND,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_BOOT_DRIVE,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_LAUNCH_PLAN,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMPOSITION_BIND,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_NATIVE_BOP_BIND,
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_INITIAL_STATE,
    /* Appended to preserve the published values of prior diagnostics. */
    BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_STARTUP_CONFIGURATION_BIND
};

#ifdef __cplusplus
extern "C" {
#endif

/* Installs the one process-local whole-package composition from the immutable
 * CLI launch environment.  Returns 1 when bound, 0 when no CLI environment
 * is present, and -1 for a declared-but-invalid composition. */
int bx_ntvdm_composition_runtime_v1_install_from_environment(void);
/* Copied strings are borrowed only for this call; the installed composition
 * retains only its own validated/resource-owned state. */
/* The explicit mode form is the native-engine ABI seam.  It admits only the
 * currently implemented Direct and Readonly modes; Overlay/Virtual are not
 * silently substituted. */
int bx_ntvdm_composition_runtime_v1_install_from_copied_input_with_mode(
    const uint16_t *profile, uint32_t profile_chars,
    const uint16_t *root, uint32_t root_chars,
    const uint16_t *launch, uint32_t launch_chars,
    uint32_t include_mask, uint32_t exclude_mask, uint32_t mutation_mode);
/* Compatibility entry for existing callers: it explicitly selects Direct. */
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
int bx_ntvdm_composition_runtime_v1_copy_initial_state(
    bx_ntvdm_initial_state_v1 *out);
/* Publishes the installed evidence-selected state only as a generic copied
 * mantle action.  It exposes no raw machine object or guest service meaning. */
int bx_ntvdm_composition_runtime_v1_prepare_initial_state_action(
    struct bx_ntvdm_mechanical_action_v1 *action);
/* Produces the full mechanical preentry request from the installed direct
 * composition.  The record contains no Bochs object, host pointer, or BOP
 * identity; it is consumed synchronously by the mantle machine stage. */
int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_request(
    struct bx_ntvdm_machine_stage_v1_request *request);
int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_entry(
    struct bx_ntvdm_machine_stage_v1_entry *entry);
/* Opaque post-stop fact.  It contains neither an OpenNT/BOP identifier nor a
 * guest or host resource and is valid only before the runtime reset. */
int bx_ntvdm_composition_runtime_v1_copy_ordinary_terminal(void);
void bx_ntvdm_composition_runtime_v1_reset(void);

#ifdef __cplusplus
}
#endif

#endif
