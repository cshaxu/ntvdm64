#include <windows.h>
#include <stdio.h>
#include "ntvdm-exe/session/session.h"
#include "wow_user_session_binding.h"

static session owner;
static wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
static wow_user_session_binding registration;
static volatile LONG errors, child_bound;
static BOOL refuse_retirement;
static unsigned retirement_calls;
static char task_identity;

static BOOL WINAPI retire_fixture(PVOID context)
{
    if (context != &task_identity) return FALSE;
    ++retirement_calls;
    if (refuse_retirement) {
        SetLastError(ERROR_BUSY);
        return FALSE;
    }
    return TRUE;
}

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
    /* The production unbind callback is fallible. Explicit session detach
     * must not hide rejection, drop the hook, or make a retry impossible. */
    runtime.lifecycle = &task_identity;
    runtime.retire_thread = retire_fixture;
    wow_user_runtime_current()->thread =
        (struct wow_task_order_thread *)&task_identity;
    refuse_retirement = TRUE;
    CHECK(!wow_user_session_detach(&registration));
    CHECK(GetLastError() == ERROR_BUSY);
    CHECK(retirement_calls == 1);
    CHECK(registration.registered && registration.session == &owner &&
        registration.runtime == &runtime);
    CHECK(owner.thread_hook_count == 1);
    CHECK(wow_user_runtime_current() != NULL &&
        wow_user_runtime_current()->runtime == &runtime);
    CHECK(session_thread_current() == &owner);
    /* The generic worker-thread exit path must propagate the same rejection,
     * retaining counters/TLS rather than silently abandoning USER state. */
    {
        session_binding_diagnostic before, after;
        CHECK(session_binding_diagnostic_snapshot(&owner, &before));
        CHECK(!session_thread_unbind(&owner));
        CHECK(GetLastError() == ERROR_BUSY && retirement_calls == 2);
        CHECK(session_thread_current() == &owner);
        CHECK(wow_user_runtime_current() != NULL);
        CHECK(session_binding_diagnostic_snapshot(&owner, &after));
        CHECK(before.total == after.total &&
            before.softpc_entry == after.softpc_entry &&
            before.original_worker == after.original_worker);
    }
    refuse_retirement = FALSE;
    CHECK(session_thread_unbind(&owner));
    CHECK(retirement_calls == 3);
    CHECK(session_thread_current() == NULL && wow_user_runtime_current() == NULL);
    CHECK(owner.binding_count == 0 && registration.registered);
    CHECK(session_thread_bind_owned(&owner, SESSION_THREAD_BINDING_SOFTPC_ENTRY));
    CHECK(wow_user_session_detach(&registration));
    CHECK(retirement_calls == 3);
    CHECK(!registration.registered && owner.thread_hook_count == 0);
    CHECK(wow_user_runtime_current() == NULL);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    printf("WOW_USER_SESSION_BINDING errors=%ld child=%ld lifecycle=1\n",
        errors, child_bound);
    return errors ? 3 : 0;
}
