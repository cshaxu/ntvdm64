#include "redir_native_session.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <intrin.h>

#include "adapter-softpc/physical_irq.h"
#include "opennt-bop/vdmredir/vrdisp_compat.h"
#include "opennt-bop/vdmredir/vrmslot_compat.h"

/* DIVERGENCE(BOP-DIV-087): original DEM imported VDMREDIR dynamically. This
 * static program keeps provider binding declarations private to BOP
 * composition rather than adding them to an original mirror header. */

/* This is intentionally not a replacement VDMREDIR.DLL.  The provider body
 * was not recovered with OpenNT; the admitted first group supplies the
 * lifecycle and the DEM synchronous-pipe interception seam.  The 57 service
 * bodies below return the historical Win32 unavailable failure until their
 * protocol layouts are recovered as later owner groups. */

static runtime_redir_native_session *g_active_session;
static __declspec(thread) const runtime_demhndl_guest_span *g_scoped_spans;
static __declspec(thread) uint32_t g_scoped_span_count;

static void resume_with_error(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, DWORD error);
static void resume_success(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);
static int guest_read_bytes(uint16_t segment, uint16_t offset, uint8_t *bytes,
    uint32_t count);
static int guest_write_bytes(uint16_t segment, uint16_t offset,
    const uint8_t *bytes, uint32_t count);
static void set_gpr16(struct runtime_generic_ud_outcome *outcome,
    uint32_t index, uint16_t value);

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

/* `vrnmpipe.h:MAXIMUM_ASYNC_PIPES` and DOS_ASYNC_NAMED_PIPE_INFO describe
 * the historical VDMREDIR-owned lifetime.  The original provider body is not
 * retained, so this source-derived record preserves its field order and
 * ownership split: host I/O retains a private duplicate HANDLE and buffer;
 * guest results and callback addresses remain copied numeric 16:16 values. */
#define RUNTIME_REDIR_ASYNC_PIPE_CAPACITY 32u
enum { RUNTIME_REDIR_ASYNC_FREE = 0, RUNTIME_REDIR_ASYNC_PENDING = 1,
       RUNTIME_REDIR_ASYNC_COMPLETE = 2, RUNTIME_REDIR_ASYNC_DELIVERING = 3 };
typedef struct runtime_redir_async_pipe_record {
    volatile long state;
    OVERLAPPED overlapped;
    HANDLE pipe, worker;
    uint8_t *buffer;
    uint32_t serial, type2, write, buffer_far, bytes_far, error_far, anr_far, semaphore_far;
    DWORD buffer_bytes, transferred, error;
} runtime_redir_async_pipe_record;
static runtime_redir_async_pipe_record g_async_pipes[RUNTIME_REDIR_ASYNC_PIPE_CAPACITY];
static volatile long g_async_next_serial = 0;

static int session_valid(const runtime_redir_native_session *session)
{
    return session != NULL && session->magic == RUNTIME_REDIR_NATIVE_SESSION_MAGIC &&
        session->abi_version == RUNTIME_REDIR_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        runtime_dem_direct_context_valid(session->direct) &&
        session->guest_state != NULL && session->guest_read != NULL &&
        session->guest_write != NULL;
}

int runtime_redir_native_session_initialize(runtime_redir_native_session *session,
    runtime_dem_direct_context *direct, void *guest_state,
    runtime_demhndl_guest_read_fn guest_read,
    runtime_demhndl_guest_write_fn guest_write)
{
    if (session == NULL || direct == NULL || guest_state == NULL ||
        guest_read == NULL || guest_write == NULL ||
        !runtime_dem_direct_context_valid(direct)) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_REDIR_NATIVE_SESSION_MAGIC;
    session->abi_version = RUNTIME_REDIR_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session->direct = direct;
    session->guest_state = guest_state;
    session->guest_read = guest_read;
    session->guest_write = guest_write;
    return session_valid(session);
}

int runtime_redir_native_session_bind(runtime_redir_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u || g_active_session != NULL)
        return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

static void reset_mailslots(void)
{
    if (g_active_session != NULL && g_active_session->direct != NULL)
        VrpResetMailslots(g_active_session->direct->state,
            g_active_session->direct->release_handle);
}

