#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

typedef struct fixture_context { uint8_t guest[0x20000]; } fixture_context;

static int guest_read(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes);
    return 1;
}

static int guest_write(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes);
    return 1;
}

static void initialize_event(bx_ntvdm_exception_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x500u;
}

static int invoke(fixture_context *context, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result,
    bx_ntvdm_command_misc_session *session, uint32_t first_call)
{
    bx_ntvdm_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = BX_NTVDM_COMMAND_MISC_GET_INIT_ENVIRONMENT;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    call.session = session;
    call.first_call = first_call;
    return bx_ntvdm_command_misc_invoke(&call) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME;
}

static int multisz_contains(const CHAR *strings, uint32_t bytes, const CHAR *needle)
{
    const CHAR *end = strings + bytes;
    while (strings < end && *strings != '\0') {
        if (strcmp(strings, needle) == 0) return 1;
        strings += strlen(strings) + 1u;
    }
    return 0;
}

static int multisz_has_prefix(const CHAR *strings, uint32_t bytes, const CHAR *prefix)
{
    const CHAR *end = strings + bytes;
    size_t prefix_bytes = strlen(prefix);
    while (strings < end && *strings != '\0') {
        if (strncmp(strings, prefix, prefix_bytes) == 0) return 1;
        strings += strlen(strings) + 1u;
    }
    return 0;
}

int main(void)
{
    fixture_context context;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_command_misc_session session;
    CHAR command[] = "COMSPEC=C:\\NTDOS64\\COMMAND.COM";
    USHORT required;

    memset(&context, 0, sizeof(context));
    initialize_event(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_command_misc_session_initialize(&session);
    memcpy(session.comspec, command, sizeof(command));
    session.comspec_bytes = (USHORT)sizeof(command);
    cpu.es = 0x100u;
    cpu.ebx = 1u;

    if (!invoke(&context, &event, &cpu, &result, &session, 1u) ||
        result.cpu_delta.gpr16_values[3] <= 1u) return 1;
    required = result.cpu_delta.gpr16_values[3];

    memset(context.guest + 0x1000u, 0xcc, 0x4000u);
    cpu.ebx = required;
    if (!invoke(&context, &event, &cpu, &result, &session, 1u) ||
        result.cpu_delta.gpr16_values[3] != required ||
        !multisz_contains((CHAR *)context.guest + 0x1000u,
            (uint32_t)required << 4, command) ||
        multisz_has_prefix((CHAR *)context.guest + 0x1000u,
            (uint32_t)required << 4, "WINDIR=")) return 2;

    cpu.ebx = 0x10u;
    memset(context.guest + 0x1000u, 0xcc, 0x100u);
    if (!invoke(&context, &event, &cpu, &result, &session, 0u) ||
        result.cpu_delta.gpr16_values[3] != 0u || context.guest[0x1000u] != 0xccu) return 3;

    free(lpszzVDMEnv32); lpszzVDMEnv32 = NULL; cchVDMEnv32 = 0u;
    puts("T231 S5 direct OpenNT cmdGetInitEnvironment sizing, filtering, and no-op paths verified");
    return 0;
}
