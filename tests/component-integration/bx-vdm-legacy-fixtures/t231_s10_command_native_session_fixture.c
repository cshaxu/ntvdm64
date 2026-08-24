#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/command_v2_generic_ud_bridge.h"
#include "opennt-host/command/command_native_session_shim.h"

static void event_initialize(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = 0x2800u;
    event->eip = 0x2800u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x54u; event->window[3] = service;
}

int main(void)
{
    bx_ntvdm_command_native_session session;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;

    if (!bx_ntvdm_command_native_session_initialize(&session) ||
        !bx_ntvdm_command_native_session_bind(&session)) return 1;
    event_initialize(&event, 0u);
    memset(&outcome, 0, sizeof(outcome));
    if (!bx_ntvdm_command_v2_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        outcome.resume_rip != 0u) return 2;
    event_initialize(&event, 17u);
    if (bx_ntvdm_command_v2_generic_ud_recognizes(&event) ||
        bx_ntvdm_command_v2_generic_ud_dispatch(&event, &outcome)) return 3;
    bx_ntvdm_command_native_session_unbind(&session);
    event_initialize(&event, 0u);
    if (bx_ntvdm_command_v2_generic_ud_dispatch(&event, &outcome)) return 4;
    puts("T231 v2 COMMAND session owns 54:00 without a v1 fallback");
    return 0;
}