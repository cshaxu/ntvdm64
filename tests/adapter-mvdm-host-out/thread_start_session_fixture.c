#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "opennt-abi/host-compat/include/thread_start_compat.h"
#include "ntvdm-exe/session/session.h"

static DWORD __cdecl bound_worker(LPVOID parameter)
{
    session *expected = (session *)parameter;
    return session_thread_current() == expected ? 0u : 1u;
}

static session *void_owner;
static LONG void_errors, released, terminal_releases;
static DWORD WINAPI bound_winapi_worker(LPVOID parameter)
{
    return bound_worker(parameter);
}
static VOID __cdecl bound_void_worker(VOID)
{
    if (session_thread_current() != void_owner) InterlockedIncrement(&void_errors);
}
static VOID __cdecl bound_parameter_worker(LPVOID parameter)
{
    if (session_thread_current() != parameter) InterlockedIncrement(&void_errors);
}
static DWORD WINAPI explicit_exit_worker(LPVOID parameter)
{
    opennt_exit_thread(bound_worker(parameter));
    return 99; /* A native exit must not return. */
}

static int reject_unbind, reject_bind;
static LONG cpu_releases, cpu_order_errors;
void __cdecl opennt_ccpu_exit_thread(void);
/* Link the actual pre-CCPU adapter against this terminal witness. It checks
 * ordering only, not execution of original CPU allocation/list cleanup. */
void __cdecl ccpu386exitthread(void)
{
    if (session_thread_current() || terminal_releases != released ||
            session_thread_is_finishing()) InterlockedIncrement(&cpu_order_errors);
    InterlockedIncrement(&cpu_releases);
}
static DWORD WINAPI cpu_exit_worker(LPVOID parameter)
{
    if (bound_worker(parameter)) return 1;
    opennt_ccpu_exit_thread();
    return session_thread_current() ? 2 : 0;
}
static int bind_hook(void *context) { (void)context; return !reject_bind; }
static int unbind_hook(void *context)
{
    (void)context;
    if (reject_unbind == 2) {
        RaiseException(0xe0447003, 0, 0, NULL);
        return 0;
    }
    if (reject_unbind) { SetLastError(ERROR_BUSY); return 0; }
    if (session_thread_is_finishing()) InterlockedIncrement(&terminal_releases);
    InterlockedIncrement(&released);
    return 1;
}

static int verify_rejected_exit(void)
{
    char path[MAX_PATH], command[MAX_PATH + 32];
    STARTUPINFOA startup = {sizeof(startup)};
    PROCESS_INFORMATION process = {0};
    DWORD code = 0;
    if (!GetModuleFileNameA(NULL, path, sizeof(path))) return 10;
    if (sprintf_s(command, sizeof(command), "\"%s\" --reject", path) < 0) return 11;
    if (!CreateProcessA(path, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
            NULL, NULL, &startup, &process)) return 12;
    if (WaitForSingleObject(process.hProcess, 5000) != WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess, 99); /* Only this owned child. */
        WaitForSingleObject(process.hProcess, 5000);
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
        return 13;
    }
    if (!GetExitCodeProcess(process.hProcess, &code)) code = 0;
    CloseHandle(process.hThread); CloseHandle(process.hProcess);
    return code == 0xc0000420 ? 0 : 14;
}

