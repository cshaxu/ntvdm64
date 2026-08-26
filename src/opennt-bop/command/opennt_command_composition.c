#include "opennt_command_composition.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

void cmdComSpec(void);
void cmdSaveWorld(void);
void cmdGetCurrentDir(void);
void cmdSetInfo(void);
void cmdGetKbdLayout(void);
void cmdGetStdHandle(void);
void cmdGetStartInfo(void);
void cmdGetConfigSys(void);
void cmdGetAutoexecBat(void);
void cmdGetInitEnvironment(void);
void cmdCheckBinary(void);
void cmdGetNextCmd(void);
void cmdExec(void);
void cmdExecComspec32(void);
void cmdReturnExitCode(void);
void cmdExitVDM(void);
BOOL CmdDispatch(ULONG service);

BOOL VDMForWOW;
WORD *pFDAccess;
BOOL bPifFastPaste;
ULONG DosSessionId;
USHORT nDrives;
PIF_DATA pfdata;
UINT VdmExitCode;

#pragma warning(push)
#pragma warning(disable: 4324) /* jmp_buf has platform-required alignment; this private stack record never crosses an ABI. */
typedef struct runtime_command_misc_active_call {
    runtime_command_misc_call *call;
    uint8_t *guest_buffer;
    uint32_t guest_address;
    uint32_t guest_bytes;
    uint32_t write_back;
    uint8_t *guest_buffer2;
    uint8_t *guest_buffer3;
    uint8_t *guest_buffer4;
    uint32_t guest_address2;
    uint32_t guest_bytes2;
    uint32_t guest_address3;
    uint32_t guest_bytes3;
    HANDLE local_child_job;
    jmp_buf terminal_exit;
} runtime_command_misc_active_call;
#pragma warning(pop)

static __declspec(thread) runtime_command_misc_active_call *g_active_call;
/* The product currently admits exactly one session.  This host-private
 * binding gives the imported worker a scoped session without retaining the
 * active BOP call, a guest pointer, or a raw HANDLE in its continuation. */
static runtime_command_misc_session *g_pending_session;
static __declspec(thread) runtime_command_misc_session *g_worker_session;

static uint32_t real_mode_address(USHORT segment, USHORT offset)
{
    return ((uint32_t)segment << 4) + (uint32_t)offset;
}

static int copy_guest_multisz(runtime_command_misc_active_call *active,
    uint32_t address, uint8_t **buffer_out, uint32_t *bytes_out)
{
    uint8_t *buffer;
    uint32_t index;
    if (active == NULL || buffer_out == NULL || bytes_out == NULL || *buffer_out != NULL ||
        address > 0x100000u - 2u) return 0;
    buffer = (uint8_t *)malloc(256u);
    if (buffer == NULL) return 0;
    for (index = 0u; index < USHRT_MAX; ++index) {
        uint8_t value;
        if (address > 0x100000u - 1u - index ||
            !active->call->guest_read(active->call->guest_state, address + index, &value, 1u)) {
            free(buffer);
            return 0;
        }
        if (index == 256u || (index > 256u && (index & (index - 1u)) == 0u)) {
            uint8_t *expanded = (uint8_t *)realloc(buffer, index * 2u);
            if (expanded == NULL) { free(buffer); return 0; }
            buffer = expanded;
        }
        buffer[index] = value;
        if (index != 0u && buffer[index - 1u] == 0u && value == 0u) {
            *buffer_out = buffer;
            *bytes_out = index + 1u;
            return 1;
        }
    }
    free(buffer);
    return 0;
}

static int set_ax(USHORT value)
{
    return g_active_call != NULL &&
        runtime_cpu_delta_set_gpr16(&g_active_call->call->result->cpu_delta,
            0u, value);
}

void runtime_command_misc_session_initialize(runtime_command_misc_session *session)
{
    if (session == NULL) return;
    memset(session, 0, sizeof(*session));
    session->magic = RUNTIME_COMMAND_MISC_SESSION_MAGIC;
    session->abi_version = RUNTIME_COMMAND_MISC_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
    session_input_initialize(&session->input);
    opennt_host_event_reset();
    session->handles = runtime_host_handle_manager_session();
    (void)runtime_host_handle_manager_initialize(session->handles);
}

void runtime_command_misc_session_dispose(runtime_command_misc_session *session)
{
    int cancelled = 0;
    if (!runtime_command_misc_session_valid(session)) return;
    if (session == g_pending_session) {
        HANDLE job = INVALID_HANDLE_VALUE;
        HANDLE worker = INVALID_HANDLE_VALUE;
        if (session->pending.state == RUNTIME_COMMAND_LOCAL_CHILD_PENDING) {
            /* A dispose can race CreateProcess.  Record intent in the fixed
             * continuation so worker_attach_process terminates a job which
             * is published after this point. */
            session->pending.cancel_requested = 1u;
            cancelled = 1;
            if (session->pending.job_token != 0u &&
                runtime_host_handle_manager_lookup_handle(session->handles,
                    session->pending.job_token, &job))
                (void)TerminateJobObject(job, ERROR_CANCELLED);
        }
        if (session->pending.worker_token != 0u &&
            runtime_host_handle_manager_lookup_handle(session->handles,
                session->pending.worker_token, &worker))
            (void)WaitForSingleObject(worker, INFINITE);
        if (cancelled) {
            session->local_child_state = RUNTIME_COMMAND_LOCAL_CHILD_CANCELLED;
            session->local_child_error = ERROR_CANCELLED;
            session->pending.state = RUNTIME_COMMAND_LOCAL_CHILD_CANCELLED;
        }
        g_pending_session = NULL;
    }
    session_input_dispose(&session->input);
    runtime_host_handle_manager_reset(session->handles);
}

int runtime_command_misc_session_valid(const runtime_command_misc_session *session)
{
    return session != NULL && session->magic == RUNTIME_COMMAND_MISC_SESSION_MAGIC &&
        session->abi_version == RUNTIME_COMMAND_MISC_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) &&
        session_input_valid(&session->input) &&
        runtime_host_handle_manager_valid(session->handles);
}

