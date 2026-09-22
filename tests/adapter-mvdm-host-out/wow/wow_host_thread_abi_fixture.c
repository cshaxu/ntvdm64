#include <windows.h>
#include <stdio.h>

#include "ntvdm-exe/session/session.h"
#include "wow_user_session_binding.h"

extern HANDLE WINAPI host_CreateThread(LPSECURITY_ATTRIBUTES, DWORD,
    LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

static session owner;
static wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
static wow_user_session_binding registration;
static volatile LONG errors;
static volatile LONG ccpu_new_thread_calls;

#define CHECK(value) do { if (!(value)) { InterlockedIncrement(&errors); \
    printf("FAIL line=%d\n", __LINE__); } } while (0)

/* The original host source calls these CCPU lifecycle hooks around its public
 * thread creation API. Their CCPU implementation is outside this focused ABI
 * fixture; retain their observable call edge without substituting a thread
 * entry or a session binding. */
void ccpu386newthread(void)
{
    InterlockedIncrement(&ccpu_new_thread_calls);
}

void ccpu386exitthread(void)
{
}

LONG VdmUnhandledExceptionFilter(PEXCEPTION_POINTERS exception)
{
    UNREFERENCED_PARAMETER(exception);
    return EXCEPTION_EXECUTE_HANDLER;
}

static DWORD WINAPI original_winapi_worker(LPVOID parameter)
{
    DWORD expected = (DWORD)(ULONG_PTR)parameter;
    wow_user_runtime_thread *binding = wow_user_runtime_current();

    CHECK(expected == 0x4a21u);
    CHECK(session_thread_current() == &owner);
    CHECK(binding != NULL && binding->runtime == &runtime);
    if (binding != NULL) {
        CHECK(wow_user_runtime_enter(binding));
        CHECK(wow_user_runtime_leave(binding));
    }
    return expected;
}

int __cdecl main(void)
{
    HANDLE thread;
    DWORD thread_id;
    DWORD result;

    session_initialize(&owner, 42040u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_SOFTPC_ENTRY));
    CHECK(wow_user_session_attach(&registration, &owner, &runtime));

    thread = host_CreateThread(NULL, 0u, original_winapi_worker,
        (LPVOID)(ULONG_PTR)0x4a21u, 0u, &thread_id);
    CHECK(thread != NULL && thread_id != 0u);
    if (thread != NULL) {
        CHECK(WaitForSingleObject(thread, 5000u) == WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread, &result) && result == 0x4a21u);
        CHECK(CloseHandle(thread));
    }
    CHECK(ccpu_new_thread_calls == 1);
    CHECK(wow_user_session_detach(&registration));
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    printf("WOW_HOST_THREAD_WINAPI_ABI errors=%ld ccpu_new=%ld\n", errors,
        ccpu_new_thread_calls);
    return errors ? 3 : 0;
}