int main(int argc, char **argv)
{
    session instance;
    HANDLE thread;
    DWORD exit_code = 0u;
    unsigned kind;

    reject_unbind = argc == 2 && strcmp(argv[1], "--reject") == 0;
    if (reject_unbind) SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    session_initialize(&instance, 1u);
    if (!session_activate(&instance)) return 1;
    if (!session_register_thread_hook(&instance, bind_hook, unbind_hook, NULL)) return 5;
    if (!session_thread_bind(&instance)) return 1;
    void_owner = &instance;
    /* Current product has one registered WOW hook. Its bind rejection must
     * fully roll back before each original ABI returns from its host thread.
     * No multi-hook hypothetical is promoted to a production failure. */
    reject_bind = 1;
    for (kind = 0; kind != 4; ++kind) {
        session_binding_diagnostic bindings;
        switch (kind) {
        case 0: thread = opennt_create_cdecl_thread(NULL, 0u, bound_worker,
            &instance, 0u, NULL); break;
        case 1: thread = opennt_create_winapi_thread_named(NULL, 0u,
            bound_winapi_worker, &instance, 0u, NULL, "fixture-rejected-start"); break;
        case 2: thread = opennt_create_void_cdecl_thread(NULL, 0u,
            bound_void_worker, NULL, 0u, NULL); break;
        default: thread = opennt_create_void_cdecl_parameter_thread(NULL, 0u,
            bound_parameter_worker, &instance, 0u, NULL); break;
        }
        if (!thread || WaitForSingleObject(thread,5000) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(thread,&exit_code) || exit_code != ERROR_INVALID_STATE)
            return 20;
        CloseHandle(thread);
        if (!session_binding_diagnostic_snapshot(&instance,&bindings) ||
            bindings.total != 1 || released || terminal_releases ||
            session_thread_current() != &instance) return 21;
    }
    reject_bind = 0;
    puts("THREAD_START_ROLLBACK rejected=4 owner_count=1 no_worker_entry=1");
    for (kind = 0; kind != 5; ++kind) {
        switch (kind) {
        case 0: thread = opennt_create_cdecl_thread(NULL, 0u, bound_worker,
            &instance, 0u, NULL); break;
        case 1: thread = opennt_create_winapi_thread_named(NULL, 0u,
            bound_winapi_worker, &instance, 0u, NULL, "fixture-winapi"); break;
        case 2: thread = opennt_create_void_cdecl_thread(NULL, 0u,
            bound_void_worker, NULL, 0u, NULL); break;
        case 3: thread = opennt_create_void_cdecl_parameter_thread(NULL, 0u,
            bound_parameter_worker, &instance, 0u, NULL); break;
        default: thread = opennt_create_winapi_thread_named(NULL, 0u,
            explicit_exit_worker, &instance, 0u, NULL, "fixture-exit"); break;
        }
        if (thread == NULL) return 2;
        if (WaitForSingleObject(thread, 5000u) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(thread, &exit_code) || exit_code != 0u) return 3;
        CloseHandle(thread);
        if (void_errors || released != (LONG)kind + 1 ||
                terminal_releases != released || session_thread_is_finishing() ||
                session_thread_current() != &instance) return 7;
    }
    thread = opennt_create_winapi_thread_named(NULL,0,cpu_exit_worker,&instance,
        0,NULL,"fixture-pre-ccpu");
    if (!thread || WaitForSingleObject(thread,5000) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread,&exit_code) || exit_code || cpu_order_errors ||
        cpu_releases != 1 || released != 6 || terminal_releases != 6) return 22;
    CloseHandle(thread);
    puts("THREAD_PRE_CCPU actual_adapter=1 unbind_before_cpu=1 once=1");
    /* A refused finish must retain the binding but clear its dynamic intent;
     * the same owner can still perform an ordinary retryable detach. */
    reject_unbind = 1;
    if (session_thread_finish(&instance) || session_thread_is_finishing() ||
            session_thread_current() != &instance) return 8;
    reject_unbind = 2;
    __try {
        (void)session_thread_finish(&instance);
        return 15;
    } __except (GetExceptionCode() == 0xe0447003 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        if (session_thread_is_finishing() ||
                session_thread_current() != &instance) return 16;
    }
    reject_unbind = 0;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 4;
    if (terminal_releases != 6 || released != 7 ||
            session_thread_is_finishing()) return 9;
    if (verify_rejected_exit()) return 6;
    puts("THREAD_TERMINAL_UNBIND normal=5 once=1 rejected_exit=c0000420");
    puts("THREAD_TERMINAL_INTENT terminal=5 ordinary=1 rejected_reset=1 exception_reset=1");
    return 0;
}