int runtime_command_misc_session_set_command_source(
    runtime_command_misc_session *session, const CHAR *application,
    const CHAR *tail, USHORT drive, USHORT code_page)
{
    if (!runtime_command_misc_session_valid(session) || application == NULL ||
        tail == NULL) return 0;
    return session_input_set_startup(&session->input, application, tail,
        drive, code_page);
}

int runtime_command_misc_session_set_command_environment(
    runtime_command_misc_session *session, const CHAR *environment,
    uint32_t bytes)
{
    if (!runtime_command_misc_session_valid(session)) return 0;
    return session_input_set_environment(&session->input, environment, bytes);
}

void GetWowKernelCmdLine(void)
{
    /* DIVERGENCE (T236 S5): the directly imported cmdGetNextCmd preserves
     * OpenNT's VDMForWOW branch and its non-returning terminal convention.
     * The original helper publishes krnl386 startup input for the separate
     * WOWEXEC/WOW32/NE-loader product composition.  This one-session CLI has
     * no admitted WOW root or consumer, so resuming would fabricate a WOW
     * launch.  Preserve the original terminal shape as a typed controlled
     * stop; the complete input/publication/loader contract transfers to the
     * OpenNT WOW16 owner package. */
    TerminateVDM();
}
ULONG runtime_command_misc_redirection_token(PREDIRCOMPLETE_INFO info)
{ return info == NULL ? 0u : runtime_command_misc_active_session()->redirection_token; }

BOOL runtime_command_worker_prepare_startup(STARTUPINFO *startup)
{
    runtime_command_misc_session *session;
    uint32_t index;
    uint32_t explicit_streams = 0u;
    HANDLE *targets[3];
    if (startup == NULL ||
        (session = runtime_command_misc_active_session()) == NULL) return FALSE;
    targets[0] = &startup->hStdError;
    targets[1] = &startup->hStdOutput;
    targets[2] = &startup->hStdInput;
    for (index = 0u; index < 3u; ++index) {
        uint32_t token = session->pending.standard_handle_tokens[index];
        if (token == UINT32_MAX) continue;
        /* DIVERGENCE (T236 S2): retain OpenNT's three-handle ordering but bind
         * endpoints to this child only. SetStdHandle would alter the CLI. */
        if (token == 0u || !runtime_host_handle_manager_lookup_handle(
                session->handles, token, targets[index])) {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }
        ++explicit_streams;
    }
    /* The original worker receives VDM-installed standard handles.  A sentinel
     * means this BOP supplied none, so do not force unrelated CLI handles into
     * the child; retain CreateProcess's normal default-stream behavior. */
    if (explicit_streams != 0u) startup->dwFlags |= STARTF_USESTDHANDLES;
    return TRUE;
}

BOOL runtime_command_create_process(LPCSTR application, LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes, LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles, DWORD creation_flags, LPVOID environment, LPCSTR current_directory,
    LPSTARTUPINFOA startup, LPPROCESS_INFORMATION process_information)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    const CHAR *entry = (const CHAR *)environment;
    uint32_t environment_bytes = 0u;
    uint32_t environment_flags = 0u;
    BOOL created;
    if (session != NULL) {
        session->create_process_attempted = 1u;
        session->create_process_last_error = ERROR_SUCCESS;
        if (entry != NULL) {
            while (environment_bytes < RUNTIME_COMMAND_CONTINUATION_ENV_MAX) {
                size_t entry_bytes = strlen(entry);
                if (entry_bytes == 0u) {
                    ++environment_bytes;
                    break;
                }
                if (entry_bytes >= RUNTIME_COMMAND_CONTINUATION_ENV_MAX - environment_bytes)
                    break;
                if (_strnicmp(entry, "COMSPEC=", 8u) == 0) environment_flags |= 0x01u;
                if (_strnicmp(entry, "SystemRoot=", 11u) == 0) environment_flags |= 0x02u;
                if (_strnicmp(entry, "PATH=", 5u) == 0) environment_flags |= 0x04u;
                environment_bytes += (uint32_t)entry_bytes + 1u;
                entry += entry_bytes + 1u;
            }
        }
        session->create_process_environment_bytes = environment_bytes;
        session->create_process_environment_flags = environment_flags;
    }
    /* DIVERGENCE (T236 S2): cmdexec.c's source buffers are explicitly ANSI.
     * Bind that historical contract to public CreateProcessA, avoiding a
     * build-wide TCHAR setting while preserving every original argument. */
    created = CreateProcessA(application, command, process_attributes,
        thread_attributes, inherit_handles, creation_flags, environment,
        current_directory, startup, process_information);
    if (session != NULL) session->create_process_last_error = created ?
        ERROR_SUCCESS : GetLastError();
    return created;
}

void runtime_command_worker_attach_process(HANDLE process)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    HANDLE job;
    DWORD error = ERROR_NOT_ENOUGH_MEMORY;
    if (session == NULL || process == NULL || process == INVALID_HANDLE_VALUE) return;
    job = CreateJobObjectA(NULL, NULL);
    if (job != NULL && AssignProcessToJobObject(job, process) &&
        runtime_host_handle_manager_publish(session->handles, job,
            RUNTIME_HOST_HANDLE_OWNED, &session->pending.job_token, &error)) {
        if (session->pending.cancel_requested != 0u)
            (void)TerminateJobObject(job, ERROR_CANCELLED);
        return;
    }
    if (job != NULL) CloseHandle(job);
}

void runtime_command_worker_finish(BOOL child_created, DWORD exit_code)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    HANDLE event = INVALID_HANDLE_VALUE;
    DWORD ignored;
    if (session == NULL) return;
    if (session->pending.job_token != 0u) {
        (void)runtime_host_handle_manager_release(session->handles,
            session->pending.job_token, &ignored);
        session->pending.job_token = 0u;
    }
    session->local_child_exit_code = exit_code;
    session->local_child_error = exit_code;
    session->local_child_state = child_created ?
        RUNTIME_COMMAND_LOCAL_CHILD_COMPLETED : RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
    session->pending.state = session->local_child_state;
    if (session->pending.completion_event_token != 0u &&
        runtime_host_handle_manager_lookup_handle(session->handles,
            session->pending.completion_event_token, &event))
        (void)SetEvent(event);
}