static void reset_async_pipes(void)
{
    uint32_t index;
    for (index = 0u; index < RUNTIME_REDIR_ASYNC_PIPE_CAPACITY; ++index) {
        runtime_redir_async_pipe_record *record = &g_async_pipes[index];
        if (_InterlockedCompareExchange(&record->state, RUNTIME_REDIR_ASYNC_FREE,
                RUNTIME_REDIR_ASYNC_FREE) == RUNTIME_REDIR_ASYNC_PENDING &&
            record->pipe != INVALID_HANDLE_VALUE)
            (void)CancelIoEx(record->pipe, &record->overlapped);
        if (record->worker != NULL) {
            (void)WaitForSingleObject(record->worker, INFINITE);
            CloseHandle(record->worker);
        }
        if (record->overlapped.hEvent != NULL) CloseHandle(record->overlapped.hEvent);
        if (record->pipe != NULL && record->pipe != INVALID_HANDLE_VALUE) CloseHandle(record->pipe);
        free(record->buffer);
        memset(record, 0, sizeof(*record));
        record->pipe = INVALID_HANDLE_VALUE;
    }
    (void)_InterlockedExchange(&g_async_next_serial, 0);
}

void runtime_redir_native_session_unbind(runtime_redir_native_session *session)
{
    if (session != NULL && g_active_session == session) reset_async_pipes();
    if (session != NULL && g_active_session == session) reset_mailslots();
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) {
        session->bound = 0u;
        session->loaded = 0u;
        session->mode = 0u;
    }
}

static uint32_t read_le32(const uint8_t *bytes)
{ return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
    ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24); }

static void write_le16(uint8_t *bytes, uint16_t value)
{ bytes[0] = (uint8_t)value; bytes[1] = (uint8_t)(value >> 8); }

static int far_read(uint32_t far_value, uint8_t *bytes, uint32_t count)
{ return guest_read_bytes((uint16_t)(far_value >> 16), (uint16_t)far_value, bytes, count); }
static int far_write(uint32_t far_value, const uint8_t *bytes, uint32_t count)
{ return guest_write_bytes((uint16_t)(far_value >> 16), (uint16_t)far_value, bytes, count); }

static DWORD WINAPI async_pipe_worker(void *opaque)
{
    runtime_redir_async_pipe_record *record = (runtime_redir_async_pipe_record *)opaque;
    DWORD transferred = 0u;
    if (!GetOverlappedResult(record->pipe, &record->overlapped, &transferred, TRUE)) {
        record->transferred = 0u;
        record->error = GetLastError();
    } else {
        record->transferred = transferred;
        record->error = ERROR_SUCCESS;
    }
    _InterlockedExchange(&record->state, RUNTIME_REDIR_ASYNC_COMPLETE);
    /* This is only a thread-safe physical-line publication.  The CPU timer
     * owns the later native PIC mutation and guest int5c owns continuation. */
    (void)runtime_machine_post_physical_irq(14u);
    return 0u;
}

static runtime_redir_async_pipe_record *allocate_async_pipe(void)
{
    uint32_t index;
    for (index = 0u; index < RUNTIME_REDIR_ASYNC_PIPE_CAPACITY; ++index) {
        runtime_redir_async_pipe_record *record = &g_async_pipes[index];
        if (_InterlockedCompareExchange(&record->state, RUNTIME_REDIR_ASYNC_PENDING,
                RUNTIME_REDIR_ASYNC_FREE) == RUNTIME_REDIR_ASYNC_FREE) {
            memset(record, 0, sizeof(*record));
            record->state = RUNTIME_REDIR_ASYNC_PENDING;
            record->pipe = INVALID_HANDLE_VALUE;
            record->serial = (uint32_t)_InterlockedIncrement(&g_async_next_serial);
            return record;
        }
    }
    return NULL;
}

static void release_async_pipe(runtime_redir_async_pipe_record *record)
{
    if (record == NULL) return;
    if (record->worker != NULL) CloseHandle(record->worker);
    if (record->overlapped.hEvent != NULL) CloseHandle(record->overlapped.hEvent);
    if (record->pipe != INVALID_HANDLE_VALUE && record->pipe != NULL) CloseHandle(record->pipe);
    free(record->buffer);
    memset(record, 0, sizeof(*record));
    record->pipe = INVALID_HANDLE_VALUE;
    _InterlockedExchange(&record->state, RUNTIME_REDIR_ASYNC_FREE);
}

