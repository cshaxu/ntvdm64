#include "dem_native_session.h"
#include "opennt-bop/dem/nt_bop_dem.h"

#include <string.h>

static runtime_dem_native_session *g_active_session;

static int session_valid(const runtime_dem_native_session *session)
{
    return session != NULL && session->magic == RUNTIME_DEM_NATIVE_SESSION_MAGIC &&
        session->abi_version == RUNTIME_DEM_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        runtime_dem_direct_context_valid(session->direct) &&
        session->guest_state != NULL && session->guest_read != NULL &&
        session->guest_write != NULL;
}

int runtime_dem_native_session_initialize(runtime_dem_native_session *session,
    runtime_dem_direct_context *direct, void *guest_state,
    runtime_demhndl_guest_read_fn guest_read,
    runtime_demhndl_guest_write_fn guest_write)
{
    if (session == NULL || direct == NULL || guest_state == NULL ||
        guest_read == NULL || guest_write == NULL ||
        !runtime_dem_direct_context_valid(direct)) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_DEM_NATIVE_SESSION_MAGIC;
    session->abi_version = RUNTIME_DEM_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session->direct = direct;
    session->guest_state = guest_state;
    session->guest_read = guest_read;
    session->guest_write = guest_write;
    return session_valid(session);
}

int runtime_dem_native_session_bind(runtime_dem_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u || g_active_session != NULL)
        return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

void runtime_dem_native_session_unbind(runtime_dem_native_session *session)
{
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) session->bound = 0u;
}

static int event_valid(const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= RUNTIME_GENERIC_UD_WINDOW_BYTES;
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

int runtime_dem_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    runtime_exception_event boundary;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_demhndl_call call;
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
    if (event->window_bytes < 4u || event->window[0] != 0xc4u ||
        event->window[1] != 0xc4u || event->window[2] != 0x50u ||
        event->eip > UINT64_MAX - 3u) return 0;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_DEMHNDL_CALL_MAGIC;
    call.abi_version = RUNTIME_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    /* MS_bop_0 itself reads the service byte at staged CS:IP and invokes the
     * original DemDispatch table.  This copied byte is only the bounded
     * mapping descriptor needed by the same-shaped SAS facade. */
    call.service = event->window[3];
    call.direct = g_active_session->direct; call.boundary = &boundary;
    call.cpu = &cpu; call.result = &result; call.guest_state = g_active_session->guest_state;
    call.guest_read = g_active_session->guest_read;
    call.guest_write = g_active_session->guest_write;
    /* DIVERGENCE(BOP-DIV-098): the admitted x86 FastOrSlow caller uses
     * 50:42 although the visible OpenNT table slot is a no-op.  No historical
     * worker is available; retain the previously-approved source-derived
     * demRead-body seam for this exact service.  All other DEM services enter
     * the imported MS_bop_0 body below. */
    if (call.service == 0x42u)
        return runtime_demhndl_invoke_fast_read(&call) &&
            copy_outcome(&result, outcome);
    cpu.eip += 3u;
    return runtime_demhndl_invoke_body_with_resume(&call, MS_bop_0, 4u) &&
        copy_outcome(&result, outcome);
}

int runtime_dem_native_session_invoke_scoped_body(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void (*body)(void),
    uint32_t resume_bytes)
{
    runtime_exception_event boundary;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_demhndl_call call;
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        !event_valid(event) || outcome == NULL || body == NULL ||
        (resume_bytes != 3u && resume_bytes != 4u)) return 0;
    memset(&boundary, 0, sizeof(boundary));
    boundary.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = sizeof(boundary);
    boundary.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.cpu_id = event->cpu_id; boundary.vector = event->vector;
    boundary.error_code = event->error_code; boundary.fault_rip = event->fault_rip;
    copy_cpu(event, &cpu);
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_DEMHNDL_CALL_MAGIC;
    call.abi_version = RUNTIME_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.direct = g_active_session->direct; call.boundary = &boundary;
    call.cpu = &cpu; call.result = &result; call.guest_state = g_active_session->guest_state;
    call.guest_read = g_active_session->guest_read;
    call.guest_write = g_active_session->guest_write;
    return runtime_demhndl_invoke_body_with_resume(&call, body, resume_bytes) &&
        copy_outcome(&result, outcome);
}
