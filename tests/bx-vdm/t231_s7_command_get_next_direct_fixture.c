#include "bop/shim/command_misc_shim.h"
#include <stdio.h>
#include <string.h>

typedef struct fixture_context { uint8_t guest[0x10000]; } fixture_context;

static int read_guest(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{ fixture_context *context = (fixture_context *)state; if (context == NULL || address > sizeof(context->guest) || bytes > sizeof(context->guest) - address) return 0; memcpy(buffer, context->guest + address, bytes); return 1; }
static int write_guest(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{ fixture_context *context = (fixture_context *)state; if (context == NULL || address > sizeof(context->guest) || bytes > sizeof(context->guest) - address) return 0; memcpy(context->guest + address, buffer, bytes); return 1; }
static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{ memset(event, 0, sizeof(*event)); event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC; event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event->struct_bytes = sizeof(*event); event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event->vector = 6u; event->fault_rip = 0x500u; }
static int invoke(fixture_context *context, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result,
    bx_ntvdm_command_misc_session *session, uint32_t first_call)
{ bx_ntvdm_command_misc_call call; memset(&call, 0, sizeof(call)); call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC; call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION; call.struct_bytes = sizeof(call); call.service = BX_NTVDM_COMMAND_MISC_GET_NEXT; call.boundary = event; call.cpu = cpu; call.result = result; call.guest_state = context; call.guest_read = read_guest; call.guest_write = write_guest; call.session = session; call.first_call = first_call; return bx_ntvdm_command_misc_invoke(&call); }

int main(void)
{
    fixture_context context; bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result; bx_ntvdm_command_misc_session session, retry_session; CMDINFO *info;
    uint32_t info_address = 0x1000u, command_address = 0x4000u, app_address = 0x5000u;
    memset(&context, 0, sizeof(context)); event_initialize(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu.ds = 0x100u;
    bx_ntvdm_command_misc_session_initialize(&session);
    if (!bx_ntvdm_command_misc_session_set_command_source(&session,
            "C:\\TOOLS\\HELLO.COM", "-x", 2u, 437u)) { fprintf(stderr, "source\n"); return 1; }
    info = (CMDINFO *)(context.guest + info_address); info->EnvSeg = 0x300u; info->EnvSize = 0x100u;
    info->CmdLineSeg = 0x400u; info->CmdLineOff = 0u; info->CmdLineSize = 128u;
    info->ExecPathSeg = 0x500u; info->ExecPathOff = 0u; info->ExecPathSize = 128u;
    if (!invoke(&context, &event, &cpu, &result, &session, 1u) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        result.resume_rip != event.fault_rip + 4u) { fprintf(stderr, "first\n"); return 2; }
    info = (CMDINFO *)(context.guest + info_address);
    if (info->ExecExtType != COM_EXTENTION || info->ExecPathSize != 19u ||
        info->CurDrive != 2u || info->CodePage != 437u ||
        strcmp((CHAR *)context.guest + app_address, "C:\\TOOLS\\HELLO.COM") != 0 ||
        context.guest[command_address + 1u] != 10u ||
        strcmp((CHAR *)context.guest + command_address + 2u, "HELLO -x\r\n") != 0 ||
        session.command_source_delivered != 1u) { fprintf(stderr, "result ext=%u path=%u drive=%u cp=%u app=%s cnt=%u line=%s delivered=%u\n", info->ExecExtType, info->ExecPathSize, info->CurDrive, info->CodePage, context.guest + app_address, context.guest[command_address + 1u], context.guest + command_address + 2u, session.command_source_delivered); return 3; }
    if (!invoke(&context, &event, &cpu, &result, &session, 0u) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_STOP) { fprintf(stderr, "terminal\n"); return 4; }
    bx_ntvdm_command_misc_session_initialize(&retry_session);
    if (!bx_ntvdm_command_misc_session_set_command_source(&retry_session,
            "C:\\TOOLS\\RETRY.EXE", "", 2u, 437u) ||
        !bx_ntvdm_command_misc_session_set_command_environment(&retry_session,
            "FOO=BAR\0\0", 9u)) return 5;
    memset(&context, 0, sizeof(context));
    info = (CMDINFO *)(context.guest + info_address); info->EnvSeg = 0x300u; info->EnvSize = 4u;
    info->CmdLineSeg = 0x400u; info->CmdLineSize = 128u;
    info->ExecPathSeg = 0x500u; info->ExecPathSize = 128u;
    if (!invoke(&context, &event, &cpu, &result, &retry_session, 0u) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_values[0] != 9u || retry_session.command_source_repeat_pending == 0u ||
        retry_session.command_source_delivered != 0u) return 6;
    info->EnvSize = 9u;
    if (!invoke(&context, &event, &cpu, &result, &retry_session, 0u) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        retry_session.command_source_repeat_pending != 0u || retry_session.command_source_delivered != 1u ||
        strcmp((CHAR *)context.guest + app_address, "C:\\TOOLS\\RETRY.EXE") != 0 ||
        memcmp(context.guest + 0x3000u, "FOO=BAR\0\0", 9u) != 0) return 7;
    puts("T231 S7 direct OpenNT cmdGetNextCmd CLI-source handoff, environment retry/re-entry, and no-command terminal behavior verified");
    return 0;
}