static int start_async_pipe(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, int write)
{
    uint8_t descriptor[24]; uint32_t descriptor_bytes = 20u, token;
    HANDLE original, duplicate = INVALID_HANDLE_VALUE;
    runtime_redir_async_pipe_record *record;
    BOOL issued;
    DWORD error;
    if ((uint16_t)event->eax == 0x0090u || (uint16_t)event->eax == 0x0091u)
        descriptor_bytes = 24u;
    if (!guest_read_bytes((uint16_t)event->ds, (uint16_t)event->esi,
            descriptor, descriptor_bytes)) { resume_with_error(event, outcome, ERROR_INVALID_ADDRESS); return 1; }
    token = ((uint32_t)(uint16_t)event->ebx) | ((uint32_t)(uint16_t)event->ebp << 16);
    if (token == 0u || !g_active_session->direct->lookup_handle(
            g_active_session->direct->state, token, &original) ||
        !VrIsNamedPipeHandle(original)) {
        resume_with_error(event, outcome, ERROR_INVALID_HANDLE); return 1;
    }
    record = allocate_async_pipe();
    if (record == NULL) { resume_with_error(event, outcome, ERROR_TOO_MANY_OPEN_FILES); return 1; }
    record->buffer_bytes = (DWORD)(descriptor[4] | ((uint16_t)descriptor[5] << 8));
    record->buffer_far = read_le32(descriptor + 6u);
    record->bytes_far = read_le32(descriptor);
    record->error_far = read_le32(descriptor + 10u);
    record->anr_far = read_le32(descriptor + 14u);
    record->type2 = descriptor_bytes == 24u;
    record->semaphore_far = record->type2 ? read_le32(descriptor + 20u) : 0u;
    record->write = write != 0;
    if ((record->buffer_bytes != 0u && record->buffer_far == 0u) ||
        record->bytes_far == 0u || record->error_far == 0u || record->anr_far == 0u ||
        (record->type2 && record->semaphore_far == 0u)) {
        release_async_pipe(record); resume_with_error(event, outcome, ERROR_INVALID_ADDRESS); return 1;
    }
    if (!DuplicateHandle(GetCurrentProcess(), original, GetCurrentProcess(),
            &duplicate, 0u, FALSE, DUPLICATE_SAME_ACCESS)) {
        error = GetLastError(); release_async_pipe(record); resume_with_error(event, outcome, error); return 1;
    }
    record->pipe = duplicate;
    record->overlapped.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (record->overlapped.hEvent == NULL ||
        (record->buffer_bytes != 0u && (record->buffer = (uint8_t *)malloc(record->buffer_bytes)) == NULL)) {
        error = record->overlapped.hEvent == NULL ? GetLastError() : ERROR_NOT_ENOUGH_MEMORY;
        release_async_pipe(record); resume_with_error(event, outcome, error); return 1;
    }
    if (record->write && record->buffer_bytes != 0u &&
        !far_read(record->buffer_far, record->buffer, record->buffer_bytes)) {
        release_async_pipe(record); resume_with_error(event, outcome, ERROR_INVALID_ADDRESS); return 1;
    }
    issued = record->write ? WriteFile(record->pipe, record->buffer, record->buffer_bytes,
        NULL, &record->overlapped) : ReadFile(record->pipe, record->buffer,
        record->buffer_bytes, NULL, &record->overlapped);
    if (!issued && GetLastError() != ERROR_IO_PENDING) {
        error = GetLastError(); release_async_pipe(record); resume_with_error(event, outcome, error); return 1;
    }
    record->worker = CreateThread(NULL, 0u, async_pipe_worker, record, 0u, NULL);
    if (record->worker == NULL) {
        error = GetLastError(); (void)CancelIoEx(record->pipe, &record->overlapped);
        release_async_pipe(record); resume_with_error(event, outcome, error); return 1;
    }
    resume_success(event, outcome);
    return 1;
}