static int copy_pending_environment(CHAR *destination, uint32_t *bytes_out,
    const CHAR *source)
{
    uint32_t index;
    if (destination == NULL || bytes_out == NULL || source == NULL) return 0;
    for (index = 1u; index < RUNTIME_COMMAND_CONTINUATION_ENV_MAX; ++index) {
        destination[index - 1u] = source[index - 1u];
        if (source[index - 1u] == '\0' && source[index] == '\0') {
            destination[index] = '\0';
            *bytes_out = index + 1u;
            return 1;
        }
    }
    return 0;
}

static int snapshot_host_environment(runtime_command_misc_session *session)
{
    LPCH source;
    uint32_t bytes = 0u;
    if (session == NULL) return 0;
    source = GetEnvironmentStringsA();
    if (source == NULL) return 0;
    for (;;) {
        if (bytes >= RUNTIME_COMMAND_CONTINUATION_ENV_MAX) {
            FreeEnvironmentStringsA(source);
            return 0;
        }
        if (source[bytes++] == '\0' && source[bytes] == '\0') {
            ++bytes;
            break;
        }
    }
    /* DIVERGENCE (T236 S2): cmdXformEnvironment originally snapshots the
     * NTVDM process environment. Preserve that source input as a copied,
     * session-owned public Win32 snapshot: the guest DOS multisz remains the
     * separate pEnv32 input and is never substituted for the host snapshot. */
    if (!session_input_set_environment(&session->input, source, bytes)) {
        FreeEnvironmentStringsA(source);
        return 0;
    }
    FreeEnvironmentStringsA(source);
    return 1;
}

