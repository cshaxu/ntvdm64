#include <stdio.h>
#include <string.h>

#include "opennt-host/command/command_misc_shim.h"

typedef struct fixture_context { uint8_t guest[0x10000]; } fixture_context;

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
    bx_ntvdm_command_misc_session *session)
{
    bx_ntvdm_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = BX_NTVDM_COMMAND_MISC_GET_START_INFO;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    call.session = session;
    return bx_ntvdm_command_misc_invoke(&call) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

int main(void)
{
    fixture_context context;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_command_misc_session session;

    memset(&context, 0, sizeof(context));
    initialize_event(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_command_misc_session_initialize(&session);
    cpu.eax = 0x5500u;

    session.dos_session_id = 0u;
    if (!invoke(&context, &event, &cpu, &result, &session) ||
        result.cpu_delta.gpr16_values[0] != 0x5500u) return 1;

    session.dos_session_id = 1u;
    if (!invoke(&context, &event, &cpu, &result, &session) ||
        result.cpu_delta.gpr16_values[0] != 0x5501u) return 2;

    puts("T231 S5 direct OpenNT cmdGetStartInfo: DosSessionId to AL verified");
    return 0;
}