static int deliver_async_pipe(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    uint32_t index; runtime_redir_async_pipe_record *chosen = NULL;
    uint32_t serial = UINT32_MAX; uint8_t result[2];
    for (index = 0u; index < RUNTIME_REDIR_ASYNC_PIPE_CAPACITY; ++index) {
        runtime_redir_async_pipe_record *record = &g_async_pipes[index];
        if (_InterlockedCompareExchange(&record->state, RUNTIME_REDIR_ASYNC_COMPLETE,
                RUNTIME_REDIR_ASYNC_COMPLETE) == RUNTIME_REDIR_ASYNC_COMPLETE &&
            record->serial < serial) { chosen = record; serial = record->serial; }
    }
    if (chosen == NULL || _InterlockedCompareExchange(&chosen->state,
            RUNTIME_REDIR_ASYNC_DELIVERING, RUNTIME_REDIR_ASYNC_COMPLETE) != RUNTIME_REDIR_ASYNC_COMPLETE) {
        /* `int5c.asm` requires the explicit nothing disposition (ZF=0,
         * CF=0); leaving a prior guest ZF intact would choose a false branch. */
        resume_success(event, outcome);
        outcome->eflags_write_mask = 1u | 0x40u;
        outcome->eflags_values &= ~0x40u;
        return 1;
    }
    if ((!chosen->write && chosen->transferred != 0u &&
         !far_write(chosen->buffer_far, chosen->buffer, chosen->transferred)) ||
        (write_le16(result, (uint16_t)chosen->transferred), !far_write(chosen->bytes_far, result, 2u)) ||
        (write_le16(result, (uint16_t)chosen->error), !far_write(chosen->error_far, result, 2u))) {
        release_async_pipe(chosen); resume_with_error(event, outcome, ERROR_INVALID_ADDRESS); return 1;
    }
    resume_success(event, outcome);
    outcome->eflags_write_mask = 1u | 0x40u;
    outcome->eflags_values = (event->eflags | 1u) & ~0x40u; /* ZF=0, CF=1: int5c async path. */
    set_gpr16(outcome, 0u, (uint16_t)chosen->type2); /* AL */
    set_gpr16(outcome, 1u, (uint16_t)chosen->anr_far); /* BX */
    set_gpr16(outcome, 2u, (uint16_t)(chosen->anr_far >> 16)); /* CX */
    set_gpr16(outcome, 4u, (uint16_t)chosen->buffer_far); /* SI */
    set_gpr16(outcome, 5u, (uint16_t)chosen->semaphore_far); /* DI */
    outcome->segment_write_mask = (1u << 0) | (1u << 3); /* ES, DS */
    outcome->segment_values[0] = (uint16_t)(chosen->semaphore_far >> 16);
    outcome->segment_values[3] = (uint16_t)(chosen->buffer_far >> 16);
    release_async_pipe(chosen);
    return 1;
}

static void resume_with_error(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, DWORD error)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 4u;
    outcome->gpr16_write_mask = 1u;
    outcome->gpr16_values[0] = (uint16_t)error;
    outcome->eflags_write_mask = 1u;
    outcome->eflags_values = event->eflags | 1u;
}

static void resume_success(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 4u;
    outcome->gpr16_write_mask = 1u;
    outcome->gpr16_values[0] = 0u;
    outcome->eflags_write_mask = 1u;
    outcome->eflags_values = event->eflags & ~1u;
}

static uint16_t word_at(uint32_t value) { return (uint16_t)value; }
static uint32_t real_address(uint16_t segment, uint16_t offset)
{ return ((uint32_t)segment << 4) + offset; }

static int read_oem_string(uint16_t segment, uint16_t offset, char *text,
    uint32_t capacity)
{
    uint32_t index, address;
    if (g_active_session == NULL || text == NULL || capacity < 2u) return 0;
    address = real_address(segment, offset);
    for (index = 0u; index < capacity; ++index) {
        if (!g_active_session->guest_read(g_active_session->guest_state,
                address + index, (uint8_t *)&text[index], 1u)) return 0;
        if (text[index] == '\0') return 1;
    }
    return 0;
}

static int guest_read_bytes(uint16_t segment, uint16_t offset, uint8_t *bytes,
    uint32_t count)
{
    if (g_active_session == NULL || bytes == NULL ||
        !g_active_session->guest_read(g_active_session->guest_state,
            real_address(segment, offset), bytes, count)) {
        SetLastError(ERROR_INVALID_ADDRESS);
        return 0;
    }
    return 1;
}

