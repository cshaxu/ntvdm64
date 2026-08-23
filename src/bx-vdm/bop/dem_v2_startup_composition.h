#ifndef BX_NTVDM_BOP_DEM_V2_STARTUP_COMPOSITION_H
#define BX_NTVDM_BOP_DEM_V2_STARTUP_COMPOSITION_H

#include <stdint.h>

#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_initial_state_abi.h"
#include "bx_ntvdm_initial_state_action_v1.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

/* Direct v2 startup composition.  These are copied CLI descriptors and
 * mechanical stage records only; no legacy BOP provider/session is exposed. */
int bx_ntvdm_dem_v2_startup_install(const uint16_t *profile, uint32_t profile_chars,
    const uint16_t *root, uint32_t root_chars, const uint16_t *launch,
    uint32_t launch_chars, uint32_t include_mask, uint32_t exclude_mask,
    uint32_t mutation_mode);
void bx_ntvdm_dem_v2_startup_reset(void);
int bx_ntvdm_dem_v2_startup_prepare_machine_stage_request(
    struct bx_ntvdm_machine_stage_v1_request *request);
int bx_ntvdm_dem_v2_startup_prepare_machine_stage_entry(
    struct bx_ntvdm_machine_stage_v1_entry *entry);
int bx_ntvdm_dem_v2_startup_copy_ordinary_terminal(void);
/* Copy the one profile-declared CLI launch input for the directly imported
 * COMMAND host-service plane.  The result is ordinary OEM data; it is never
 * a host pathname, guest pointer, or mantle object. */
int bx_ntvdm_dem_v2_startup_copy_command_source(char *application,
    uint32_t application_capacity, char *tail, uint32_t tail_capacity,
    uint16_t *drive, uint16_t *code_page);
/* Copy the admitted source-built COMMAND.COM pathname in the exact bounded
 * form which OpenNT cmdconf's SHELL= producer can pass to guest sysconf. */
int bx_ntvdm_dem_v2_startup_copy_bootstrap_command(char *command_path,
    uint32_t command_path_capacity);

/* The native engine captures this once at composition admission.  It is
 * adapter-private session data, never a guest or mantle ABI. */
const bx_ntvdm_host_drive_snapshot_v1 *
bx_ntvdm_dem_v2_startup_drive_snapshot(void);

#endif
