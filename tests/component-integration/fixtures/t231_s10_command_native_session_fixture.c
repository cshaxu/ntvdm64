#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/command_generic_ud_bridge.h"
#include "opennt-bop/ingress/command_native_session.h"

static void event_initialize(struct runtime_generic_ud_event *event,
    uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip = 0x2800u;
    event->eip = 0x2800u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x54u; event->window[3] = service;
}

int main(void)
{
    runtime_command_native_session session;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;

    if (!runtime_command_native_session_initialize(&session) ||
        !runtime_command_native_session_bind(&session)) return 1;
    event_initialize(&event, 0u);
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_command_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_STOP ||
        outcome.resume_rip != 0u) return 2;
    event_initialize(&event, 17u);
    if (runtime_command_generic_ud_recognizes(&event) ||
        runtime_command_generic_ud_dispatch(&event, &outcome)) return 3;
    runtime_command_native_session_unbind(&session);
    event_initialize(&event, 0u);
    if (runtime_command_generic_ud_dispatch(&event, &outcome)) return 4;
    puts("T231 v2 COMMAND session owns 54:00 without a v1 fallback");
    return 0;
}
