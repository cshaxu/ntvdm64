#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/config_complete_v2_generic_ud_bridge.h"
#include "opennt-host/dem/dem_direct_host_session.h"
#include "opennt-host/dem/dem_native_session_shim.h"
#include "opennt-host/dem/dem_drive_policy_shim.h"

static int read_guest(void *state, uint32_t address, uint8_t *bytes, uint32_t count)
{ (void)state; (void)address; (void)bytes; (void)count; return 0; }
static int write_guest(void *state, uint32_t address, const uint8_t *bytes, uint32_t count)
{ (void)state; (void)address; (void)bytes; (void)count; return 0; }

int main(void)
{
    bx_ntvdm_dem_direct_host_session host;
    bx_ntvdm_dem_native_session session;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    uint8_t types[26] = {0};
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    int ok;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(0u, types, 0u, 0u, &drives) ||
        !bx_ntvdm_demdasd_drive_policy_bind(&drives) ||
        !bx_ntvdm_dem_direct_host_session_initialize(&host) ||
        !bx_ntvdm_dem_native_session_initialize(&session,
            bx_ntvdm_dem_direct_host_session_context(&host), &host,
            read_guest, write_guest) || !bx_ntvdm_dem_native_session_bind(&session)) return 1;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event); event.vector = 6u;
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event.fault_rip = 0x1234u; event.eax = 0u;
    event.window_bytes = 3u; event.window[0] = 0xc4u;
    event.window[1] = 0xc4u; event.window[2] = 0x5eu;
    ok = bx_ntvdm_config_complete_v2_generic_ud_dispatch(&event, &outcome) &&
        outcome.disposition == BX_NTVDM_GENERIC_UD_RESUME &&
        outcome.resume_rip == 0x1237u && outcome.gpr16_write_mask == 0u;
    bx_ntvdm_dem_native_session_unbind(&session);
    bx_ntvdm_dem_direct_host_session_reset(&host);
    bx_ntvdm_demdasd_drive_policy_reset();
    if (!ok) return 2;
    puts("T245 S11 config-complete: original MS_bop_E zero-floppy path resumes +3");
    return 0;
}