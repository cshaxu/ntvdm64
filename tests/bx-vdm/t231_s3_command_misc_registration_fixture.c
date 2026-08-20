#include <stdio.h>
#include <string.h>

#include "bop/shim/command_misc_shim.h"

typedef struct fixture_context { uint8_t guest[0x20000]; } fixture_context;

static int guest_read(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes); return 1;
}

static int guest_write(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes); return 1;
}

static void initialize_event(bx_ntvdm_exception_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u; event->fault_rip = 0x600u;
}

static int invoke(fixture_context *context, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result,
    bx_ntvdm_command_misc_session *session, uint32_t service)
{
    bx_ntvdm_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call); call.service = service;
    call.boundary = event; call.cpu = cpu; call.result = result;
    call.guest_state = context; call.guest_read = guest_read; call.guest_write = guest_write;
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
    SCSINFO expected;
    const uint32_t scs_address = 0x1000u;
    const uint32_t dos_binary_address = 0x1200u;
    const uint32_t fd_access_address = 0x1300u;

    memset(&context, 0, sizeof(context)); memset(&expected, 0x5a, sizeof(expected));
    memcpy(context.guest + scs_address, &expected, sizeof(expected));
    context.guest[dos_binary_address] = 0x44u;
    context.guest[fd_access_address] = 0x33u; context.guest[fd_access_address + 1u] = 0x22u;
    initialize_event(&event); bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_command_misc_session_initialize(&session);
    cpu.ds = 0x100u; cpu.edx = 0u; cpu.ebx = 0x200u; cpu.ecx = 0x300u;
    if (!invoke(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_SET_INFO) || !bx_ntvdm_command_misc_session_valid(&session) ||
        pSCSInfo != &session.scs_info || pSCS_ToSync != &session.scs_info.SCS_ToSync ||
        pIsDosBinary != &session.is_dos_binary || pFDAccess != &session.fd_access ||
        session.scs_info_address != scs_address || session.is_dos_binary_address != dos_binary_address ||
        session.fd_access_address != fd_access_address ||
        memcmp(&session.scs_info, &expected, sizeof(expected)) != 0 ||
        session.is_dos_binary != 0x44u || session.fd_access != 0x2233u) return 1;
    if (!invoke(&context, &event, &cpu, &result, NULL,
            BX_NTVDM_COMMAND_MISC_SAVE_WORLD) || result.cpu_delta.gpr16_write_mask != 0u)
        return 2;
    puts("T231 S3 direct OpenNT cmdmisc registration: CMDINFO state and original SaveWorld no-op verified");
    return 0;
}
