#include <stdio.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

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

static void initialize_event(runtime_exception_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event->abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x500u;
}

static int invoke(fixture_context *context, runtime_exception_event *event,
    runtime_cpu_state *cpu, runtime_cpu_result *result,
    runtime_command_misc_session *session)
{
    runtime_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = RUNTIME_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = RUNTIME_COMMAND_MISC_GET_START_INFO;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    call.session = session;
    return runtime_command_misc_invoke(&call) &&
        result->disposition == RUNTIME_CPU_RESULT_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

int main(void)
{
    fixture_context context;
    runtime_exception_event event;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_command_misc_session session;

    memset(&context, 0, sizeof(context));
    initialize_event(&event);
    runtime_cpu_state_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    runtime_command_misc_session_initialize(&session);
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