static int guest_write_bytes(uint16_t segment, uint16_t offset,
    const uint8_t *bytes, uint32_t count)
{
    if (g_active_session == NULL || bytes == NULL ||
        !g_active_session->guest_write(g_active_session->guest_state,
            real_address(segment, offset), bytes, count)) {
        SetLastError(ERROR_INVALID_ADDRESS);
        return 0;
    }
    return 1;
}

static void set_gpr16(struct runtime_generic_ud_outcome *outcome,
    uint32_t index, uint16_t value)
{
    outcome->gpr16_write_mask |= (1u << index);
    outcome->gpr16_values[index] = value;
}

static int mailslot_get_record(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome,
    PVR_MAILSLOT_INFO *out_record, HANDLE *out_host)
{
    PVR_MAILSLOT_INFO record = VrpMapMailslotHandle16(word_at(event->ebx));
    HANDLE host;
    if (record == NULL || !g_active_session->direct->lookup_handle(
            g_active_session->direct->state, record->Handle16, &host)) {
        resume_with_error(event, outcome, ERROR_INVALID_HANDLE);
        return 0;
    }
    *out_record = record;
    *out_host = host;
    return 1;
}

static int mailslot_query(HANDLE host, DWORD *next, DWORD *count)
{
    DWORD maximum, timeout;
    return GetMailslotInfo(host, &maximum, next, count, &timeout) != 0;
}

int runtime_mailslot_read_rehost(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    PVR_MAILSLOT_INFO record;
    HANDLE host;
    DWORD read = 0u, next = MAILSLOT_NO_MESSAGE, count;
    if (!mailslot_get_record(event, outcome, &record, &host)) return 1;
    if (!SetMailslotInfo(host, ((DWORD)word_at(event->edx) << 16) | word_at(event->ecx)) ||
        !GetMailslotInfo(host, NULL, &next, &count, NULL)) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    if (next != MAILSLOT_NO_MESSAGE && next > record->MessageSize) {
        resume_with_error(event, outcome, ERROR_INSUFFICIENT_BUFFER); return 1;
    }
    if (next != MAILSLOT_NO_MESSAGE && next != 0u) {
        uint8_t *bytes = (uint8_t *)malloc(next);
        if (bytes == NULL || !ReadFile(host, bytes, next, &read, NULL) ||
            !guest_write_bytes(word_at(event->es), word_at(event->edi), bytes, read)) {
            DWORD error = bytes == NULL ? ERROR_NOT_ENOUGH_MEMORY : GetLastError();
            free(bytes); resume_with_error(event, outcome, error); return 1;
        }
        free(bytes);
    }
    if (!mailslot_query(host, &next, &count)) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    resume_success(event, outcome);
    set_gpr16(outcome, 0u, (uint16_t)read);
    set_gpr16(outcome, 1u, next == MAILSLOT_NO_MESSAGE ? 0u : (uint16_t)next);
    set_gpr16(outcome, 2u, 0u); /* DOS mailslot priority */
    return 1;
}

static int mailslot_terminate(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    return runtime_redir_native_session_invoke_scoped_body(event, outcome,
        runtime_vrmslot_terminate_bop_body, 4u);
}

static int invoke_mailslot_spans(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void (*body)(void),
    const runtime_demhndl_guest_span *spans, uint32_t count)
{
    int handled;
    if (g_scoped_spans != NULL || spans == NULL || count == 0u || count > 8u) return 0;
    g_scoped_spans = spans; g_scoped_span_count = count;
    handled = runtime_redir_native_session_invoke_scoped_body(event, outcome, body, 4u);
    g_scoped_spans = NULL; g_scoped_span_count = 0u;
    return handled;
}

