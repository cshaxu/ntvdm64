#include <stdio.h>
#include <string.h>

#include "app/bop/config_complete_entry.h"
#include "app/bop/dem_host_session.h"
#include "app/bop/dem_session.h"

static int read_guest(void *state, uint32_t address, uint8_t *bytes, uint32_t count)
{ (void)state; (void)address; (void)bytes; (void)count; return 0; }
static int write_guest(void *state, uint32_t address, const uint8_t *bytes, uint32_t count)
{ (void)state; (void)address; (void)bytes; (void)count; return 0; }

int main(void)
{
    runtime_dem_direct_host_session host;
    runtime_dem_native_session session;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    int ok;
    if (!runtime_dem_direct_host_session_initialize(&host) ||
        !runtime_dem_native_session_initialize(&session,
            runtime_dem_direct_host_session_context(&host), &host,
            read_guest, write_guest) || !runtime_dem_native_session_bind(&session)) return 1;
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event); event.vector = 6u;
    event.execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event.fault_rip = 0x1234u; event.eax = 0u;
    event.window_bytes = 3u; event.window[0] = 0xc4u;
    event.window[1] = 0xc4u; event.window[2] = 0x5eu;
    ok = runtime_config_complete_generic_ud_dispatch(&event, &outcome) &&
        outcome.disposition == RUNTIME_GENERIC_UD_RESUME &&
        outcome.resume_rip == 0x1237u && outcome.gpr16_write_mask == 0u;
    runtime_dem_native_session_unbind(&session);
    runtime_dem_direct_host_session_reset(&host);
    if (!ok) return 2;
    puts("T245 S11 config-complete: original MS_bop_E zero-floppy path resumes +3");
    return 0;
}
