#include "redir_session_shim.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This is intentionally not a replacement VDMREDIR.DLL.  The provider body
 * was not recovered with OpenNT; the admitted first group supplies the
 * lifecycle and the DEM synchronous-pipe interception seam.  The 57 service
 * bodies below return the historical Win32 unavailable failure until their
 * protocol layouts are recovered as later owner groups. */

static bx_ntvdm_redir_native_session *g_active_session;

/* `vrmslot.h:VR_MAILSLOT_INFO` deliberately has both a DOS Handle16 and a
 * native Handle32.  Keep the same two-layer shape: the existing shared
 * manager owns Handle32; this Redirector-only index owns the historical
 * invented 16-bit mailslot number.  It never truncates a native HANDLE. */
#define BX_NTVDM_REDIR_MAILSLOT_CAPACITY 32u
typedef struct bx_ntvdm_redir_mailslot_record {
    uint16_t legacy_handle, pdb, buffer_segment, buffer_offset, selector;
    uint32_t host_token, message_size, slot_size;
    /* Win32 mailslots have no non-destructive read primitive.  A successful
     * Peek therefore owns one copied message until the matching Read, which
     * preserves the DOS-visible peek/read contract without exposing a host
     * buffer or HANDLE to the guest. */
    uint8_t *peek_bytes;
    uint32_t peek_size, peek_next_size;
} bx_ntvdm_redir_mailslot_record;
static bx_ntvdm_redir_mailslot_record g_mailslots[BX_NTVDM_REDIR_MAILSLOT_CAPACITY];
static uint16_t g_next_mailslot_handle = 1u;

static int session_valid(const bx_ntvdm_redir_native_session *session)
{
    return session != NULL && session->magic == BX_NTVDM_REDIR_NATIVE_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_REDIR_NATIVE_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) && session->bound <= 1u &&
        bx_ntvdm_dem_direct_context_valid(session->direct) &&
        session->guest_state != NULL && session->guest_read != NULL &&
        session->guest_write != NULL;
}

int bx_ntvdm_redir_native_session_initialize(bx_ntvdm_redir_native_session *session,
    bx_ntvdm_dem_direct_context *direct, void *guest_state,
    bx_ntvdm_demhndl_guest_read_fn guest_read,
    bx_ntvdm_demhndl_guest_write_fn guest_write)
{
    if (session == NULL || direct == NULL || guest_state == NULL ||
        guest_read == NULL || guest_write == NULL ||
        !bx_ntvdm_dem_direct_context_valid(direct)) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_REDIR_NATIVE_SESSION_MAGIC;
    session->abi_version = BX_NTVDM_REDIR_NATIVE_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session->direct = direct;
    session->guest_state = guest_state;
    session->guest_read = guest_read;
    session->guest_write = guest_write;
    return session_valid(session);
}

int bx_ntvdm_redir_native_session_bind(bx_ntvdm_redir_native_session *session)
{
    if (!session_valid(session) || session->bound != 0u || g_active_session != NULL)
        return 0;
    session->bound = 1u;
    g_active_session = session;
    return 1;
}

static void reset_mailslots(void)
{
    uint32_t index;
    if (g_active_session != NULL && g_active_session->direct != NULL) {
        for (index = 0u; index < BX_NTVDM_REDIR_MAILSLOT_CAPACITY; ++index) {
            DWORD error;
            free(g_mailslots[index].peek_bytes);
            if (g_mailslots[index].host_token != 0u)
                (void)g_active_session->direct->release_handle(
                    g_active_session->direct->state, g_mailslots[index].host_token, &error);
        }
    }
    memset(g_mailslots, 0, sizeof(g_mailslots));
    g_next_mailslot_handle = 1u;
}

static void clear_mailslot(bx_ntvdm_redir_mailslot_record *record)
{
    if (record == NULL) return;
    free(record->peek_bytes);
    memset(record, 0, sizeof(*record));
}

void bx_ntvdm_redir_native_session_unbind(bx_ntvdm_redir_native_session *session)
{
    if (session != NULL && g_active_session == session) reset_mailslots();
    if (session != NULL && g_active_session == session) g_active_session = NULL;
    if (session_valid(session)) {
        session->bound = 0u;
        session->loaded = 0u;
        session->mode = 0u;
    }
}

