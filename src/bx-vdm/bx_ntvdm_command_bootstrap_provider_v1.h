#ifndef BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_H
#define BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_H

/*
 * COMMAND's bootstrap/environment/config component.  This is deliberately a
 * package boundary, rather than a collection of BOP leaves: cmddisp.c slots
 * 01, 02, 04, 05, 0c, 0d, 0f and 10 share the initial command/session state
 * and must not each select an unrelated host policy.
 */

#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"
#include "bx_ntvdm_cmd_get_next_service.h"
#include "bx_ntvdm_command_boot_input_v1.h"
#include "bx_ntvdm_command_launch_plane_v1.h"

#define BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_MAGIC UINT32_C(0x42584342)
#define BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_VERSION UINT32_C(1)

typedef struct bx_ntvdm_command_bootstrap_provider_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_cmd_comspec_bootstrap_v1 comspec;
    bx_ntvdm_cmd_get_next_state_v1 get_next;
    bx_ntvdm_command_launch_plane_v1 set_info;
    bx_ntvdm_command_boot_input_v1 boot_input;
} bx_ntvdm_command_bootstrap_provider_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_command_bootstrap_provider_v1_initialize(
    bx_ntvdm_command_bootstrap_provider_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_view);
int bx_ntvdm_command_bootstrap_provider_v1_valid(
    const bx_ntvdm_command_bootstrap_provider_v1 *value);
/* Returns nonzero only for the original component's eight callable slots. */
int bx_ntvdm_command_bootstrap_provider_v1_owns_service(uint8_t service);
#ifdef __cplusplus
}
#endif

#endif
