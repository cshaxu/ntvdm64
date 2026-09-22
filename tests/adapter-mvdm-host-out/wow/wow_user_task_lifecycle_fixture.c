#include "wow_user_task_lifecycle.h"
#include "ntvdm-exe/session/session.h"

#include <stdio.h>

static unsigned errors;
#define CHECK(value) do { if (!(value)) { ++errors; \
    fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static VOID WINAPI end_task(void)
{
    ++errors; /* Registration retains, but does not invoke, this callback. */
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    wow_cleanup_window stale_window = {0};
    HWND native_window;
    HANDLE wowexec = CreateEventW(NULL, FALSE, FALSE, NULL);

    CHECK(wowexec != NULL);
    session_initialize(&owner, 42040u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, wowexec));
    CHECK(lifecycle.registered && lifecycle.process.pwpi != NULL);
    CHECK(wow_user_task_lifecycle_init(&lifecycle, 0x0400, "APP.EXE",
        0x1234, 0, 0, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_SHOW));
    CHECK(wow_user_runtime_current() == &binding && binding.thread != NULL);
    /* Exercise the recovered USER task-order owner after InitTask.  This is
     * deliberately not a synthetic wake: with no pending work, the original
     * yield path must retain its own immediate scheduler semantics. */
    CHECK(wow_user_task_lifecycle_yield(&lifecycle));
    /* The same task context enters the recovered original cleanup owner;
     * the empty enrolled-object case is a real lifecycle state, not a
     * substitute success callback. */
    CHECK(wow_user_task_lifecycle_wow_cleanup(&lifecycle,
        (HANDLE)(ULONG_PTR)0x1234, 0x1234, NULL, 0));
    /* A native object disappeared before its enrollment was retired. Do not
     * free the task underneath the still-enrolled cleanup owner on failure. */
    native_window = CreateWindowExA(0, "STATIC", "", WS_POPUP, 0, 0, 1, 1,
        NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(native_window != NULL);
    if (native_window) {
        WORD index = LOWORD(native_window);
        wow_cleanup_handle *entry = &lifecycle.objects->entries[index];
        struct wow_user_task_lifecycle_thread *task = lifecycle.threads;
        struct wow_task_order_thread *thread = binding.thread;
        CHECK(DestroyWindow(native_window));
        stale_window.thread = thread;
        entry->phead = &stale_window;
        entry->pOwner = thread;
        entry->bType = TYPE_WINDOW;
        entry->wUniq = HIWORD(native_window);
        lifecycle.objects->last_handle = index;
        CHECK(!wow_user_task_lifecycle_cleanup(&lifecycle, 0x1234));
        CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
        CHECK(lifecycle.threads == task && binding.thread == thread);
        CHECK(!binding.exclusive_held && lifecycle.cleanup.thread == NULL);
        if (lifecycle.threads == task && binding.thread == thread) {
            CHECK(thread->ptdb != NULL && entry->pOwner == thread);
            CHECK(WaitForSingleObject(thread->pEventQueueServer, 0) != WAIT_FAILED);
        }
        ZeroMemory(entry, sizeof(*entry));
        lifecycle.objects->last_handle = 0;
    }
    CHECK(wow_user_task_lifecycle_cleanup(&lifecycle, 0x1234));
    CHECK(binding.thread == NULL && lifecycle.threads == NULL);
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    fprintf(stderr, "WOW_USER_TASK_LIFECYCLE errors=%u\n", errors);
    return errors ? 1 : 0;
}