BOOL runtime_command_worker_begin(PCHAR command, PCHAR environment)
{
    runtime_command_misc_session *session;
    uint32_t tokens[3] = { UINT32_MAX, UINT32_MAX, UINT32_MAX };
    uint32_t address, index, command_bytes;
    HANDLE event, worker;
    DWORD error = ERROR_NOT_ENOUGH_MEMORY;
    if (g_active_call == NULL || command == NULL || environment == NULL ||
        (session = g_active_call->call->session) == NULL ||
        g_pending_session != NULL ||
        session->pending.state == RUNTIME_COMMAND_LOCAL_CHILD_PENDING) return FALSE;
    command_bytes = (uint32_t)strlen(command) + 1u;
    if (command_bytes == 0u || command_bytes > sizeof(session->pending.command)) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        return FALSE;
    }
    /* The copied OpenNT worker must receive the same immutable command and
     * double-NUL environment snapshot after the BOP call has returned. */
    memset(&session->pending, 0, sizeof(session->pending));
    /* A generation identifies an admitted BOP attempt, including an input
     * rejection before CreateThread.  This prevents a failed stream-token
     * request from being observationally invisible between two child runs. */
    session->pending.generation = ++session->local_child_generation;
    if (!copy_pending_environment(session->pending.environment,
            &session->pending.environment_bytes, environment)) {
        SetLastError(ERROR_BAD_ENVIRONMENT);
        return FALSE;
    }
    if (!snapshot_host_environment(session)) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (g_active_call->call->service == RUNTIME_COMMAND_MISC_EXEC) {
        address = real_mode_address(g_active_call->call->cpu->ss,
            (USHORT)g_active_call->call->cpu->ebp);
        if (address > 0x100000u - sizeof(tokens) ||
            !g_active_call->call->guest_read(g_active_call->call->guest_state, address,
                (uint8_t *)tokens, sizeof(tokens))) {
            SetLastError(ERROR_INVALID_HANDLE);
            session->local_child_state = RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
            session->local_child_error = ERROR_INVALID_HANDLE;
            session->pending.state = RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
            session->pending.error = ERROR_INVALID_HANDLE;
            return FALSE;
        }
        for (index = 0u; index < 3u; ++index) {
            HANDLE unused;
            if (tokens[index] != UINT32_MAX && (tokens[index] == 0u ||
                !runtime_host_handle_manager_lookup_handle(session->handles,
                    tokens[index], &unused))) {
                SetLastError(ERROR_INVALID_HANDLE);
                session->local_child_state = RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
                session->local_child_error = ERROR_INVALID_HANDLE;
                session->pending.state = RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
                session->pending.error = ERROR_INVALID_HANDLE;
                return FALSE;
            }
        }
    }
    session->pending.state = RUNTIME_COMMAND_LOCAL_CHILD_PENDING;
    session->pending.service = g_active_call->call->service;
    session->pending.command_bytes = command_bytes;
    memcpy(session->pending.command, command, command_bytes);
    memcpy(session->pending.standard_handle_tokens, tokens, sizeof(tokens));
    event = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (event == NULL || !runtime_host_handle_manager_publish(session->handles,
            event, RUNTIME_HOST_HANDLE_OWNED,
            &session->pending.completion_event_token, &error)) {
        if (event != NULL) CloseHandle(event);
        memset(&session->pending, 0, sizeof(session->pending));
        SetLastError(error);
        return FALSE;
    }
    g_pending_session = session;
    session->local_child_state = RUNTIME_COMMAND_LOCAL_CHILD_PENDING;
    session->local_child_error = ERROR_SUCCESS;
    worker = CreateThread(NULL, 0u, runtime_command_worker_thread, NULL, 0u, NULL);
    if (worker == NULL || !runtime_host_handle_manager_publish(session->handles,
            worker, RUNTIME_HOST_HANDLE_OWNED, &session->pending.worker_token,
            &error)) {
        if (worker != NULL) {
            (void)WaitForSingleObject(worker, INFINITE);
            CloseHandle(worker);
        }
        (void)runtime_host_handle_manager_release(session->handles,
            session->pending.completion_event_token, &error);
        memset(&session->pending, 0, sizeof(session->pending));
        g_pending_session = NULL;
        SetLastError(error);
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI runtime_command_worker_thread(LPVOID ignored)
{
    session_input *input;
    (void)ignored;
    g_worker_session = g_pending_session;
    if (g_worker_session == NULL) return ERROR_INVALID_STATE;
    input = &g_worker_session->input;
    /* cmdCreateProcess reaches the original GetNextVDMCommand re-entry
     * calls on this worker thread.  Bind only the neutral copied session
     * input for that duration: the facade retains the historical VDM API
     * shape without retaining an active BOP call or guest pointer. */
    if (!opennt_vdm_api_bind_input(input)) {
        g_worker_session = NULL;
        return ERROR_INVALID_STATE;
    }
    pCommand32 = g_worker_session->pending.command;
    pEnv32 = g_worker_session->pending.environment;
    cmdCreateProcess();
    opennt_vdm_api_unbind_input(input);
    g_worker_session = NULL;
    return 0u;
}

BOOL runtime_command_worker_complete(void)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    HANDLE worker;
    DWORD ignored;
    if (session == NULL || session != g_pending_session ||
        (session->pending.state != RUNTIME_COMMAND_LOCAL_CHILD_PENDING &&
         session->pending.state != RUNTIME_COMMAND_LOCAL_CHILD_COMPLETED &&
         session->pending.state != RUNTIME_COMMAND_LOCAL_CHILD_FAILED) ||
        session->pending.worker_token == 0u ||
        !runtime_host_handle_manager_lookup_handle(session->handles,
            session->pending.worker_token, &worker)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (WaitForSingleObject(worker, 0u) != WAIT_OBJECT_0) {
        SetLastError(ERROR_IO_INCOMPLETE);
        return FALSE;
    }
    (void)runtime_host_handle_manager_release(session->handles,
        session->pending.worker_token, &ignored);
    session->pending.worker_token = 0u;
    if (session->pending.completion_event_token != 0u) {
        (void)runtime_host_handle_manager_release(session->handles,
            session->pending.completion_event_token, &ignored);
        session->pending.completion_event_token = 0u;
    }
    g_pending_session = NULL;
    return session->pending.state == RUNTIME_COMMAND_LOCAL_CHILD_COMPLETED ||
        session->pending.state == RUNTIME_COMMAND_LOCAL_CHILD_FAILED;
}

BOOL runtime_command_worker_reentry_pending(void)
{
    runtime_command_misc_session *session = runtime_command_misc_active_session();
    /* A completed record is historical state until a worker token still owns
     * the pending continuation.  cmdExec32 releases that token as it consumes
     * the exact BOP re-entry; a later 54:08 is a fresh request, not a replay. */
    return session != NULL && session->pending.worker_token != 0u &&
        (session->pending.state ==
        RUNTIME_COMMAND_LOCAL_CHILD_PENDING || session->pending.state ==
        RUNTIME_COMMAND_LOCAL_CHILD_COMPLETED || session->pending.state ==
        RUNTIME_COMMAND_LOCAL_CHILD_FAILED);
}

BOOL runtime_command_misc_set_pending(void)
{
    return g_active_call != NULL && runtime_cpu_result_pending(
        g_active_call->call->result);
}

static int validate_comspec_input(const runtime_command_misc_call *call)
{
    uint8_t value;
    uint32_t address;
    uint32_t index;
    if (call == NULL) return 0;
    address = real_mode_address(call->cpu->ds, (USHORT)call->cpu->edx);
    if (address > 0x100000u - (RUNTIME_COMMAND_MISC_COMSPEC_MAX + 1u))
        return 0;
    for (index = 0u; index <= RUNTIME_COMMAND_MISC_COMSPEC_MAX; ++index) {
        if (!call->guest_read(call->guest_state, address + index, &value, 1u)) return 0;
        if (value == 0u) return 1;
    }
    return 0;
}

int runtime_command_misc_call_valid(const runtime_command_misc_call *call)
{
    return call != NULL && call->magic == RUNTIME_COMMAND_MISC_CALL_MAGIC &&
        call->abi_version == RUNTIME_COMMAND_MISC_CALL_VERSION &&
        call->struct_bytes == sizeof(*call) &&
         (call->service == RUNTIME_COMMAND_MISC_EXIT ||
          call->service == RUNTIME_COMMAND_MISC_GET_NEXT ||
          call->service == RUNTIME_COMMAND_MISC_COMSPEC ||
         call->service == RUNTIME_COMMAND_MISC_SAVE_WORLD ||
         call->service == RUNTIME_COMMAND_MISC_GET_CURRENT_DIR ||
         call->service == RUNTIME_COMMAND_MISC_SET_INFO ||
         call->service == RUNTIME_COMMAND_MISC_INIT_CONSOLE ||
         call->service == RUNTIME_COMMAND_MISC_GET_CONFIG_SYS ||
         call->service == RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT ||
          call->service == RUNTIME_COMMAND_MISC_GET_KBD_LAYOUT ||
          call->service == RUNTIME_COMMAND_MISC_CHECK_BINARY ||
          call->service == RUNTIME_COMMAND_MISC_EXEC ||
          call->service == RUNTIME_COMMAND_MISC_EXEC_COMSPEC32 ||
          call->service == RUNTIME_COMMAND_MISC_RETURN_EXIT_CODE ||
         call->service == RUNTIME_COMMAND_MISC_GET_INIT_ENVIRONMENT ||
         call->service == RUNTIME_COMMAND_MISC_GET_START_INFO ||
         call->service == 0x06u) &&
        call->boundary != NULL && runtime_exception_event_valid(call->boundary) &&
        call->cpu != NULL && runtime_cpu_state_valid(call->cpu) &&
        call->cpu->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        call->result != NULL && call->guest_read != NULL && call->guest_write != NULL &&
         ((call->service != RUNTIME_COMMAND_MISC_SET_INFO &&
           call->service != RUNTIME_COMMAND_MISC_GET_NEXT &&
          call->service != RUNTIME_COMMAND_MISC_EXEC &&
          call->service != RUNTIME_COMMAND_MISC_EXEC_COMSPEC32 &&
          call->service != RUNTIME_COMMAND_MISC_RETURN_EXIT_CODE &&
          call->service != RUNTIME_COMMAND_MISC_GET_CONFIG_SYS &&
          call->service != RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT) ||
         runtime_command_misc_session_valid(call->session));
}

USHORT runtime_command_misc_get_dx(void) { return (USHORT)g_active_call->call->cpu->edx; }
USHORT runtime_command_misc_get_bx(void) { return (USHORT)g_active_call->call->cpu->ebx; }
USHORT runtime_command_misc_get_cx(void) { return (USHORT)g_active_call->call->cpu->ecx; }
USHORT runtime_command_misc_get_si(void) { return (USHORT)g_active_call->call->cpu->esi; }
USHORT runtime_command_misc_get_ds(void) { return g_active_call->call->cpu->ds; }
USHORT runtime_command_misc_get_es(void) { return g_active_call->call->cpu->es; }
USHORT runtime_command_misc_get_ss(void) { return g_active_call->call->cpu->ss; }
USHORT runtime_command_misc_get_bp(void) { return (USHORT)g_active_call->call->cpu->ebp; }
USHORT runtime_command_misc_get_ax(void) { return (USHORT)g_active_call->call->cpu->eax; }
USHORT runtime_command_misc_get_cs(void) { return g_active_call->call->cpu->cs; }
USHORT runtime_command_misc_get_ip(void) { return (USHORT)g_active_call->call->cpu->eip; }
UCHAR runtime_command_misc_get_al(void) { return (UCHAR)(g_active_call->call->cpu->eax & 0xffu); }
UCHAR runtime_command_misc_get_ah(void) { return (UCHAR)((g_active_call->call->cpu->eax >> 8u) & 0xffu); }
void runtime_command_misc_set_ax(USHORT value) { (void)set_ax(value); }
void runtime_command_misc_set_al(USHORT value)
{ runtime_command_misc_set_ax((USHORT)((runtime_command_misc_get_ax() & 0xff00u) | (value & 0xffu))); }
void runtime_command_misc_set_cf(int value)
{ (void)runtime_cpu_result_set_cf(g_active_call->call->result, value); }
void runtime_command_misc_set_dx(USHORT value)
{ (void)runtime_cpu_delta_set_gpr16(&g_active_call->call->result->cpu_delta, 2u, value); }
void runtime_command_misc_set_bx(USHORT value)
{ (void)runtime_cpu_delta_set_gpr16(&g_active_call->call->result->cpu_delta, 3u, value); }
void runtime_command_misc_set_cx(USHORT value)
{ (void)runtime_cpu_delta_set_gpr16(&g_active_call->call->result->cpu_delta, 1u, value); }
void runtime_command_misc_set_ds(USHORT value)
{ (void)runtime_cpu_delta_set_segment(&g_active_call->call->result->cpu_delta, 3u, value); }
void runtime_command_misc_set_es(USHORT value)
{ (void)runtime_cpu_delta_set_segment(&g_active_call->call->result->cpu_delta, 0u, value); }
void runtime_command_misc_set_ip(USHORT value)
{
    /* The source body advances its staged CCPU IP. The outer Bochs resume is
     * already represented by the fixed typed result set by this invocation. */
    if (g_active_call != NULL)
        ((runtime_cpu_state *)g_active_call->call->cpu)->eip = value;
}
void runtime_command_misc_sas_load(ULONG address, UCHAR *target)
{
    if (g_active_call == NULL || target == NULL || address >= 0x100000u ||
        !g_active_call->call->guest_read(g_active_call->call->guest_state,
            address, target, 1u)) {
        if (target != NULL) *target = 0xffu;
    }
}
BOOL runtime_command_misc_dispatch_source_command(ULONG service)
{
    /* Retail cmddisp.c checks its table bound only in DBG builds. Preserve
     * the source dispatch table, but reject an unreadable/out-of-range staged
     * service at the modern checked-memory boundary before it can index it. */
    if (g_active_call == NULL || service >= 17u) {
        if (g_active_call != NULL) runtime_command_misc_set_cf(1);
        return FALSE;
    }
    g_active_call->call->service = service;
    return CmdDispatch(service);
}

runtime_command_misc_session *runtime_command_misc_active_session(void)
{
    return g_active_call != NULL ? g_active_call->call->session :
        g_worker_session;
}

PREDIRCOMPLETE_INFO runtime_command_misc_redirection_from_guest(uint32_t token)
{
    runtime_command_misc_session *session;
    if (g_active_call == NULL || (session = g_active_call->call->session) == NULL ||
        token == 0u || token != session->redirection_token) return NULL;
    return &session->redirection_info;
}

int runtime_command_misc_publish_handle(HANDLE handle)
{
    runtime_command_misc_session *session;
    uint32_t guest_handle;
    DWORD error;
    if (g_active_call == NULL || (session = g_active_call->call->session) == NULL ||
        !runtime_host_handle_manager_publish(session->handles, handle,
            RUNTIME_HOST_HANDLE_BORROWED, &guest_handle, &error)) return 0;
    /* The original guest ABI is BX:CX. Preserve it as a fixed-width token,
     * never as a truncated host HANDLE. */
    runtime_command_misc_set_cx((USHORT)guest_handle);
    runtime_command_misc_set_bx((USHORT)(guest_handle >> 16u));
    return 1;
}

void RcErrorDialogBox(UINT error, PVOID first, PVOID second)
{
    runtime_opennt_rc_error_dialog(error, (CHAR *)first, (CHAR *)second);
}
void TerminateVDM(void)
{
    /* OpenNT's terminal path does not return.  The typed composition models
     * that directly as a controlled stop instead of resuming after an error. */
    if (g_active_call != NULL) {
        (void)runtime_cpu_result_stop(g_active_call->call->result);
        longjmp(g_active_call->terminal_exit, 1);
    }
}
void nt_init_event_thread(void)
{
    if (g_active_call != NULL && g_active_call->call->session != NULL)
        g_active_call->call->session->console_initialized = 1u;
}

DWORD runtime_command_misc_get_environment_variable(LPSTR name,
    LPSTR buffer, DWORD bytes)
{
    DWORD result;
    if (name == NULL || buffer == NULL || bytes == 0u) return 0u;
    result = GetEnvironmentVariableA(name, buffer, bytes);
    if (result != 0u || name[0] != '=' || name[1] < 'A' || name[1] > 'Z' ||
        name[2] != ':' || name[3] != '\0') return result;
    /* There is no ambient hidden-drive environment entry. The active process
     * directory is the only public Win32 equivalent for the source fallback. */
    result = GetCurrentDirectoryA(bytes, buffer);
    if (result == 0u || result >= bytes || buffer[1] != ':' ||
        (buffer[0] != name[1] && buffer[0] != (CHAR)(name[1] + ('a' - 'A'))))
        return 0u;
    return result;
}
/* OpenNT cmdkeyb.c called the old NTVDM console-composition export
 * GetConsoleKeyboardLayoutNameA.  It is not linkable from the modern public
 * Win32 import libraries.  GetKeyboardLayoutNameA is the public supported
 * capability with the same current-layout-name result; keep this replacement
 * at the host shim and leave cmdkeyb.c's registry/failure algorithm intact. */
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    return GetKeyboardLayoutNameA(name);
}

