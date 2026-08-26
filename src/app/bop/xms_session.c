#include "xms_session.h"

#include "opennt-bop/xms/opennt_xms_compat.h"
#include "adapter-softpc/mechanical_action.h"

#include <string.h>

static runtime_xms_native_session *g_active_session;

static int session_valid(const runtime_xms_native_session *session)
{
    return session != NULL && session->magic == RUNTIME_XMS_NATIVE_SESSION_MAGIC &&
        session->abi_version == RUNTIME_XMS_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u;
}

int runtime_xms_native_session_initialize(
    runtime_xms_native_session *session, uint32_t total_memory_kib)
{
    if (session == NULL || total_memory_kib < 1024u ||
        !runtime_xms_configure_memory_kib(total_memory_kib) ||
        !runtime_xms_initialize()) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_XMS_NATIVE_SESSION_MAGIC;
    session->abi_version = RUNTIME_XMS_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    return session_valid(session);
}

int runtime_xms_native_session_bind(runtime_xms_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u || g_active_session != NULL)
        return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

void runtime_xms_native_session_unbind(runtime_xms_native_session *session)
{
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) session->bound = 0u;
}

static int event_valid(const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 4u;
}

static void copy_cpu(const struct runtime_generic_ud_event *source,
    runtime_cpu_state *target)
{
    runtime_cpu_state_initialize(target, source->execution_mode);
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
    return session_valid((runtime_xms_native_session *)state) &&
        runtime_machine_checked_ram_read(address, bytes, byte_count);
}

static int guest_write(void *state, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    return session_valid((runtime_xms_native_session *)state) &&
        runtime_machine_checked_ram_write(address, bytes, byte_count);
}

static int copy_outcome(const runtime_cpu_result *result,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!runtime_cpu_result_valid(result) || outcome == NULL ||
        result->disposition == RUNTIME_CPU_RESULT_PASS_THROUGH) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = result->disposition == RUNTIME_CPU_RESULT_RESUME ?
        RUNTIME_GENERIC_UD_RESUME : RUNTIME_GENERIC_UD_STOP;
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

int runtime_xms_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    runtime_exception_event boundary;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_xms_call call;
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        !event_valid(event) || outcome == NULL) return 0;
    memset(&boundary, 0, sizeof(boundary));
    boundary.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = sizeof(boundary);
    boundary.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.cpu_id = event->cpu_id; boundary.vector = event->vector;
    boundary.error_code = event->error_code; boundary.fault_rip = event->fault_rip;
    copy_cpu(event, &cpu);
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_XMS_CALL_MAGIC;
    call.abi_version = RUNTIME_XMS_CALL_VERSION;
    call.struct_bytes = sizeof(call); call.service = event->window[3];
    call.boundary = &boundary; call.cpu = &cpu; call.result = &result;
    call.guest_state = g_active_session; call.guest_read = guest_read;
    call.guest_write = guest_write;
    return runtime_xms_invoke(&call) && copy_outcome(&result, outcome);
}
