#include <stdio.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

typedef struct fixture_context {
    uint8_t guest[0x20000];
} fixture_context;

static int guest_read(void *state, uint32_t address, uint8_t *buffer,
    uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes);
    return 1;
}

static int guest_write(void *state, uint32_t address, const uint8_t *buffer,
    uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes);
    return 1;
}

static void initialize_event(runtime_exception_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event->abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x1000u;
}

static int invoke(fixture_context *context, runtime_exception_event_v1 *event,
    runtime_cpu_state_v1 *cpu, runtime_cpu_result_v2 *result,
    uint32_t service, uint32_t first_call)
{
    runtime_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = RUNTIME_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = service;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    call.first_call = first_call;
    return runtime_command_misc_invoke(&call) &&
        result->disposition == RUNTIME_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

int main(void)
{
    fixture_context context;
    runtime_exception_event_v1 event;
    runtime_cpu_state_v1 cpu;
    runtime_cpu_result_v2 result;
    uint32_t address = 0x1000u;

    memset(&context, 0, sizeof(context));
    initialize_event(&event);
    runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u; cpu.edx = 0u; cpu.eax = 0xab00u;
    memcpy(context.guest + address, "C:\\COMMAND.COM", 14u);
    if (!invoke(&context, &event, &cpu, &result,
            RUNTIME_COMMAND_MISC_COMSPEC, 1u) ||
        strcmp(lpszComSpec, "COMSPEC=C:\\COMMAND.COM") != 0 ||
        cbComSpec != strlen(lpszComSpec) + 1u ||
        result.cpu_delta.gpr16_values[0] != 0xab01u) return 1;

    memset(lpszComSpec, 0, sizeof(lpszComSpec));
    cpu.eax = 0xcd00u;
    if (!invoke(&context, &event, &cpu, &result,
            RUNTIME_COMMAND_MISC_COMSPEC, 0u) || lpszComSpec[0] != '\0' ||
        result.cpu_delta.gpr16_write_mask != 0u) return 2;

    memset(context.guest + address, 'X', RUNTIME_COMMAND_MISC_COMSPEC_MAX + 1u);
    cpu.eax = 0u;
    if (invoke(&context, &event, &cpu, &result,
            RUNTIME_COMMAND_MISC_COMSPEC, 1u)) return 3;

    memset(context.guest + address, 0, RUNTIME_COMMAND_MISC_CURRENT_DIR_BYTES);
    cpu.esi = 0u; cpu.eax = 2u;
    if (!invoke(&context, &event, &cpu, &result,
            RUNTIME_COMMAND_MISC_GET_CURRENT_DIR, 1u) ||
        (result.eflags_values & RUNTIME_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        context.guest[address] != 'C' || context.guest[address + 1u] != ':') return 4;

    cpu.eax = 25u;
    if (!invoke(&context, &event, &cpu, &result,
            RUNTIME_COMMAND_MISC_GET_CURRENT_DIR, 1u) ||
        (result.eflags_values & RUNTIME_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_values[0] != 0u) return 5;
    runtime_opennt_error_dialog_fixture_suppress(TRUE);
    if (RcMessageBox(0x1234u, NULL, NULL, 16u | 1u) != 1 ||
        runtime_opennt_error_dialog_fixture_last_error() != 0x1234u) return 6;
    runtime_opennt_error_dialog_fixture_suppress(FALSE);
    puts("T231 S2 direct OpenNT cmdmisc import: COMSPEC and current-directory ABI verified");
    return 0;
}