LPVOID runtime_command_misc_get_vdm_addr(USHORT segment, USHORT offset)
{
    runtime_command_misc_active_call *active = g_active_call;
    uint32_t bytes;
    uint32_t index;
    if (active == NULL) return NULL;
    if (active->call->service == RUNTIME_COMMAND_MISC_EXEC ||
        active->call->service == RUNTIME_COMMAND_MISC_EXEC_COMSPEC32) {
        uint32_t address = real_mode_address(segment, offset);
        uint32_t maximum = active->call->service == RUNTIME_COMMAND_MISC_EXEC &&
            segment == active->call->cpu->ds && offset == (USHORT)active->call->cpu->esi ? 124u : USHRT_MAX;
        uint8_t **buffer = active->guest_buffer == NULL ? &active->guest_buffer : &active->guest_buffer2;
        uint32_t *buffer_address = active->guest_buffer == NULL ? &active->guest_address : &active->guest_address2;
        uint32_t *buffer_bytes = active->guest_buffer == NULL ? &active->guest_bytes : &active->guest_bytes2;
        if (*buffer != NULL || (maximum == 124u && address > 0x100000u - maximum)) return NULL;
        *buffer_address = address;
        if (maximum != 124u) {
            /* DIVERGENCE(BOP-DIV-036): the old 1 KiB probe was an adapter limit, not an
             * OpenNT environment contract.  Read a bounded DOS multisz and
             * retain only its exact copied extent for the CLI backend. */
            if (!copy_guest_multisz(active, address, buffer, buffer_bytes)) return NULL;
            return *buffer;
        }
        *buffer = (uint8_t *)calloc(maximum, 1u);
        if (*buffer == NULL || !active->call->guest_read(active->call->guest_state,
                address, *buffer, maximum)) return NULL;
        *buffer_bytes = maximum;
        return *buffer;
    }
    if (active->call->service == RUNTIME_COMMAND_MISC_GET_NEXT) {
        CMDINFO *info;
        uint32_t address = real_mode_address(segment, offset);
        uint8_t **buffer;
        uint32_t *buffer_address;
        uint32_t *buffer_bytes;
        if (active->guest_buffer == NULL) {
            if (address > 0x100000u - sizeof(CMDINFO)) return NULL;
            active->guest_buffer = (uint8_t *)calloc(sizeof(CMDINFO), 1u);
            if (active->guest_buffer == NULL || !active->call->guest_read(active->call->guest_state,
                    address, active->guest_buffer, sizeof(CMDINFO))) return NULL;
            active->guest_address = address;
            active->guest_bytes = sizeof(CMDINFO);
            active->write_back = 1u;
            return active->guest_buffer;
        }
        info = (CMDINFO *)active->guest_buffer;
        if (segment == info->ExecPathSeg && offset == info->ExecPathOff) {
            bytes = info->ExecPathSize;
            buffer = &active->guest_buffer2; buffer_address = &active->guest_address2;
            buffer_bytes = &active->guest_bytes2;
        } else if (segment == info->CmdLineSeg && offset == info->CmdLineOff) {
            bytes = info->CmdLineSize;
            buffer = &active->guest_buffer3; buffer_address = &active->guest_address2;
            buffer_bytes = &active->guest_bytes2;
        } else if (segment == info->EnvSeg && offset == 0u) {
            bytes = info->EnvSize;
            buffer = &active->guest_buffer4; buffer_address = &active->guest_address3;
            buffer_bytes = &active->guest_bytes3;
        } else return NULL;
        /* DIVERGENCE(BOP-DIV-037): each historical SAS pointer is materialized as a
         * bounded copied span.  This preserves cmdmisc.c's pointer order but
         * rejects an alias or an unbounded real-mode address. */
        if (bytes == 0u || address > 0x100000u - bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(bytes, 1u);
        if (*buffer == NULL) return NULL;
        *buffer_address = address;
        *buffer_bytes = bytes;
        return *buffer;
    }
    if (active->call->service == RUNTIME_COMMAND_MISC_CHECK_BINARY) {
        uint32_t address = real_mode_address(segment, offset);
        uint32_t binary_bytes = active->guest_bytes == 0u ? MAX_PATH :
            active->guest_bytes == MAX_PATH ? sizeof(PARAMBLOCK) : 129u;
        uint8_t **buffer = active->guest_bytes == 0u ? &active->guest_buffer :
            active->guest_bytes == MAX_PATH ? &active->guest_buffer2 : &active->guest_buffer3;
        if (address > 0x100000u - binary_bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(binary_bytes, 1u);
        if (*buffer == NULL || !active->call->guest_read(active->call->guest_state,
                address, *buffer, binary_bytes)) return NULL;
        if (active->guest_bytes == 0u) {
            if (memchr(*buffer, 0, binary_bytes) == NULL) return NULL;
            active->guest_bytes = MAX_PATH;
        } else if (active->guest_bytes == MAX_PATH) active->guest_bytes = MAX_PATH + 1u;
        return *buffer;
    }
    if (active->call->service == RUNTIME_COMMAND_MISC_GET_INIT_ENVIRONMENT) {
        uint32_t requested = (uint32_t)(USHORT)active->call->cpu->ebx << 4;
        active->guest_address = real_mode_address(segment, offset);
        if (active->guest_address > 0x100000u ||
            requested > 0x100000u - active->guest_address || active->guest_buffer != NULL)
            return NULL;
        active->guest_buffer = (uint8_t *)calloc(requested == 0u ? 1u : requested, 1u);
        if (active->guest_buffer == NULL) return NULL;
        active->guest_bytes = requested;
        active->write_back = 1u;
        return active->guest_buffer;
    }
    if (active->call->service == RUNTIME_COMMAND_MISC_GET_KBD_LAYOUT) {
        uint32_t keyboard_bytes = active->guest_buffer == NULL ? 128u : 300u;
        uint8_t **buffer = active->guest_buffer == NULL ? &active->guest_buffer : &active->guest_buffer2;
        uint32_t *address = active->guest_buffer == NULL ? &active->guest_address : &active->guest_address2;
        uint32_t *size = active->guest_buffer == NULL ? &active->guest_bytes : &active->guest_bytes2;
        *address = real_mode_address(segment, offset);
        if (*address > 0x100000u - keyboard_bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(keyboard_bytes, 1u);
        if (*buffer == NULL) return NULL;
        *size = keyboard_bytes;
        return *buffer;
    }
    active->guest_address = real_mode_address(segment, offset);
    if (active->call->service == RUNTIME_COMMAND_MISC_SET_INFO) {
        runtime_command_misc_session *session = active->call->session;
        if (active->guest_address > 0x100000u - sizeof(SCSINFO)) return NULL;
        if (active->guest_bytes == 0u) {
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, (uint8_t *)&session->scs_info,
                    sizeof(session->scs_info))) return NULL;
            session->scs_info_address = active->guest_address;
            active->guest_bytes = 1u;
            return &session->scs_info;
        }
        if (active->guest_bytes == 1u) {
            if (active->guest_address >= 0x100000u) return NULL;
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, &session->is_dos_binary, 1u)) return NULL;
            session->is_dos_binary_address = active->guest_address;
            active->guest_bytes = 2u;
            return &session->is_dos_binary;
        }
        if (active->guest_bytes == 2u) {
            if (active->guest_address > 0x100000u - sizeof(WORD)) return NULL;
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, (uint8_t *)&session->fd_access,
                    sizeof(session->fd_access))) return NULL;
            session->fd_access_address = active->guest_address;
            active->guest_bytes = 3u;
            return &session->fd_access;
        }
        return NULL;
    }
    if (active->guest_buffer != NULL) return NULL;
    bytes = active->call->service == RUNTIME_COMMAND_MISC_COMSPEC ?
        RUNTIME_COMMAND_MISC_COMSPEC_MAX + 1u :
        (active->call->service == RUNTIME_COMMAND_MISC_GET_CONFIG_SYS ||
         active->call->service == RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT) ?
        64u : RUNTIME_COMMAND_MISC_CURRENT_DIR_BYTES;
    if (active->guest_address > 0x100000u - bytes) return NULL;
    active->guest_buffer = (uint8_t *)calloc(bytes, 1u);
    if (active->guest_buffer == NULL) return NULL;
    active->guest_bytes = bytes;
    active->write_back = active->call->service == RUNTIME_COMMAND_MISC_GET_CURRENT_DIR ||
        active->call->service == RUNTIME_COMMAND_MISC_GET_CONFIG_SYS ||
        active->call->service == RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT;
    if (active->write_back) return active->guest_buffer;
    for (index = 0u; index < bytes; ++index) {
        if (!active->call->guest_read(active->call->guest_state,
                active->guest_address + index, active->guest_buffer + index, 1u))
            return NULL;
        if (active->guest_buffer[index] == 0u) return active->guest_buffer;
    }
    return NULL;
}

