#include "wow_user_task_lifecycle.h"
#include "wow_user_private_access.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/session/session.h"

#include <stdio.h>

static unsigned errors;
static unsigned native_direct_calls;
#define CHECK(value) do { if (!(value)) { ++errors; \
    fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static VOID WINAPI end_task(void)
{
    ++errors; /* Registration retains, but does not invoke, this callback. */
}

static DWORD WINAPI delayed_input(void *parameter)
{
    Sleep(50);
    return PostThreadMessageA((DWORD)(ULONG_PTR)parameter, WM_APP + 52, 19, 0)
        ? 0 : 1;
}

static LRESULT CALLBACK reentrant_receive(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_APP + 62) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        wow_user_task_lifecycle *lifecycle = binding->runtime->lifecycle;
        CHECK(!binding->exclusive_held);
        CHECK(lifecycle->process.pwpi->CSOwningThread == binding->thread);
        /* Do not enter a known-held nonrecursive lock in the red test. */
        if (!binding->exclusive_held)
            CHECK(wow_user_task_lifecycle_yield(lifecycle));
        CHECK(!binding->exclusive_held);
        CHECK(lifecycle->process.pwpi->CSOwningThread == binding->thread);
        return 62;
    }
    if (message == WM_APP + 63) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        wow_task_order_state *state = binding->thread->ppi->pwpi;
        CHECK(!InSendMessage());
        CHECK(!binding->exclusive_held);
        CHECK(state->CSOwningThread == binding->thread);
        CHECK(state->ptiScheduled == binding->thread);
        ++native_direct_calls;
        if (wp) return wow_native_SendMessageA(window, message, 0, lp) + 1;
        SetLastError(0x5678);
        return lp;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static DWORD WINAPI send_reentrant(void *window)
{
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(window, WM_APP + 62, 0, 0,
        SMTO_ABORTIFHUNG, 3000, &result) && result == 62 ? 0 : 1;
}

