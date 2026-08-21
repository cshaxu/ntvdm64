#include "bop/shim/command_misc_shim.h"

#include <stdio.h>
#include <string.h>

typedef struct t236_fixture_context {
    uint8_t guest[0x10000];
} t236_fixture_context;

static int read_guest(void *state, uint32_t address, uint8_t *buffer,
    uint32_t bytes)
{
    t236_fixture_context *context = (t236_fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes);
    return 1;
}

static int write_guest(void *state, uint32_t address, const uint8_t *buffer,
    uint32_t bytes)
{
    t236_fixture_context *context = (t236_fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes);
    return 1;
}

static int invoke(t236_fixture_context *context,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, bx_ntvdm_command_misc_session *session,
    uint32_t service)
{
    bx_ntvdm_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = BX_NTVDM_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = service;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = read_guest;
    call.guest_write = write_guest;
    call.session = session;
    return bx_ntvdm_command_misc_invoke(&call);
}

static int invoke_pending_completion(t236_fixture_context *context,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, bx_ntvdm_command_misc_session *session,
    uint32_t service)
{
    uint32_t attempt;
    if (!invoke(context, event, cpu, result, session, service) ||
        result->disposition != BX_NTVDM_CPU_RESULT_V2_PENDING) return 0;
    for (attempt = 0u; attempt < 100u; ++attempt) {
        Sleep(10u);
        if (!invoke(context, event, cpu, result, session, service)) return 0;
        if (result->disposition != BX_NTVDM_CPU_RESULT_V2_PENDING) return 1;
    }
    return 0;
}

