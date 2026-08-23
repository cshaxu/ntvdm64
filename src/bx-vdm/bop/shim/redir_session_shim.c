#include "redir_session_shim.h"

#include <string.h>

/* This is intentionally not a replacement VDMREDIR.DLL.  The provider body
 * was not recovered with OpenNT; the admitted first group supplies the
 * lifecycle and the DEM synchronous-pipe interception seam.  The 57 service
 * bodies below return the historical Win32 unavailable failure until their
 * protocol layouts are recovered as later owner groups. */

static bx_ntvdm_redir_native_session *g_active_session;

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

void bx_ntvdm_redir_native_session_unbind(bx_ntvdm_redir_native_session *session)
{
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
