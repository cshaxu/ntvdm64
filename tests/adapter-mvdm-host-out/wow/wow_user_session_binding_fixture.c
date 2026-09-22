#include <windows.h>
#include <stdio.h>
#include "ntvdm-exe/session/session.h"
#include "wow_user_session_binding.h"

static session owner;
static wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
static wow_user_session_binding registration;
static volatile LONG errors, child_bound;

#define CHECK(value) do { if (!(value)) { InterlockedIncrement(&errors); \
    printf("FAIL line=%d\n", __LINE__); } } while (0)

static DWORD WINAPI child(LPVOID ignored)
{
    wow_user_runtime_thread *binding;
    UNREFERENCED_PARAMETER(ignored);
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    binding = wow_user_runtime_current();
    CHECK(binding != NULL && binding->runtime == &runtime);
    if (binding) {
        CHECK(wow_user_runtime_enter(binding));
        CHECK(wow_user_runtime_leave_for_callout(binding));
        CHECK(wow_user_runtime_reenter_after_callout(binding));
        CHECK(wow_user_runtime_leave(binding));
    }
    InterlockedIncrement(&child_bound);
    CHECK(session_thread_unbind(&owner));
    return errors ? 1 : 0;
}

int __cdecl main(void)
{
    HANDLE thread;
    DWORD exit_code;
    session_initialize(&owner, 42040u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner, SESSION_THREAD_BINDING_SOFTPC_ENTRY));
    CHECK(wow_user_session_attach(&registration, &owner, &runtime));
    CHECK(wow_user_runtime_current() != NULL);
    thread = CreateThread(NULL, 0, child, NULL, 0, NULL);
    CHECK(thread != NULL);
    if (thread) {
        CHECK(WaitForSingleObject(thread, 5000) == WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread, &exit_code) && exit_code == 0);
        CHECK(CloseHandle(thread));
    }
    CHECK(child_bound == 1);
    CHECK(wow_user_session_detach(&registration));
    CHECK(wow_user_runtime_current() == NULL);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    printf("WOW_USER_SESSION_BINDING errors=%ld child=%ld lifecycle=1\n",
        errors, child_bound);
    return errors ? 3 : 0;
}
