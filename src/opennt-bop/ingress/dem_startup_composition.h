#ifndef RUNTIME_BOP_DEM_STARTUP_COMPOSITION_H
#define RUNTIME_BOP_DEM_STARTUP_COMPOSITION_H

#include <stdint.h>

#include "app/initial_state_abi.h"
#include "app/initial_state_action.h"
#include "adapter-softpc/machine_stage.h"

/* Direct startup composition.  These are copied CLI descriptors and
 * mechanical stage records only; no legacy BOP provider/session is exposed. */
int runtime_dem_startup_install(const uint16_t *profile, uint32_t profile_chars,
    const uint16_t *root, uint32_t root_chars, const uint16_t *launch,
    uint32_t launch_chars, uint32_t mutation_mode);
void runtime_dem_startup_reset(void);
int runtime_dem_startup_prepare_machine_stage_request(
    struct runtime_machine_stage_request *request);
int runtime_dem_startup_prepare_machine_stage_entry(
    struct runtime_machine_stage_entry *entry);
int runtime_dem_startup_copy_ordinary_terminal(void);
/* Copy the one profile-declared CLI launch input for the directly imported
 * COMMAND host-service plane.  The result is ordinary OEM data; it is never
 * a host pathname, guest pointer, or machine object. */
int runtime_dem_startup_copy_command_source(char *application,
    uint32_t application_capacity, char *tail, uint32_t tail_capacity,
    uint16_t *drive, uint16_t *code_page);
/* Copy the admitted source-built COMMAND.COM pathname in the exact bounded
 * form which OpenNT cmdconf's SHELL= producer can pass to guest sysconf. */
int runtime_dem_startup_copy_bootstrap_command(char *command_path,
    uint32_t command_path_capacity);

#endif
