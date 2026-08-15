#include "bx_ntvdm_dem_namespace_identity_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static bx_ntvdm_dem_namespace_identity_observation_v1 record;

void bx_ntvdm_dem_namespace_identity_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    memset(&record, 0, sizeof(record));
}

int bx_ntvdm_dem_namespace_identity_observation_v1_copy(
    bx_ntvdm_dem_namespace_identity_observation_v1 *out)
{
    if (out == 0 || !enabled || record.captured == 0u) return 0;
    *out = record;
    return 1;
}

void bx_ntvdm_dem_namespace_identity_observation_v1_consider(uint8_t service,
    int resolved, uint8_t drive_index, uint32_t admitted_mask,
    const bx_ntvdm_readonly_namespace_v1 *startup_namespace, int declared_match,
    uint32_t declared_slot, uint32_t declared_bytes_ready,
    const bx_ntvdm_cpu_result_v2 *result)
{
    if (!enabled || service != 0x12u || result == 0 ||
        !bx_ntvdm_cpu_result_v2_valid(result)) return;
    ++record.seen_open_count;
    if (record.seen_open_count != 2u) return;
    record.captured = 1u;
    record.namespace_attached = startup_namespace != 0 ? 1u : 0u;
    record.namespace_generation = startup_namespace != 0 ? startup_namespace->generation : 0u;
    record.drive_index = drive_index;
    record.declared_slot = declared_slot;
    record.declared_bytes_ready = declared_bytes_ready;
    record.admitted_drive = drive_index < 26u &&
        (admitted_mask & (UINT32_C(1) << drive_index)) != 0u;
    if (!resolved) record.identity_class = BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_INVALID;
    else if (declared_match) record.identity_class = BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_DECLARED;
    else if (record.admitted_drive) record.identity_class =
        BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_OTHER_ADMITTED;
    else record.identity_class = BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_UNADMITTED;
    record.disposition = result->disposition;
    record.gpr16_write_mask = result->cpu_delta.gpr16_write_mask;
    record.eflags_write_mask = result->eflags_write_mask;
    record.eflags_values = result->eflags_values;
    memcpy(record.gpr16_values, result->cpu_delta.gpr16_values,
        sizeof(record.gpr16_values));
}