static int runtime_command_misc_invoke_internal(runtime_command_misc_call *call,
    void (*body)(void))
{
    runtime_command_misc_active_call active;
    if (!runtime_command_misc_call_valid(call) || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u || call->service >= 17u)
        return 0;
    if (call->service == RUNTIME_COMMAND_MISC_COMSPEC &&
        !validate_comspec_input(call)) return 0;
    memset(&active, 0, sizeof(active));
    IsFirstCall = call->first_call ? TRUE : FALSE;
    IsRepeatCall = call->service == RUNTIME_COMMAND_MISC_GET_NEXT && call->session != NULL &&
        call->session->input.repeat_pending != 0u;
    DosEnvCreated = IsRepeatCall;
    IsFirstVDM = TRUE;
    fBlock = FALSE;
    Exe32ActiveCount = 0u;
    nDrives = 1u;
    memset(&VDMInfo, 0, sizeof(VDMInfo));
    memset(&cmdVDMEnvBlk, 0, sizeof(cmdVDMEnvBlk));
    if (IsRepeatCall) {
        cmdVDMEnvBlk.cchEnv = call->session->input.transformed_environment_bytes;
        if (cmdVDMEnvBlk.cchEnv < 2u || call->session->input.transformed_environment == NULL)
            return 0;
        cmdVDMEnvBlk.lpszzEnv = (CHAR *)malloc(cmdVDMEnvBlk.cchEnv);
        if (cmdVDMEnvBlk.lpszzEnv == NULL) return 0;
        memcpy(cmdVDMEnvBlk.lpszzEnv, call->session->input.transformed_environment,
            cmdVDMEnvBlk.cchEnv);
    }
    VDMForWOW = call->vdm_for_wow ? TRUE : FALSE;
    DosSessionId = call->session != NULL ? call->session->dos_session_id : 0u;
    memset(lpszComSpec, 0, sizeof(lpszComSpec));
    cbComSpec = 0u;
    if (call->service != RUNTIME_COMMAND_MISC_COMSPEC && call->session != NULL) {
        memcpy(lpszComSpec, call->session->comspec, sizeof(lpszComSpec));
        cbComSpec = call->session->comspec_bytes;
    }
    runtime_cpu_result_pass_through(call->result);
    if (!runtime_cpu_result_resume(call->result, call->boundary->fault_rip + 4u))
        return 0;
    active.call = call;
    g_active_call = &active;
    if (call->session != NULL) {
        pSCSInfo = &call->session->scs_info;
        pSCS_ToSync = &call->session->scs_info.SCS_ToSync;
        pIsDosBinary = &call->session->is_dos_binary;
        pFDAccess = &call->session->fd_access;
    }
    /* Preserve OpenNT's original 17-slot cmddisp.c table rather than growing
     * an adapter-owned service recognizer.  The preceding range guard is the
     * required modern boundary check because the retail body checked this
     * index only in DBG builds. */
    if (setjmp(active.terminal_exit) == 0 &&
        (body != NULL ? (body(), 0) : !CmdDispatch(call->service))) {
        g_active_call = NULL;
        return 0;
    }
    if (call->service == RUNTIME_COMMAND_MISC_EXEC && active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes)) {
        free(active.guest_buffer); free(active.guest_buffer2); g_active_call = NULL; return 0;
    }
    if (call->service == RUNTIME_COMMAND_MISC_GET_NEXT && call->session != NULL) {
        call->session->input.repeat_pending = IsRepeatCall ? 1u : 0u;
        if (IsRepeatCall && cmdVDMEnvBlk.lpszzEnv != NULL && cmdVDMEnvBlk.cchEnv >= 2u) {
            if (!session_input_set_transformed_environment(&call->session->input,
                    cmdVDMEnvBlk.lpszzEnv, cmdVDMEnvBlk.cchEnv)) {
                free(cmdVDMEnvBlk.lpszzEnv);
                cmdVDMEnvBlk.lpszzEnv = NULL;
                g_active_call = NULL;
                return 0;
            }
            free(cmdVDMEnvBlk.lpszzEnv);
            cmdVDMEnvBlk.lpszzEnv = NULL;
        }
    }
    if (call->service == RUNTIME_COMMAND_MISC_GET_NEXT &&
        ((active.guest_buffer != NULL && !call->guest_write(call->guest_state,
             active.guest_address, active.guest_buffer, active.guest_bytes)) ||
         (active.guest_buffer2 != NULL && !call->guest_write(call->guest_state,
             real_mode_address(((CMDINFO *)active.guest_buffer)->ExecPathSeg,
                 ((CMDINFO *)active.guest_buffer)->ExecPathOff), active.guest_buffer2,
             ((CMDINFO *)active.guest_buffer)->ExecPathSize)) ||
         (active.guest_buffer3 != NULL && !call->guest_write(call->guest_state,
             real_mode_address(((CMDINFO *)active.guest_buffer)->CmdLineSeg,
                 ((CMDINFO *)active.guest_buffer)->CmdLineOff), active.guest_buffer3,
             ((CMDINFO *)active.guest_buffer)->CmdLineSize)) ||
         (active.guest_buffer4 != NULL && !call->guest_write(call->guest_state,
             active.guest_address3, active.guest_buffer4, active.guest_bytes3)))) {
        free(active.guest_buffer); free(active.guest_buffer2); free(active.guest_buffer3);
        free(active.guest_buffer4); g_active_call = NULL; return 0;
    }
    if (call->service == RUNTIME_COMMAND_MISC_CHECK_BINARY && call->session != NULL &&
        call->session->scs_info_address != 0u &&
        !call->guest_write(call->guest_state, call->session->scs_info_address,
            (const uint8_t *)&call->session->scs_info, sizeof(call->session->scs_info))) {
        free(active.guest_buffer); free(active.guest_buffer2); free(active.guest_buffer3); free(active.guest_buffer4);
        g_active_call = NULL; return 0;
    }
    if (call->service == RUNTIME_COMMAND_MISC_COMSPEC && call->session != NULL) {
        memcpy(call->session->comspec, lpszComSpec, sizeof(lpszComSpec));
        call->session->comspec_bytes = cbComSpec;
    }
    if (call->service == RUNTIME_COMMAND_MISC_GET_INIT_ENVIRONMENT) {
        uint32_t index;
        /* cmdenv.c receives only a segment pointer.  Recover the exact
         * multi-string extent it populated so a successful bounded request
         * does not write arbitrary zeroed capacity back into guest RAM. */
        active.write_back = 0u;
        for (index = 0u; index + 1u < active.guest_bytes; ++index) {
            if (active.guest_buffer[index] == 0u && active.guest_buffer[index + 1u] == 0u) {
                active.guest_bytes = index + 2u;
                active.write_back = 1u;
                break;
            }
        }
    }
    if (call->service == RUNTIME_COMMAND_MISC_SET_INFO && active.guest_bytes != 3u) {
        g_active_call = NULL;
        return 0;
    }
    if (active.write_back && active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes)) {
        free(active.guest_buffer); g_active_call = NULL; return 0;
    }
    if (call->service == RUNTIME_COMMAND_MISC_GET_KBD_LAYOUT &&
        ((active.guest_buffer != NULL && !call->guest_write(call->guest_state, active.guest_address, active.guest_buffer, active.guest_bytes)) ||
         (active.guest_buffer2 != NULL && !call->guest_write(call->guest_state, active.guest_address2, active.guest_buffer2, active.guest_bytes2)))) {
        free(active.guest_buffer); free(active.guest_buffer2); g_active_call = NULL; return 0;
    }
    free(active.guest_buffer);
    free(active.guest_buffer2);
    free(active.guest_buffer3);
    free(active.guest_buffer4);
    g_active_call = NULL;
    return runtime_cpu_result_valid(call->result);
}

int runtime_command_misc_invoke(runtime_command_misc_call *call)
{
    int result;
    if (call == NULL || (call->session != NULL &&
        !opennt_vdm_api_bind_input(&call->session->input))) return 0;
    result = runtime_command_misc_invoke_internal(call, NULL);
    if (call->session != NULL) opennt_vdm_api_unbind_input(&call->session->input);
    return result;
}

int runtime_command_misc_invoke_body(runtime_command_misc_call *call,
    void (*body)(void))
{
    int result;
    if (body == NULL || call == NULL || (call->session != NULL &&
        !opennt_vdm_api_bind_input(&call->session->input))) return 0;
    result = runtime_command_misc_invoke_internal(call, body);
    if (call->session != NULL) opennt_vdm_api_unbind_input(&call->session->input);
    return result;
}