int main(void)
{
    t236_fixture_context context;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_command_misc_session session;
    uint32_t *standard_handles;
    uint32_t standard_token;
    DWORD error, bytes;
    DWORD old_comspec_bytes;
    CHAR old_comspec[MAX_PATH], test_comspec[MAX_PATH], pipe_text[64];
    HANDLE pipe_read, pipe_write;
    HANDLE host_stdin, host_stdout, host_stderr;
    SECURITY_ATTRIBUTES security;

    memset(&context, 0, sizeof(context));
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = 0x500u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_command_misc_session_initialize(&session);
    host_stdin = GetStdHandle(STD_INPUT_HANDLE);
    host_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    host_stderr = GetStdHandle(STD_ERROR_HANDLE);

    cpu.ds = 0x100u;
    cpu.esi = 0u;
    cpu.es = 0x200u;
    cpu.ss = 0x600u;
    cpu.ebp = 0u;
    cpu.eax = 0x0102u;
    standard_handles = (uint32_t *)(context.guest + 0x6000u);
    standard_handles[0] = UINT32_MAX;
    standard_handles[1] = UINT32_MAX;
    standard_handles[2] = UINT32_MAX;
    /* Model cmdCheckStandardHandles' already-established redirection state.
     * cmdExec32 must preserve that source value through its completion
     * notification without changing the CLI process standard handles. */
    fSoftpcRedirection = TRUE;
    memcpy(context.guest + 0x1000u, "exit 37\r", 8u);
    memcpy(context.guest + 0x2000u, "T236=local\0\0", 12u);

    if (!invoke_pending_completion(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC)) return 11;
    if (result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 12;
    if ((result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) return 13;
    if (result.cpu_delta.gpr16_values[0] != (0x0100u | 37u)) return 14;
    if (context.guest[0x1007u] != 0u) return 15;
    if (session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_COMPLETED) return 16;
    if (session.local_child_generation != 1u) return 17;
    if (session.local_child_exit_code != 37u) return 18;
    if (session.local_child_events_blocked != 0u) return 19;
    if (session.local_child_stdout_redirected != 1u ||
        session.local_child_std_handle_notification_count == 0u) return 23;
    if (session.local_child_reentrancy != 0u || session.local_child_reentrancy_peak != 1u)
        return 20;
    if (GetStdHandle(STD_INPUT_HANDLE) != host_stdin ||
        GetStdHandle(STD_OUTPUT_HANDLE) != host_stdout ||
        GetStdHandle(STD_ERROR_HANDLE) != host_stderr) return 21;

    /* The guest instruction would already have resumed here.  Deliberately
     * reinvoking the same route must not consume completion twice or launch
     * another child from stale continuation state. */
    if (!invoke(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC) ||
        session.local_child_generation != 1u ||
        session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_COMPLETED)
        return 22;

    fSoftpcRedirection = FALSE;
    old_comspec_bytes = GetEnvironmentVariableA("COMSPEC", old_comspec,
        (DWORD)sizeof(old_comspec));
    if (old_comspec_bytes == 0u || old_comspec_bytes >= sizeof(old_comspec) ||
        _snprintf_s(test_comspec, sizeof(test_comspec), _TRUNCATE,
            "%s /c exit 41", old_comspec) < 0 ||
        !SetEnvironmentVariableA("COMSPEC", test_comspec)) return 2;
    cpu.eax = 0x0002u;
    if (!invoke_pending_completion(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC_COMSPEC32) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        result.cpu_delta.gpr16_values[0] != 41u ||
        session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_COMPLETED ||
        session.local_child_generation != 2u ||
        session.local_child_exit_code != 41u ||
        session.local_child_stdout_redirected != 0u ||
        session.local_child_std_handle_notification_count < 2u) {
        SetEnvironmentVariableA("COMSPEC", old_comspec);
        return 2;
    }
    SetEnvironmentVariableA("COMSPEC", old_comspec);

    standard_handles[1] = 0x00010000u;
    memcpy(context.guest + 0x1000u, "exit 0\r", 7u);
    if (!invoke(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        (result.cpu_delta.gpr16_values[0] & 0xffu) !=
            (ERROR_INVALID_HANDLE & 0xffu) ||
        session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_FAILED ||
        session.local_child_error != ERROR_INVALID_HANDLE ||
        session.local_child_events_blocked != 0u) {
        return 3;
    }

    security.nLength = sizeof(security);
    security.lpSecurityDescriptor = NULL;
    security.bInheritHandle = TRUE;
    if (!CreatePipe(&pipe_read, &pipe_write, &security, 0u) ||
        !SetHandleInformation(pipe_read, HANDLE_FLAG_INHERIT, 0u) ||
        !bx_ntvdm_host_handle_manager_publish(&session.handles, pipe_write,
            BX_NTVDM_HOST_HANDLE_BORROWED, &standard_token, &error)) return 4;
    standard_handles[1] = standard_token;
    cpu.eax = 0x0102u;
    memcpy(context.guest + 0x1000u, "echo T236\r", 10u);
    if (!invoke_pending_completion(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC)) return 51;
    if ((result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) return 52;
    if (session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_COMPLETED) return 53;
    if (session.local_child_generation != 4u) return 54;
    if (session.local_child_events_blocked != 0u) return 55;
    if (!bx_ntvdm_host_handle_manager_release(&session.handles, standard_token,
            &error)) return 56;
    CloseHandle(pipe_write);
    memset(pipe_text, 0, sizeof(pipe_text));
    if (!ReadFile(pipe_read, pipe_text, sizeof(pipe_text) - 1u, &bytes, NULL) ||
        strstr(pipe_text, "T236") == NULL) {
        CloseHandle(pipe_read);
        return 6;
    }
    CloseHandle(pipe_read);

    standard_handles[1] = UINT32_MAX;
    /* Do not rely on PATH: the worker deliberately receives the current
     * session's bounded environment, and this cancellation case needs a
     * child that remains alive long enough to cancel on every host. */
    if (GetSystemDirectoryA(pipe_text, (DWORD)sizeof(pipe_text)) == 0u ||
        _snprintf_s((CHAR *)(context.guest + 0x1000u), 124u, _TRUNCATE,
            "%s\\ping.exe -n 5 127.0.0.1 >nul\r", pipe_text) < 0) return 57;
    if (!invoke(&context, &event, &cpu, &result, &session,
            BX_NTVDM_COMMAND_MISC_EXEC) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PENDING) return 57;
    /* Disposing a single-session host composition is cancellation, not a
     * detached host process.  The worker receives the intent even if this
     * call races before its Job object has been published. */
    Sleep(50u);
    bx_ntvdm_command_misc_session_dispose(&session);
    if (session.local_child_state != BX_NTVDM_COMMAND_LOCAL_CHILD_CANCELLED ||
        session.local_child_error != ERROR_CANCELLED ||
        session.pending.state != BX_NTVDM_COMMAND_LOCAL_CHILD_CANCELLED ||
        session.handles.entry_count != 0u) return 58;

    puts("T236 S2 pending imported worker, opaque-handle stream isolation, direct/COMSPEC, failure, pipe, double-completion and cancellation contracts verified");
    return 0;
}