static int dispatch_service(uint8_t service,
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    switch (service) {
    case 0x00u: /* SVC_RDRINITIALIZE */
    case 0x01u: /* SVC_RDRUNINITIALIZE */
        /* DIVERGENCE(BOP-DIV-088): the imported VrInitialize/VrUninitialize
         * success path requires VDD hook registration, CCPU guest-state write,
         * NetBIOS/DLC and ICA lifecycle.  None may be fabricated by this
         * copied-frame route.  Until the Redirector owner admits that complete
         * source-shaped substrate, fail both services rather than marking this
         * session loaded without their original prerequisites. */
        resume_with_error(event, outcome, ERROR_INVALID_FUNCTION);
        return 1;
    case 0x29u: /* SVC_RDRRETURN_MODE */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        resume_success(event, outcome);
        outcome->gpr16_write_mask |= 1u;
        outcome->gpr16_values[0] = (uint16_t)g_active_session->mode;
        return 1;
    case 0x2au: /* SVC_RDRSET_MODE */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        g_active_session->mode = (uint16_t)event->eax;
        resume_success(event, outcome);
        return 1;
    case 0x0bu: /* SVC_RDRMAKEMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return runtime_redir_native_session_invoke_scoped_body(event, outcome,
            VrMakeMailslot, 4u);
    case 0x09u: /* SVC_RDRDELETEMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return runtime_redir_native_session_invoke_scoped_body(event, outcome,
            VrDeleteMailslot, 4u);
    case 0x0au: /* SVC_RDRGETMAILSLOTINFO */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return runtime_redir_native_session_invoke_scoped_body(event, outcome,
            VrGetMailslotInfo, 4u);
    case 0x0cu: /* SVC_RDRPEEKMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return runtime_redir_native_session_invoke_scoped_body(event, outcome,
            VrPeekMailslot, 4u);
    case 0x0du: /* SVC_RDRREADMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        { PVR_MAILSLOT_INFO record = VrpMapMailslotHandle16(word_at(event->ebx));
          runtime_demhndl_guest_span span;
          if (record == NULL) { resume_with_error(event, outcome, ERROR_INVALID_HANDLE); return 1; }
          span.segment = word_at(event->es); span.offset = word_at(event->edi);
          span.bytes = record->MessageSize; span.write_back = 1u;
          return invoke_mailslot_spans(event, outcome, VrReadMailslot, &span, 1u); }
    case 0x0eu: /* SVC_RDRWRITEMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        { uint8_t descriptor[sizeof(struct DosWriteMailslotStruct)];
          runtime_demhndl_guest_span spans[2];
          if (!guest_read_bytes(word_at(event->es), word_at(event->edi), descriptor,
              sizeof(descriptor))) { resume_with_error(event, outcome, GetLastError()); return 1; }
          spans[0].segment = word_at(event->es); spans[0].offset = word_at(event->edi);
          spans[0].bytes = sizeof(descriptor); spans[0].write_back = 0u;
          spans[1].segment = (uint16_t)(descriptor[6] | ((uint16_t)descriptor[7] << 8));
          spans[1].offset = (uint16_t)(descriptor[4] | ((uint16_t)descriptor[5] << 8));
          spans[1].bytes = word_at(event->ecx); spans[1].write_back = 0u;
          return invoke_mailslot_spans(event, outcome, VrWriteMailslot, spans, 2u); }
    case 0x0fu: /* SVC_RDRTERMINATE / NetResetEnvironment */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_terminate(event, outcome);
    case 0x23u: /* SVC_RDRREADASYNCNMPIPE */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return start_async_pipe(event, outcome, 0);
    case 0x24u: /* SVC_RDRWRITEASYNCNMPIPE */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return start_async_pipe(event, outcome, 1);
    case 0x26u: /* SVC_NETBIOS5CINTERRUPT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return deliver_async_pipe(event, outcome);
    default:
        /* 02..08 and 20/21 are intentionally one typed provider route, but
         * their VDMREDIR protocol body is absent.  Returning this original
         * API-style failure is safer than treating copied registers as a
         * guest pointer or claiming a pipe operation succeeded. */
        resume_with_error(event, outcome, ERROR_INVALID_FUNCTION);
        return 1;
    }
}

int runtime_redir_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        event == NULL || outcome == NULL || event->window_bytes < 4u ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    return runtime_vr_dispatch_with_frame(event->window[3], event, outcome,
        dispatch_service);
}

int runtime_redir_native_session_invoke_scoped_body(
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
    call.service = event->window_bytes >= 4u ? event->window[3] : 0u;
    call.direct = g_active_session->direct;
    call.boundary = &boundary; call.cpu = &cpu; call.result = &result;
    call.guest_state = g_active_session->guest_state;
    call.guest_read = g_active_session->guest_read;
    call.guest_write = g_active_session->guest_write;
    call.guest_spans = g_scoped_spans;
    call.guest_span_count = g_scoped_span_count;
    return runtime_demhndl_invoke_body_with_resume(&call, body, resume_bytes) &&
        copy_outcome(&result, outcome);
}
