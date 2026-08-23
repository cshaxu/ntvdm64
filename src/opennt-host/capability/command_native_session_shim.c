#include "command_native_session_shim.h"

#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

static bx_ntvdm_command_native_session *g_active_session;

static int session_valid(const bx_ntvdm_command_native_session *session)
{
    return session != NULL &&
        session->magic == BX_NTVDM_COMMAND_NATIVE_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_COMMAND_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        bx_ntvdm_command_misc_session_valid(&session->direct);
}

int bx_ntvdm_command_native_session_initialize(
    bx_ntvdm_command_native_session *session)
{
    if (session == NULL) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_COMMAND_NATIVE_SESSION_MAGIC;
    session->abi_version = BX_NTVDM_COMMAND_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    bx_ntvdm_command_misc_session_initialize(&session->direct);
    return session_valid(session);
}

int bx_ntvdm_command_native_session_bind(
    bx_ntvdm_command_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u ||
        g_active_session != NULL) return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

void bx_ntvdm_command_native_session_unbind(
    bx_ntvdm_command_native_session *session)
{
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) {
        bx_ntvdm_host_handle_manager_reset(session->direct.handles);
        session->bound = 0u;
    }
}

static int event_valid(const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != NULL && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= BX_NTVDM_GENERIC_UD_WINDOW_BYTES;
}

static void copy_cpu(const struct bx_ntvdm_generic_ud_event_v1 *source,
    bx_ntvdm_cpu_state_v1 *target)
{
    bx_ntvdm_cpu_state_v1_initialize(target, source->execution_mode);
    target->eax = source->eax; target->ebx = source->ebx; target->ecx = source->ecx;
    target->edx = source->edx; target->esi = source->esi; target->edi = source->edi;
    target->ebp = source->ebp; target->esp = source->esp; target->eip = source->eip;
    target->eflags = source->eflags; target->cs = source->cs; target->ds = source->ds;
    target->es = source->es; target->ss = source->ss; target->fs = source->fs;
    target->gs = source->gs;
}

static int guest_read(void *state, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    return session_valid((bx_ntvdm_command_native_session *)state) &&
        bx_ntvdm_mantle_checked_ram_read_v1(address, bytes, byte_count);
}

static int guest_write(void *state, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    return session_valid((bx_ntvdm_command_native_session *)state) &&
        bx_ntvdm_mantle_checked_ram_write_v1(address, bytes, byte_count);
}

static int copy_outcome(const bx_ntvdm_cpu_result_v2 *result,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!bx_ntvdm_cpu_result_v2_valid(result) || outcome == NULL ||
        result->disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME ?
        BX_NTVDM_GENERIC_UD_RESUME :
        (result->disposition == BX_NTVDM_CPU_RESULT_V2_PENDING ?
            BX_NTVDM_GENERIC_UD_PENDING : BX_NTVDM_GENERIC_UD_STOP);
    outcome->resume_rip = result->resume_rip;
    outcome->gpr16_write_mask = result->cpu_delta.gpr16_write_mask;
    memcpy(outcome->gpr16_values, result->cpu_delta.gpr16_values,
        sizeof(outcome->gpr16_values));
    outcome->segment_write_mask = result->cpu_delta.segment_write_mask;
    memcpy(outcome->segment_values, result->cpu_delta.segment_values,
        sizeof(outcome->segment_values));
    outcome->eflags_write_mask = result->eflags_write_mask;
    outcome->eflags_values = result->eflags_values;
    return 1;
}

int bx_ntvdm_command_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_command_misc_call call;
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        !event_valid(event) || outcome == NULL) return 0;
    memset(&boundary, 0, sizeof(boundary));
    boundary.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = sizeof(boundary);
    boundary.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.cpu_id = event->cpu_id; boundary.vector = event->vector;
    boundary.error_code = event->error_code; boundary.fault_rip = event->fault_rip;
    copy_cpu(event, &cpu);
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = event->window_bytes >= 4u ? event->window[3] : 0u;
    call.boundary = &boundary; call.cpu = &cpu; call.result = &result;
    call.guest_state = g_active_session; call.guest_read = guest_read;
    call.guest_write = guest_write; call.session = &g_active_session->direct;
    call.first_call = 1u;
    return bx_ntvdm_command_misc_invoke(&call) && copy_outcome(&result, outcome);
}