static void verify_receive_callout(wow_user_runtime_thread *binding)
{
    WNDCLASSW cls = {0};
    HWND window;
    HANDLE sender;
    DWORD code;
    cls.lpfnWndProc = reentrant_receive;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = L"WOW_RECEIVE_REENTRY";
    CHECK(RegisterClassW(&cls) != 0);
    window = CreateWindowExW(0, cls.lpszClassName, L"", 0, 0, 0, 1, 1,
        HWND_MESSAGE, NULL, cls.hInstance, NULL);
    CHECK(window != NULL);
    {
        WW words = {0};
        wow_window_words_binding *borrow;
        wow_task_order_state *state = binding->thread->ppi->pwpi;
        DWORD events = binding->thread->ptdb->nEvents;
        DWORD shared_events = binding->thread->ppi->shared->nEvents;
        wow_task_order_message *previous = binding->thread->psmsSent;
        DWORD_PTR returned = 0;
        /* Real production wrapper and native SendMessage, with a test-only
         * WND owner association. No guest projection/CCPU claim is made. */
        CHECK(wow_window_words_attach(window, &words));
        borrow = wow_window_words_acquire(window);
        CHECK(borrow != NULL);
        if (borrow) {
            wow_window_words_cleanup_value(borrow)->thread = binding->thread;
            CHECK(wow_native_SendMessageA(window, WM_APP + 63, 1, 113) == 114);
            CHECK(GetLastError() == 0x5678);
            CHECK(wow_native_SendMessageTimeoutA(window, WM_APP + 63, 0, 114,
                SMTO_ABORTIFHUNG, 1000, &returned) != 0 && returned == 114);
            CHECK(native_direct_calls == 3);
            CHECK(wow_user_runtime_enter(binding));
            CHECK(wow_native_SendMessageA(window, WM_APP + 63, 0, 114) == 114);
            CHECK(binding->exclusive_held);
            CHECK(wow_user_runtime_leave(binding));
            CHECK(native_direct_calls == 4);
            CHECK(state->ptiScheduled == binding->thread &&
                state->CSOwningThread == binding->thread);
            CHECK(binding->thread->ptdb->nEvents == events &&
                binding->thread->ppi->shared->nEvents == shared_events);
            CHECK(binding->thread->psmsSent == previous && !binding->exclusive_held);
            wow_window_words_detach(window);
            wow_window_words_release(borrow);
        }
    }
    sender = CreateThread(NULL, 0, send_reentrant, window, 0, NULL);
    CHECK(sender != NULL);
    CHECK(MsgWaitForMultipleObjectsEx(0, NULL, 3000, QS_SENDMESSAGE,
        MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0);
    CHECK(wow_user_runtime_enter(binding));
    binding->thread->host->receive(binding->thread);
    CHECK(binding->exclusive_held);
    CHECK(wow_user_runtime_leave(binding));
    CHECK(WaitForSingleObject(sender, 3000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(sender, &code) && code == 0);
    CHECK(CloseHandle(sender));
    CHECK(DestroyWindow(window));
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    wow_cleanup_window stale_window = {0};
    wow_task_order_waiter idle_waiter = {0};
    HWND native_window;
    MSG message;
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
    /* Original shared WOWEXEC has no per-task WaitForInputIdle record.
     * It must survive idle notification and lose FIRSTIDLE. A real waiter
     * uses the same callback and must receive its event. */
    CHECK(binding.thread->ptdb->pwti == NULL);
    CHECK(wow_user_runtime_enter(&binding));
    binding.thread->host->wake_input_idle(binding.thread);
    CHECK(!(binding.thread->TIF_flags & WOW_TASK_TIF_FIRSTIDLE));
    idle_waiter.pIdleEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(idle_waiter.pIdleEvent != NULL);
    binding.thread->ptdb->pwti = &idle_waiter;
    binding.thread->TIF_flags |= WOW_TASK_TIF_FIRSTIDLE;
    binding.thread->host->wake_input_idle(binding.thread);
    CHECK(WaitForSingleObject(idle_waiter.pIdleEvent, 0) == WAIT_OBJECT_0);
    CHECK(!(binding.thread->TIF_flags & WOW_TASK_TIF_FIRSTIDLE));
    binding.thread->ptdb->pwti = NULL;
    CloseHandle(idle_waiter.pIdleEvent);
    CHECK(wow_user_runtime_leave(&binding));
    verify_receive_callout(&binding);
    /* Exercise the recovered USER task-order owner after InitTask.  This is
     * deliberately not a synthetic wake: with no pending work, the original
     * yield path must retain its own immediate scheduler semantics. */
    CHECK(wow_user_task_lifecycle_yield(&lifecycle));
    CHECK(PostThreadMessageA(GetCurrentThreadId(), WM_APP + 51, 73, 0));
    CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 51, WM_APP + 51, PM_NOREMOVE | PM_NOYIELD, FALSE));
    CHECK(message.message == WM_APP + 51 && message.wParam == 73);
    CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 51, WM_APP + 51, PM_REMOVE, TRUE));
    CHECK(message.message == WM_APP + 51 && message.wParam == 73);
    CHECK(!wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 51, WM_APP + 51, PM_NOREMOVE | PM_NOYIELD, FALSE));
    CHECK(PostThreadMessageA(GetCurrentThreadId(), WM_QUIT, 41, 0));
    CHECK(!wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        0, 0, PM_REMOVE, TRUE));
    CHECK(message.message == WM_QUIT && message.wParam == 41);
    CHECK(wow_user_task_lifecycle_message(&lifecycle, NULL, NULL,
        0, 0, PM_REMOVE, TRUE) == -1);
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!binding.exclusive_held);
    {
        HANDLE input;
        DWORD result;
        /* Empty/examined queue must wait for actual native input, not merely
         * the original scheduler's own wake event. */
        (void)GetQueueStatus(QS_ALLINPUT);
        input = CreateThread(NULL, 0, delayed_input,
            (void *)(ULONG_PTR)GetCurrentThreadId(), 0, NULL);
        CHECK(input != NULL);
        CHECK(wow_user_task_lifecycle_wait_message(&lifecycle));
        CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
            WM_APP + 52, WM_APP + 52, PM_REMOVE, TRUE));
        CHECK(message.message == WM_APP + 52 && message.wParam == 19);
        CHECK(WaitForSingleObject(input, 1000) == WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(input, &result) && result == 0);
        CHECK(CloseHandle(input));
    }
    CHECK(PostThreadMessageA(GetCurrentThreadId(), WM_APP + 54, 54, 0));
    CHECK(PostThreadMessageA(GetCurrentThreadId(), WM_APP + 53, 53, 0));
    CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 53, WM_APP + 53, PM_REMOVE, TRUE));
    CHECK(message.message == WM_APP + 53);
    CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 54, WM_APP + 54, PM_REMOVE, TRUE));
    CHECK(message.message == WM_APP + 54);
    {
        UINT_PTR timer = SetTimer(NULL, 0, 10, NULL);
        CHECK(timer != 0);
        CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
            WM_TIMER, WM_TIMER, PM_REMOVE, TRUE));
        CHECK(message.message == WM_TIMER && message.wParam == timer);
        CHECK(KillTimer(NULL, timer));
    }
    /* A native object disappeared before its enrollment was retired. The one
     * registered cleanup/retirement edge must leave the task carrier intact
     * on failure, then run original cleanup exactly once when it succeeds. */
    native_window = CreateWindowExA(0, "STATIC", "", WS_POPUP, 0, 0, 1, 1,
        NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(native_window != NULL);
    if (native_window) {
        WORD index = LOWORD(native_window);
        wow_cleanup_handle *entry = &lifecycle.objects->entries[index];
        struct wow_user_task_lifecycle_thread *task = lifecycle.threads;
        struct wow_task_order_thread *thread = binding.thread;
        stale_window.thread = thread;
        entry->phead = &stale_window;
        entry->pOwner = thread;
        entry->bType = TYPE_WINDOW;
        entry->wUniq = HIWORD(native_window);
        lifecycle.objects->last_handle = index;
        /* A genuinely live owned window must not lose its THREADINFO merely
         * because native-thread retirement was requested. This is a negative
         * safety check, not acceptance of exceptional thread cleanup. */
        CHECK(!wow_user_runtime_unbind(&binding));
        CHECK(GetLastError() == ERROR_BUSY);
        CHECK(IsWindow(native_window));
        CHECK(wow_user_runtime_current() == &binding);
        CHECK(binding.thread == thread && lifecycle.threads == task);
        CHECK(entry->pOwner == thread && thread->ptdb != NULL);
        CHECK(!binding.exclusive_held && lifecycle.cleanup.thread == NULL);
        CHECK(WaitForSingleObject(thread->pEventQueueServer, 0) != WAIT_FAILED);
        CHECK(DestroyWindow(native_window));
        CHECK(!wow_user_task_lifecycle_exit(&lifecycle,
            (HANDLE)(ULONG_PTR)0x1234, 0x1234, NULL, 0));
        CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
        CHECK(lifecycle.threads == task && binding.thread == thread);
        CHECK(!binding.exclusive_held && lifecycle.cleanup.thread == NULL);
        CHECK(!wow_user_runtime_unbind(&binding));
        CHECK(GetLastError() == ERROR_BUSY);
        CHECK(wow_user_runtime_current() == &binding && binding.thread == thread);
        if (lifecycle.threads == task && binding.thread == thread) {
            CHECK(thread->ptdb != NULL && entry->pOwner == thread);
            CHECK(WaitForSingleObject(thread->pEventQueueServer, 0) != WAIT_FAILED);
        }
        ZeroMemory(entry, sizeof(*entry));
        lifecycle.objects->last_handle = 0;
    }
    CHECK(wow_user_task_lifecycle_exit(&lifecycle, (HANDLE)(ULONG_PTR)0x1234,
        0x1234, NULL, 0));
    CHECK(binding.thread != NULL && lifecycle.threads != NULL);
    CHECK(wow_user_task_lifecycle_exit(&lifecycle, (HANDLE)(ULONG_PTR)0x1234,
        0, NULL, 0));
    CHECK(binding.thread != NULL && binding.classes != NULL);
    CHECK(!binding.exclusive_held);
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(binding.thread == NULL && lifecycle.threads == NULL);
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    fprintf(stderr, "WOW_USER_TASK_LIFECYCLE errors=%u native_direct=%u\n", errors, native_direct_calls);
    return errors ? 1 : 0;
}