static void resume_with_error(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, DWORD error)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 4u;
    outcome->gpr16_write_mask = 1u;
    outcome->gpr16_values[0] = (uint16_t)error;
    outcome->eflags_write_mask = 1u;
    outcome->eflags_values = event->eflags | 1u;
}

static void resume_success(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
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

static int local_mailslot_name(const char *oem, wchar_t *wide, uint32_t capacity)
{
    const char *suffix;
    char local[260];
    int chars;
    if (oem == NULL || wide == NULL || capacity == 0u) return 0;
    suffix = strstr(oem, "\\MAILSLOT\\");
    if (suffix == NULL) { SetLastError(ERROR_BAD_PATHNAME); return 0; }
    if (sprintf_s(local, sizeof(local), "\\\\.\\mailslot\\%s", suffix + 10u) < 0) {
        SetLastError(ERROR_BUFFER_OVERFLOW); return 0;
    }
    chars = MultiByteToWideChar(CP_OEMCP, 0, local, -1, wide, (int)capacity);
    return chars > 0;
}

static bx_ntvdm_redir_mailslot_record *find_mailslot(uint16_t legacy_handle)
{
    uint32_t index;
    for (index = 0u; index < BX_NTVDM_REDIR_MAILSLOT_CAPACITY; ++index)
        if (g_mailslots[index].legacy_handle == legacy_handle) return &g_mailslots[index];
    return NULL;
}

static bx_ntvdm_redir_mailslot_record *allocate_mailslot(void)
{
    uint32_t index, attempts;
    bx_ntvdm_redir_mailslot_record *record = NULL;
    for (index = 0u; index < BX_NTVDM_REDIR_MAILSLOT_CAPACITY; ++index)
        if (g_mailslots[index].legacy_handle == 0u) { record = &g_mailslots[index]; break; }
    if (record == NULL) return NULL;
    for (attempts = 0u; attempts < UINT16_MAX; ++attempts) {
        uint16_t candidate = g_next_mailslot_handle++;
        if (candidate != 0u && find_mailslot(candidate) == NULL) {
            memset(record, 0, sizeof(*record));
            record->legacy_handle = candidate;
            return record;
        }
    }
    return NULL;
}

static void set_gpr16(struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    uint32_t index, uint16_t value)
{
    outcome->gpr16_write_mask |= (1u << index);
    outcome->gpr16_values[index] = value;
}

static int mailslot_make(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    char oem[260]; wchar_t name[260]; HANDLE host; DWORD error = ERROR_INVALID_HANDLE; uint32_t token = 0u;
    bx_ntvdm_redir_mailslot_record *record;
    if (!read_oem_string(word_at(event->ds), word_at(event->esi), oem, sizeof(oem)) ||
        !local_mailslot_name(oem, name, sizeof(name) / sizeof(name[0]))) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    record = allocate_mailslot();
    if (record == NULL) { resume_with_error(event, outcome, ERROR_TOO_MANY_OPEN_FILES); return 1; }
    host = CreateMailslotW(name, (DWORD)word_at(event->ebx),
        MAILSLOT_WAIT_FOREVER, NULL);
    if (host == INVALID_HANDLE_VALUE ||
        !g_active_session->direct->publish_handle(g_active_session->direct->state,
            host, &token, &error)) {
        if (host != INVALID_HANDLE_VALUE) CloseHandle(host);
        clear_mailslot(record);
        resume_with_error(event, outcome, host == INVALID_HANDLE_VALUE ? GetLastError() : error);
        return 1;
    }
    record->host_token = token;
    record->pdb = word_at(event->eax);
    record->message_size = word_at(event->ebx);
    record->slot_size = word_at(event->ecx);
    record->selector = word_at(event->edx);
    record->buffer_segment = word_at(event->es);
    record->buffer_offset = word_at(event->edi);
    resume_success(event, outcome);
    set_gpr16(outcome, 0u, record->legacy_handle);
    return 1;
}

static int mailslot_delete(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    bx_ntvdm_redir_mailslot_record *record = find_mailslot(word_at(event->ebx));
    DWORD error;
    if (record == NULL || record->pdb != word_at(event->eax)) {
        resume_with_error(event, outcome, ERROR_INVALID_HANDLE); return 1;
    }
    if (!g_active_session->direct->release_handle(g_active_session->direct->state,
            record->host_token, &error)) { resume_with_error(event, outcome, error); return 1; }
    resume_success(event, outcome);
    set_gpr16(outcome, 2u, record->selector); /* DX */
    set_gpr16(outcome, 7u, record->buffer_offset); /* DI */
    outcome->segment_write_mask |= 1u; /* ES */
    outcome->segment_values[0] = record->buffer_segment;
    clear_mailslot(record);
    return 1;
}

static int mailslot_info(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    bx_ntvdm_redir_mailslot_record *record = find_mailslot(word_at(event->ebx));
    HANDLE host; DWORD maximum, next, count, timeout;
    if (record == NULL || !g_active_session->direct->lookup_handle(
            g_active_session->direct->state, record->host_token, &host) ||
        !GetMailslotInfo(host, &maximum, &next, &count, &timeout)) {
        resume_with_error(event, outcome, record == NULL ? ERROR_INVALID_HANDLE : GetLastError()); return 1;
    }
    resume_success(event, outcome);
    set_gpr16(outcome, 0u, (uint16_t)maximum);
    set_gpr16(outcome, 3u, (uint16_t)record->slot_size);
    set_gpr16(outcome, 1u, next == MAILSLOT_NO_MESSAGE ? 0u : (uint16_t)next);
    set_gpr16(outcome, 2u, 0u);
    set_gpr16(outcome, 6u, (uint16_t)count);
    return 1;
}

static int mailslot_get_record(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_redir_mailslot_record **out_record, HANDLE *out_host)
{
    bx_ntvdm_redir_mailslot_record *record = find_mailslot(word_at(event->ebx));
    HANDLE host;
    if (record == NULL || !g_active_session->direct->lookup_handle(
            g_active_session->direct->state, record->host_token, &host)) {
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

/* Read one actual message into an owned record.  The original BOP contract
 * carries no host pointer; this is intentionally bounded by the 16-bit DOS
 * message-size value recorded by VrMakeMailslot. */
static int mailslot_fill_peek(bx_ntvdm_redir_mailslot_record *record, HANDLE host,
    DWORD *error)
{
    DWORD next, count, read = 0u, following = 0u;
    uint8_t *bytes;
    if (record->peek_bytes != NULL) return 1;
    if (!mailslot_query(host, &next, &count)) { *error = GetLastError(); return 0; }
    if (next == MAILSLOT_NO_MESSAGE || count == 0u) {
        record->peek_size = 0u;
        record->peek_next_size = 0u;
        return 1;
    }
    if (next > record->message_size || next > UINT16_MAX) {
        *error = ERROR_INSUFFICIENT_BUFFER;
        return 0;
    }
    bytes = (uint8_t *)malloc(next);
    if (bytes == NULL) { *error = ERROR_NOT_ENOUGH_MEMORY; return 0; }
    if (!ReadFile(host, bytes, next, &read, NULL)) {
        *error = GetLastError(); free(bytes); return 0;
    }
    if (!mailslot_query(host, &following, &count)) {
        *error = GetLastError(); free(bytes); return 0;
    }
    record->peek_bytes = bytes;
    record->peek_size = read;
    record->peek_next_size = following == MAILSLOT_NO_MESSAGE ? 0u : following;
    return 1;
}

static int mailslot_peek_or_read(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int destructive)
{
    bx_ntvdm_redir_mailslot_record *record;
    HANDLE host;
    DWORD error = ERROR_INVALID_HANDLE;
    if (!mailslot_get_record(event, outcome, &record, &host)) return 1;
    if (!mailslot_fill_peek(record, host, &error)) {
        resume_with_error(event, outcome, error); return 1;
    }
    if (record->peek_size != 0u && !guest_write_bytes(word_at(event->es),
            word_at(event->edi), record->peek_bytes, record->peek_size)) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    resume_success(event, outcome);
    set_gpr16(outcome, 0u, (uint16_t)record->peek_size);
    set_gpr16(outcome, 1u, (uint16_t)record->peek_next_size);
    set_gpr16(outcome, 2u, 0u); /* DOS mailslot priority */
    if (destructive) {
        free(record->peek_bytes);
        record->peek_bytes = NULL;
        record->peek_size = 0u;
        record->peek_next_size = 0u;
    }
    return 1;
}

static int mailslot_write(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    uint8_t descriptor[8];
    uint16_t buffer_offset, buffer_segment;
    uint32_t count = word_at(event->ecx);
    char oem[260]; wchar_t name[260]; HANDLE host; DWORD written;
    if (!read_oem_string(word_at(event->ds), word_at(event->esi), oem, sizeof(oem)) ||
        !local_mailslot_name(oem, name, sizeof(name) / sizeof(name[0])) ||
        !guest_read_bytes(word_at(event->es), word_at(event->edi), descriptor,
            sizeof(descriptor))) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    buffer_offset = (uint16_t)(descriptor[4] | ((uint16_t)descriptor[5] << 8));
    buffer_segment = (uint16_t)(descriptor[6] | ((uint16_t)descriptor[7] << 8));
    if (count > UINT16_MAX) { resume_with_error(event, outcome, ERROR_INVALID_PARAMETER); return 1; }
    host = CreateFileW(name, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (host == INVALID_HANDLE_VALUE) {
        resume_with_error(event, outcome, GetLastError()); return 1;
    }
    {
        uint8_t *bytes = count == 0u ? NULL : (uint8_t *)malloc(count);
        if ((count != 0u && bytes == NULL) ||
            (count != 0u && !guest_read_bytes(buffer_segment, buffer_offset, bytes, count)) ||
            !WriteFile(host, bytes, count, &written, NULL) || written != count) {
            DWORD error = bytes == NULL && count != 0u ? ERROR_NOT_ENOUGH_MEMORY : GetLastError();
            free(bytes); CloseHandle(host); resume_with_error(event, outcome, error); return 1;
        }
        free(bytes);
    }
    CloseHandle(host);
    resume_success(event, outcome);
    return 1;
}

static int mailslot_terminate(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    uint32_t index;
    for (index = 0u; index < BX_NTVDM_REDIR_MAILSLOT_CAPACITY; ++index) {
        bx_ntvdm_redir_mailslot_record *record = &g_mailslots[index];
        DWORD error;
        if (record->legacy_handle != 0u && record->pdb == word_at(event->eax)) {
            (void)g_active_session->direct->release_handle(
                g_active_session->direct->state, record->host_token, &error);
            clear_mailslot(record);
        }
    }
    resume_success(event, outcome);
    return 1;
}

int bx_ntvdm_redir_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    uint8_t service;
    if (!session_valid(g_active_session) || g_active_session->bound == 0u ||
        event == NULL || outcome == NULL || event->window_bytes < 4u ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    service = event->window[3];
    switch (service) {
    case 0x00u: /* SVC_RDRINITIALIZE */
        g_active_session->loaded = 1u;
        resume_success(event, outcome);
        return 1;
    case 0x01u: /* SVC_RDRUNINITIALIZE */
        reset_mailslots();
        g_active_session->loaded = 0u;
        g_active_session->mode = 0u;
        resume_success(event, outcome);
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
        return mailslot_make(event, outcome);
    case 0x09u: /* SVC_RDRDELETEMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_delete(event, outcome);
    case 0x0au: /* SVC_RDRGETMAILSLOTINFO */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_info(event, outcome);
    case 0x0cu: /* SVC_RDRPEEKMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_peek_or_read(event, outcome, 0);
    case 0x0du: /* SVC_RDRREADMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_peek_or_read(event, outcome, 1);
    case 0x0eu: /* SVC_RDRWRITEMAILSLOT */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_write(event, outcome);
    case 0x0fu: /* SVC_RDRTERMINATE / NetResetEnvironment */
        if (g_active_session->loaded == 0u) { resume_with_error(event, outcome, ERROR_INVALID_FUNCTION); return 1; }
        return mailslot_terminate(event, outcome);
    case 0x23u: /* SVC_RDRREADASYNCNMPIPE */
    case 0x24u: /* SVC_RDRWRITEASYNCNMPIPE */
        /* `namepipe.asm:MapNtHandle` obtains BP:BX from a guest SFT, and
         * `int5c.asm` later calls the 16-bit ANR.  Neither a raw SFT HANDLE
         * nor a raw guest callback may cross this shim.  Keep the source
         * failure result until those two proper owners publish typed seams. */
        resume_with_error(event, outcome, ERROR_INVALID_FUNCTION);
        return 1;
    default:
        /* 02..08 and 20/21 are intentionally one typed provider route, but
         * their VDMREDIR protocol body is absent.  Returning this original
         * API-style failure is safer than treating copied registers as a
         * guest pointer or claiming a pipe operation succeeded. */
        resume_with_error(event, outcome, ERROR_INVALID_FUNCTION);
        return 1;
    }
}

int bx_ntvdm_redir_loaded(void)
{
    return session_valid(g_active_session) && g_active_session->bound != 0u &&
        g_active_session->loaded != 0u;
}

BOOL bx_ntvdm_redir_load(void)
{
    /* The original DEM source requested VDMREDIR lazily.  The CLI has one
     * pre-bound provider rather than LoadLibrary; this merely admits that
     * source path and preserves a failure if the session is not available. */
    if (!session_valid(g_active_session) || g_active_session->bound == 0u) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    g_active_session->loaded = 1u;
    return TRUE;
}

LPSTR bx_ntvdm_redir_convert_local_nt_pipe_name(LPSTR existing, LPSTR name)
{
    const char *pipe, *tail;
    size_t tail_bytes, bytes;
    LPSTR result;
    if (name == NULL || !bx_ntvdm_redir_loaded()) {
        SetLastError(ERROR_INVALID_FUNCTION);
        return NULL;
    }
    /* Accept the original local UNC spelling and construct the public Win32
     * local-pipe spelling.  This replaces only VDMREDIR's absent allocation
     * helper; source demfile still owns recognition/open/error ordering. */
    pipe = strstr(name, "\\PIPE\\");
    if (pipe == NULL) { SetLastError(ERROR_BAD_PATHNAME); return NULL; }
    tail = pipe + 6u;
    tail_bytes = strlen(tail) + 1u;
    bytes = 9u + tail_bytes; /* \\\\.\\PIPE\\ plus the NUL-terminated tail. */
    if (existing != NULL && strlen(existing) + 1u >= bytes) {
        memmove(existing + 9u, tail, tail_bytes);
        memcpy(existing, "\\\\.\\PIPE\\", 9u);
        return existing;
    }
    result = (LPSTR)LocalAlloc(LMEM_FIXED, bytes);
    if (result == NULL) return NULL;
    memcpy(result, "\\\\.\\PIPE\\", 9u);
    memcpy(result + 9u, tail, tail_bytes);
    return result;
}

BOOL bx_ntvdm_redir_add_open_named_pipe_info(HANDLE file, LPSTR name)
{
    (void)name;
    if (!bx_ntvdm_redir_loaded() || file == NULL || file == INVALID_HANDLE_VALUE) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    return TRUE;
}

BOOL bx_ntvdm_redir_remove_open_named_pipe_info(HANDLE file)
{
    (void)file;
    return bx_ntvdm_redir_loaded() ? TRUE : FALSE;
}

int bx_ntvdm_redir_is_named_pipe_handle(HANDLE file)
{
    DWORD flags;
    if (!bx_ntvdm_redir_loaded() || file == NULL || file == INVALID_HANDLE_VALUE)
        return 0;
    return GetNamedPipeInfo(file, &flags, NULL, NULL, NULL) ? 1 : 0;
}

int bx_ntvdm_redir_read_named_pipe(HANDLE file, LPVOID buffer, DWORD count,
    DWORD *read_out, DWORD *error_out)
{
    DWORD read = 0u;
    BOOL ok;
    if (read_out != NULL) *read_out = 0u;
    if (error_out != NULL) *error_out = ERROR_INVALID_FUNCTION;
    if (!bx_ntvdm_redir_is_named_pipe_handle(file)) { SetLastError(ERROR_INVALID_HANDLE); return 0; }
    ok = ReadFile(file, buffer, count, &read, NULL);
    if (read_out != NULL) *read_out = read;
    if (error_out != NULL) *error_out = ok ? ERROR_SUCCESS : GetLastError();
    return ok ? 1 : 0;
}

int bx_ntvdm_redir_write_named_pipe(HANDLE file, LPVOID buffer, DWORD count,
    DWORD *written_out)
{
    DWORD written = 0u;
    BOOL ok;
    if (written_out != NULL) *written_out = 0u;
    if (!bx_ntvdm_redir_is_named_pipe_handle(file)) { SetLastError(ERROR_INVALID_HANDLE); return 0; }
    ok = WriteFile(file, buffer, count, &written, NULL);
    if (written_out != NULL) *written_out = written;
    return ok ? 1 : 0;
}
