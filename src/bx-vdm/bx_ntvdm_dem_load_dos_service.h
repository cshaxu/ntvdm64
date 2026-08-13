#ifndef BX_NTVDM_DEM_LOAD_DOS_SERVICE_H
#define BX_NTVDM_DEM_LOAD_DOS_SERVICE_H

#include "byob_image.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

/* Source-derived replacement for the unavailable demLoadDos composition.
 * It owns no host lookup: the caller supplies an already identity-checked,
 * same-island NTDOS image selected by the profile. */
int bx_ntvdm_dem_load_dos_service_v1_prepare(
    const byob_image *ntdos, const byob_component_descriptor *identity,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, uint32_t action_id,
    struct bx_ntvdm_mechanical_action_v1 *action,
    bx_ntvdm_cpu_result_v2 *result);

#endif
