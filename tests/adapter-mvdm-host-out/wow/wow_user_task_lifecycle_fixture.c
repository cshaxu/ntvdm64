#include "wow_user_task_lifecycle.h"
#include "wow_user_private_access.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/session/session.h"
#include "wow_native_process.h"
#include "nthook32-dll/hook.h"
#include "wow_title_policy.h"
#include "wow_menu_client_bindings.h"
#include "wow_idle_hook_binding.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

#include <stdio.h>

static unsigned errors;
static unsigned native_direct_calls;
#ifdef WOW_INIT_ROLLBACK_FIXTURE
extern unsigned wow_init_alloc_calls, wow_init_fail_at, wow_init_name_live;
extern unsigned wow_init_name_releases;
extern int wow_init_hook_fail;
extern unsigned wow_init_hooks_live, wow_init_hook_failures;
#endif
extern unsigned wow_task_class_publications, wow_task_class_fail_at;
#define CHECK(value) do { if (!(value)) { ++errors; \
    fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

#ifdef WOW_NATIVE_NESTED_FIXTURE
/* The generated header retains the exact production native-call macros.
 * Only their native service leaf throws: no uncaught exception crosses a
 * Windows WNDPROC boundary, whose handling is owned by modern USER/WOW64. */
static unsigned native_service_fault_calls;
static LRESULT WINAPI native_service_fault(HWND w,UINT message,WPARAM wp,LPARAM lp)
{
    (void)w;(void)message;(void)wp;(void)lp;
    ++native_service_fault_calls;
    SetLastError(0x447a4002);
    RaiseException(0xe0447402,0,0,NULL);
    return 0;
}
#include "wow_native_fixture_call.h"
WOW_NATIVE_MESSAGE(native_service_fault,
    (HWND w,UINT message,WPARAM wp,LPARAM lp),(w,message,wp,lp))
#undef WOW_NATIVE_MESSAGE
#undef WOW_NATIVE_CALL_AFTER
#endif

#ifdef WOW_START_NOTIFICATION_FIXTURE
#define FIXTURE_LAUNCH_REQUEST 0x5678
static wow_user_task_lifecycle *notification_owner;
static unsigned notification_calls;
static DWORD WINAPI notify_start(ULONG request)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    CHECK(request == 0x5678);
    CHECK(binding && !binding->exclusive_held);
    CHECK(wow_user_task_lifecycle_notify_start(notification_owner,
        0x1234, request, notify_start) == ERROR_INVALID_STATE);
    ++notification_calls;
    return notification_calls <= 7 ? ERROR_RETRY : ERROR_SUCCESS;
}
#else
#define FIXTURE_LAUNCH_REQUEST 0
#endif

#ifdef WOW_TITLE_POLICY_FIXTURE
DWORD WINAPI NtvdmSeparateWowStyle(DWORD *style);
extern BOOL fSeparateWow;
static BOOL style_attached(HWND window)
{
    typedef BOOL (__cdecl *attached_fn)(HWND);
    HMODULE module=GetModuleHandleW(L"ntw16s32.dll");
    attached_fn query=module?(attached_fn)(void *)GetProcAddress(module,"Ntw16sWindowAttached"):NULL;
    return query && query(window);
}
static HWND style_window(DWORD policy,LPCWSTR title)
{
    void *scope=NULL;HWND window;
    CHECK(wow_style_creation_begin(policy,&scope));
    window=CreateWindowExW(0,L"STATIC",title,WS_OVERLAPPEDWINDOW,0,0,400,240,NULL,NULL,NULL,NULL);
    CHECK(!wow_style_creation_end(scope));
    CHECK(style_attached(window)==(policy!=WINDOW_STYLE_OFF));return window;
}
typedef struct style_peer {
    session *owner;wow_user_runtime *runtime;wow_user_task_lifecycle *lifecycle;HDESK desktop;
} style_peer;
static DWORD WINAPI off_title_peer(void *argument)
{
    style_peer *peer=argument;wow_user_runtime_thread binding={0};DWORD policy;HWND window;WCHAR text[80];
    CHECK(SetThreadDesktop(peer->desktop));
    CHECK(session_thread_bind(peer->owner));
    CHECK(wow_user_runtime_bind(&binding,peer->runtime,NULL,NULL));
    CHECK(wow_user_task_lifecycle_init_style(peer->lifecycle,0x0400,"OFF.EXE",
        0x1236,0,0,0,0,1,1,SW_HIDE,WINDOW_STYLE_OFF,0));
    CHECK(wow_title_policy(&policy) && policy==WINDOW_STYLE_OFF);
    window=style_window(policy,L"other task");
    CHECK(window);GetWindowTextW(window,text,80);
    CHECK(!wcscmp(text,L"other task"));CHECK(DestroyWindow(window));
    CHECK(wow_user_runtime_unbind(&binding));CHECK(session_thread_unbind(peer->owner));return 0;
}
static void verify_task_title(void)
{
    DWORD policy;HWND w;WCHAR title[100];
    CHECK(!fSeparateWow);
    CHECK(NtvdmSeparateWowStyle(NULL)==ERROR_INVALID_PARAMETER);
    CHECK(NtvdmSeparateWowStyle(&policy)==ERROR_NOT_FOUND);
    fSeparateWow=TRUE; /* fixture-only classification, no delivered Get record */
    CHECK(NtvdmSeparateWowStyle(&policy)==ERROR_INVALID_STATE);
    fSeparateWow=FALSE;
    CHECK(wow_title_policy(&policy) && policy==WINDOW_STYLE_WIN16);
    w=style_window(policy,L"task");
    CHECK(w);GetWindowTextW(w,title,100);
    CHECK(!wcscmp(title,L"task"));
    CHECK(SetWindowTextW(w,L"changed"));GetWindowTextW(w,title,100);
    CHECK(!wcscmp(title,L"changed"));CHECK(DestroyWindow(w));
    w=style_window(WINDOW_STYLE_OFF,L"off");
    CHECK(w);GetWindowTextW(w,title,100);
    CHECK(!wcscmp(title,L"off"));CHECK(DestroyWindow(w));
    /* Original modal A/W and Off/Win16/All live in the required real-CCPU
     * registered-owner sibling process. This model owns only policy/peer
     * isolation and dynamic caption tests, not a second modal lifetime. */
    fprintf(stderr,"WOW_TITLE_POLICY bound task, Off isolation and dynamic caption checked\n");
}
#endif

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

static BOOL control_callout_active;
static unsigned control_paints, control_texts;
static LRESULT reentrant_receive_body(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (control_callout_active && (message == WM_PAINT || message == WM_SETTEXT ||
            message == WM_CHAR)) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(binding && !binding->exclusive_held);
        CHECK(binding && binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        if (message == WM_CHAR) return (LRESULT)wp;
        if (message == WM_PAINT) {
            PAINTSTRUCT paint;
            ++control_paints;
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
            /* Exercise a nested public text call during synchronous paint. */
            CHECK(wow_native_SetWindowTextW(window, L"\x4e2d\x6587"));
            CHECK(!binding->exclusive_held);
            return 0;
        }
        ++control_texts;
    }
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

static LRESULT CALLBACK reentrant_receive(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    wow_task_callback_scope scope;
    LRESULT result;
    /* These bodies invoke original WOW services, just as the production
     * registered callback does. Native resource-only messages need no guest
     * entry. Do not model guest code by bypassing the common callback gate. */
    if (message != WM_APP + 62 && message != WM_APP + 63 &&
            !(control_callout_active && (message == WM_PAINT ||
                message == WM_SETTEXT || message == WM_CHAR)))
        return reentrant_receive_body(window, message, wp, lp);
    CHECK(wow_task_callback_enter(&scope));
    __try { result = reentrant_receive_body(window, message, wp, lp); }
    __finally { CHECK(wow_task_callback_leave(&scope)); }
    return result;
}

static DWORD WINAPI send_reentrant(void *window)
{
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(window, WM_APP + 62, 0, 0,
        SMTO_ABORTIFHUNG, 3000, &result) && result == 62 ? 0 : 1;
}

#ifdef WOW_NATIVE_SEEN_FIXTURE
static DWORD WINAPI cross_watchdog(void *done);
#endif
static void verify_control_callouts(wow_user_runtime_thread *binding,
    const WNDCLASSW *cls)
{
    HWND parent, child;
    WW words = {0};
    wow_window_words_binding *parent_words, *child_words;
    unsigned paints, texts;
    WCHAR text[16] = {0};
    /* A real visible test-owned window is required: message-only windows
     * cannot prove synchronous UpdateWindow/RDW_UPDATENOW delivery. */
    parent = CreateWindowExW(WS_EX_TOOLWINDOW, cls->lpszClassName, L"", WS_POPUP | WS_VISIBLE,
        0, 0, 32, 32, NULL, NULL, cls->hInstance, NULL);
    CHECK(parent != NULL);
    if (!parent) return;
    child = CreateWindowExW(0, cls->lpszClassName, L"", WS_CHILD | WS_VISIBLE,
        0, 0, 16, 16, parent, (HMENU)(INT_PTR)17, cls->hInstance, NULL);
    CHECK(child != NULL);
    if (!child) { DestroyWindow(parent); return; }
    CHECK(wow_window_words_attach(parent, &words));
    CHECK(wow_window_words_attach(child, &words));
    parent_words = wow_window_words_acquire(parent);
    child_words = wow_window_words_acquire(child);
    CHECK(parent_words && child_words);
    if (parent_words && child_words) {
        wow_window_words_cleanup_value(parent_words)->thread = binding->thread;
        wow_window_words_cleanup_value(child_words)->thread = binding->thread;
        CHECK(wow_user_runtime_enter(binding));
        control_callout_active = TRUE;
        paints = control_paints; texts = control_texts;
        CHECK(InvalidateRect(child, NULL, FALSE));
        CHECK(wow_native_UpdateWindow(child));
        CHECK(control_paints > paints && control_texts > texts);
        CHECK(binding->exclusive_held);
        paints = control_paints;
        CHECK(wow_native_RedrawWindow(child, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW));
        CHECK(control_paints > paints && binding->exclusive_held);
        texts = control_texts;
        CHECK(wow_native_SetDlgItemTextW(parent, 17, L"\x4e00\x4e8c"));
        CHECK(control_texts == texts + 1 && binding->exclusive_held);
        CHECK(!wow_native_SetDlgItemTextW(parent, 18, L"missing"));
        CHECK(control_texts == texts + 1 && binding->exclusive_held);
        CHECK(wow_native_SetWindowContextHelpId(child, 0x4473));
        CHECK(GetWindowContextHelpId(child) == 0x4473 && binding->exclusive_held);
        CHECK(wow_native_SetWindowContextHelpId(child, 0));
        CHECK(GetWindowContextHelpId(child) == 0 && binding->exclusive_held);
        {
            int increased = wow_native_ShowCursor(TRUE);
            int restored = wow_native_ShowCursor(FALSE);
            CHECK(restored == increased - 1 && binding->exclusive_held);
        }
        control_callout_active = FALSE;
        CHECK(wow_user_runtime_leave(binding));
        CHECK(GetWindowTextW(child, text, ARRAYSIZE(text)) == 2);
        CHECK(text[0] == 0x4e00 && text[1] == 0x4e8c);
    }
    wow_window_words_detach(child);
    wow_window_words_detach(parent);
    if (child_words) wow_window_words_release(child_words);
    if (parent_words) wow_window_words_release(parent_words);
    CHECK(DestroyWindow(parent));
    CHECK(!IsWindow(child));
    fprintf(stderr, "WOW_CONTROL_CALLOUT paints=%u texts=%u errors=%u\n",
        control_paints, control_texts, errors);
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
    verify_control_callouts(binding, &cls);
    {
        wow_user_native_call outer, inner;
        wow_task_order_thread *thread = binding->thread;
        CHECK(wow_task_execution_ready(thread));
        CHECK(wow_user_native_call_begin_inline(&outer));
        CHECK(!wow_task_execution_ready(thread));
        CHECK(thread->ppi->pwpi->CSOwningThread == thread);
        CHECK(wow_user_native_call_begin_inline(&inner));
        CHECK(inner.execution.previous == &outer.execution);
        CHECK(wow_user_native_call_end(&inner));
        CHECK(!wow_task_execution_ready(thread) && outer.execution.active);
        CHECK(!wow_user_runtime_unbind(binding) && GetLastError() == ERROR_BUSY);
        CHECK(wow_user_runtime_current() == binding && binding->thread == thread);
        CHECK(wow_user_native_call_end(&outer));
        CHECK(wow_task_execution_ready(thread) && !outer.execution.active);
        CHECK(wow_user_native_call_begin(&outer, NULL));
        CHECK(!wow_task_execution_ready(thread));
        CHECK(thread->ppi->pwpi->CSOwningThread != thread);
        CHECK(wow_user_native_call_end(&outer));
        CHECK(wow_task_execution_ready(thread) && thread->ppi->pwpi->CSOwningThread == thread);
        fprintf(stderr, "WOW_NATIVE_EXECUTION_SCOPES errors=%u inline=1 nested=1 wait=1 unbind_rejected=1\n", errors);
    }
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
            CHECK(wow_native_SendMessageW(window, WM_APP + 63, 0, 114) == 114);
            CHECK(GetLastError() == 0x5678 && native_direct_calls == 5);
            {
                WCHAR text[16] = {0};
                /* Non-ANSI payload must survive the actual W entry; no
                 * codec or source-level W-to-A substitution is permitted. */
                CHECK(wow_native_DefWindowProcW(window, WM_SETTEXT, 0,
                    (LPARAM)L"\x4e2d\x6587") != 0);
                CHECK(wow_native_SendMessageW(window, WM_GETTEXT,
                    ARRAYSIZE(text), (LPARAM)text) == 2);
                CHECK(text[0] == 0x4e2d && text[1] == 0x6587 && !text[2]);
            }
            {
                MSG unicode_message;
                wow_user_task_lifecycle *owner = binding->runtime->lifecycle;
                /* The original USER modal loop is Unicode, unlike WOW's
                 * ANSI thunk pump. Do not lose WM_CHAR through PeekMessageA. */
                CHECK(PostMessageW(window, WM_CHAR, 0x4e2d, 1));
                CHECK(wow_user_runtime_enter(binding));
                CHECK(wow_user_task_lifecycle_messageW(owner, &unicode_message,
                    window, WM_CHAR, WM_CHAR, PM_NOREMOVE | PM_NOYIELD, FALSE));
                CHECK(unicode_message.message == WM_CHAR && unicode_message.wParam == 0x4e2d);
                CHECK(binding->exclusive_held);
                CHECK(wow_user_task_lifecycle_messageW(owner, &unicode_message,
                    window, WM_CHAR, WM_CHAR, PM_REMOVE | PM_NOYIELD, TRUE));
                CHECK(unicode_message.wParam == 0x4e2d && binding->exclusive_held);
                control_callout_active = TRUE;
                CHECK(wow_native_DispatchMessageW(&unicode_message) == 0x4e2d);
                control_callout_active = FALSE;
                CHECK(binding->exclusive_held);
                CHECK(!wow_user_task_lifecycle_messageW(owner, &unicode_message,
                    window, WM_CHAR, WM_CHAR, PM_NOREMOVE | PM_NOYIELD, FALSE));
                CHECK(wow_user_runtime_leave(binding));
            }
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
#ifdef WOW_NATIVE_SEEN_FIXTURE
    {
        HANDLE done = CreateEventW(NULL, TRUE, FALSE, NULL);
        HANDLE watchdog = CreateThread(NULL, 0, cross_watchdog, done, 0, NULL);
        HANDLE event = binding->thread->pEventQueueServer;
        CHECK(done && watchdog && ResetEvent(event));
        CHECK(HIWORD(GetQueueStatus(QS_SENDMESSAGE)) & QS_SENDMESSAGE);
        fprintf(stderr, "WOW_NATIVE_SEEN_READY\n");
        CHECK(binding->thread->host->wait(binding->thread, 1, &event, FALSE) == 0);
        fprintf(stderr, "WOW_NATIVE_SEEN_WAIT_RETURN\n");
        SetEvent(done);
        CHECK(WaitForSingleObject(watchdog, 3000) == WAIT_OBJECT_0);
        CloseHandle(watchdog); CloseHandle(done);
    }
#endif
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

#ifdef WOW_NATIVE_SEND_FIXTURE
typedef struct cross_callout {
    session *session_owner;
    wow_user_runtime *runtime;
    wow_user_task_lifecycle *lifecycle;
    wow_task_order_thread *sender_thread;
    HANDLE ready, returned, complete;
    HWND window, sender_window;
    DWORD receiver_id;
    unsigned delivered;
#ifdef WOW_NATIVE_NESTED_FIXTURE
    unsigned timed_callbacks;
    BOOL destroyed_during_send;
    BOOL retired_during_send;
    wow_window_words_binding *receiver_borrow;
#endif
#ifdef WOW_NATIVE_REPLY_FIXTURE
    volatile LONG native_returned;
#endif
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
    HANDLE foreign_entered;
    BOOL foreign_first;
#endif
} cross_callout;

#ifdef WOW_NATIVE_FOREIGN_FIXTURE
static DWORD WINAPI cross_foreign_sender(void *parameter)
{
    cross_callout *test = parameter;
    DWORD_PTR result = 0;
    /* No runtime/task registration: genuinely external to the WOW scheduler.
     * Its message tuple is identical to the later production-wrapper send. */
    if (!SendMessageTimeoutA(test->window, WM_APP + 65, 0, 0,
            SMTO_ABORTIFHUNG, 10000, &result) || result != 114)
        return 1;
    return 0;
}
#endif

static DWORD WINAPI cross_watchdog(void *done)
{
    if (WaitForSingleObject(done, 15000) != WAIT_OBJECT_0) {
        fprintf(stderr, "WOW_NATIVE_SEND_TIMEOUT\n");
        ExitProcess(98);
    }
    return 0;
}

static LRESULT cross_receive_body(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    cross_callout *test = (cross_callout *)GetWindowLongPtrW(window, GWLP_USERDATA);
    if (message == WM_NCCREATE) {
        test = ((CREATESTRUCTW *)lp)->lpCreateParams;
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)test);
    }
    if (message == WM_APP + 65) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(GetCurrentThreadId() == test->receiver_id);
        CHECK(binding->thread != test->sender_thread);
        CHECK(binding->thread->ptdb && binding->thread->ptdb->hTaskWow == 0x1235);
        CHECK(InSendMessage() && !binding->exclusive_held);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        ++test->delivered;
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
        if (test->delivered == 1) {
            wow_task_order_message *pending;
            CHECK(wow_user_runtime_enter(binding));
            pending = test->sender_thread->psmsSent;
            CHECK(!pending && !binding->thread->psmsCurrent);
            CHECK(wp == 0 && lp == 0);
            fprintf(stderr, "WOW_NATIVE_FOREIGN_ALIAS pending=%p current=%p flags=%lu\n",
                pending, binding->thread->psmsCurrent, InSendMessageEx(NULL));
            CHECK(wow_user_runtime_leave(binding));
        }
#endif
#ifdef WOW_NATIVE_IDENTITY_FIXTURE
        {
            wow_task_order_message *sent;
            CHECK(wow_user_runtime_enter(binding));
            sent = test->sender_thread->psmsSent;
            /* Approved native USER identity, not NT4 private pointer equality.
             * An unrelated pending local call is not this receive's sender. */
            CHECK(!sent && !binding->thread->psmsCurrent);
            CHECK((InSendMessageEx(NULL) & (ISMEX_SEND | ISMEX_REPLIED)) == ISMEX_SEND);
            fprintf(stderr, "WOW_NATIVE_RECEIVE_IDENTITY sent=%p current=%p flags=%lu\n",
                sent, binding->thread->psmsCurrent, InSendMessageEx(NULL));
            CHECK(wow_user_runtime_leave(binding));
        }
#endif
        fprintf(stderr, "WOW_NATIVE_SEND_RECEIVE early=%lu\n", (DWORD)wp);
        if (wp == 2) {
            DWORD_PTR nested_result = 0;
            wow_task_order_message *previous = binding->thread->psmsSent;
            CHECK(wow_native_SendMessageTimeoutA(test->sender_window, WM_APP + 66,
                0, 0, SMTO_ABORTIFHUNG, 3000, &nested_result) != 0);
            CHECK(nested_result == 113);
            CHECK(binding->thread->psmsSent == previous);
            CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
            return nested_result + 1;
        }
        if (wp == 1) {
            BOOL duplicate;
            CHECK(ReplyMessage(114));
            CHECK(InSendMessageEx(NULL) & ISMEX_REPLIED);
            duplicate = ReplyMessage(115);
            CHECK(InSendMessageEx(NULL) & ISMEX_REPLIED);
            fprintf(stderr, "WOW_NATIVE_DUPLICATE_REPLY result=%u first_value=114\n", duplicate);
            /* Native reply completion and the original cooperative WOW
             * execution reacquisition are separate. Yield through the actual
             * product wait, never require a non-yielding NT4-only rendezvous. */
            if (wow_user_msg_wait(1, &test->returned, FALSE, 3000, 0) != WAIT_OBJECT_0) {
#ifdef WOW_NATIVE_REPLY_FIXTURE
                CHECK(wow_user_runtime_enter(binding));
                fprintf(stderr, "WOW_NATIVE_REPLY_STALLED native_returned=%ld sender=%p receiver=%p scheduled=%p owner=%p current=%p\n",
                    InterlockedCompareExchange(&test->native_returned, 0, 0),
                    test->sender_thread, binding->thread,
                    binding->thread->ppi->pwpi->ptiScheduled,
                    binding->thread->ppi->pwpi->CSOwningThread,
                    binding->thread->psmsCurrent);
                CHECK(wow_user_runtime_leave(binding));
#endif
                ExitProcess(95);
            }
            CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
            return 999;
        }
        return 114;
    }
    if (message == WM_APP + 66) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(InSendMessage() && !binding->exclusive_held);
        CHECK(binding->thread->psmsSent == NULL);
        fprintf(stderr, "WOW_NATIVE_NESTED_CALLBACK scheduled=%u owner=%u\n",
            binding->thread->ppi->pwpi->ptiScheduled == binding->thread,
            binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->ppi->pwpi->ptiScheduled == binding->thread);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        return 113;
    }
#ifdef WOW_NATIVE_NESTED_FIXTURE
    if (message == WM_APP + 67) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(InSendMessage() && !binding->exclusive_held);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        /* Deliberate native callback delay, not a product pacing change.
         * The sender's 250ms API deadline expires before this callback
         * gives execution back through the actual callback finalizer. */
        Sleep(500);
        ++test->timed_callbacks;
        return 115;
    }
    if (message == WM_APP + 68) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(InSendMessage() && !binding->exclusive_held);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(wow_native_DestroyWindow(window));
        CHECK(!IsWindow(window));
        test->destroyed_during_send = TRUE;
        SetLastError(0x447a4001);
        return 116;
    }
    if (message == WM_APP + 69) {
        BOOL caught = FALSE;
        __try { RaiseException(0xe0447402,0,0,NULL); }
        __except (GetExceptionCode() == 0xe0447402 ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught = TRUE;
        }
        CHECK(caught);
        SetLastError(0x447a4002);
        return 117;
    }
    if (message == WM_APP + 71) {
        wow_user_runtime_thread *binding = wow_user_runtime_current();
        CHECK(InSendMessage() && binding && !binding->exclusive_held);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        /* Exit this native receiver normally, while the sender's actual USER
         * call is active. Release the fixture's persistent borrow exactly as
         * cross_receiver's ordinary return path does. No forced termination,
         * fabricated scheduler fields or stale receiver dereference. */
        wow_window_words_detach(window);
        wow_window_words_release(test->receiver_borrow);
        test->receiver_borrow = NULL;
        CHECK(DestroyWindow(window));
        /* This row exits the thread from inside its callback, not a
         * retryable detach. Production's session finish hook supplies this
         * terminal intent; this directly bound native fixture supplies the
         * same explicit input before abandoning its callback stack. */
        binding->terminal_exit = TRUE;
        if (!wow_user_runtime_unbind(binding)) ExitProcess(94);
        if (!session_thread_unbind(test->session_owner)) ExitProcess(94);
        test->retired_during_send = TRUE;
        ExitThread(0);
    }
#endif
    return DefWindowProcW(window, message, wp, lp);
}

static LRESULT CALLBACK cross_receive(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    wow_task_callback_scope scope;
    LRESULT result;
    if (message != WM_APP + 65 && message != WM_APP + 66 &&
            message != WM_APP + 67 && message != WM_APP + 68 &&
            message != WM_APP + 69 && message != WM_APP + 71)
        return cross_receive_body(window, message, wp, lp);
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
    if (message == WM_APP + 65) {
        cross_callout *test = (cross_callout *)GetWindowLongPtrW(window, GWLP_USERDATA);
        if (test->foreign_first) {
            /* The parent has not sent yet. This is unambiguously the external
             * sender; signal before the production callback execution gate. */
            test->foreign_first = FALSE;
            SetEvent(test->foreign_entered);
        }
    }
#endif
    SetLastError(0xe044700au);
    CHECK(wow_task_callback_enter(&scope));
    CHECK(GetLastError() == 0xe044700au);
    __try { result = cross_receive_body(window, message, wp, lp); }
    __finally {
        DWORD callback_error = GetLastError();
        CHECK(wow_task_callback_leave(&scope));
        CHECK(GetLastError() == callback_error);
    }
    return result;
}

static DWORD WINAPI cross_receiver(void *parameter)
{
    cross_callout *test = parameter;
    wow_user_runtime_thread binding = {0};
    wow_window_words_binding *borrow;
    WW words = {0};
    MSG message;
    DWORD window_style;
    CHECK(session_thread_bind(test->session_owner));
    CHECK(wow_user_runtime_bind(&binding, test->runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_init_style(test->lifecycle, 0x0400, "RECEIVER.EXE",
        0x1235, 0, 0, 0, 0, 1, 1, SW_HIDE, WINDOW_STYLE_OFF, 0x1234));
    CHECK(wow_user_task_lifecycle_style(test->lifecycle, &window_style) &&
        window_style == WINDOW_STYLE_WIN16);
    test->window = CreateWindowExW(0, L"WOW_NATIVE_CROSS", L"", 0, 0, 0, 1, 1,
        HWND_MESSAGE, NULL, GetModuleHandleW(NULL), test);
    CHECK(test->window != NULL);
    CHECK(wow_window_words_attach(test->window, &words));
    borrow = wow_window_words_acquire(test->window);
    if (!borrow) ExitProcess(94);
    /* Native-only owner association, as in the same-thread production test. */
    wow_window_words_cleanup_value(borrow)->thread = binding.thread;
#ifdef WOW_NATIVE_NESTED_FIXTURE
    test->receiver_borrow = borrow;
#endif
    SetEvent(test->ready);
    while (wow_user_task_lifecycle_message(test->lifecycle, &message, NULL,
            0, 0, PM_REMOVE, TRUE) > 0)
        DispatchMessageW(&message);
    wow_window_words_detach(test->window);
    wow_window_words_release(borrow);
#ifdef WOW_NATIVE_NESTED_FIXTURE
    CHECK(test->destroyed_during_send && !IsWindow(test->window));
#else
    CHECK(DestroyWindow(test->window));
#endif
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(session_thread_unbind(test->session_owner));
    return 0;
}

static void verify_cross_callout(session *owner, wow_user_runtime_thread *binding,
    wow_user_task_lifecycle *lifecycle)
{
    cross_callout test = {0};
    WNDCLASSW cls = {0};
    HANDLE helper, watchdog;
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
    HANDLE foreign;
    wow_user_native_call pending_call;
    DWORD_PTR pending_result = 0;
#endif
    wow_window_words_binding *sender_borrow = NULL;
    WW sender_words = {0};
    DWORD code;
    unsigned early;
    cls.lpfnWndProc = cross_receive;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = L"WOW_NATIVE_CROSS";
    CHECK(RegisterClassW(&cls) != 0);
    test.session_owner = owner; test.runtime = binding->runtime;
    test.lifecycle = lifecycle;
    test.sender_thread = binding->thread;
    test.ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    test.returned = CreateEventW(NULL, TRUE, FALSE, NULL);
    test.complete = CreateEventW(NULL, TRUE, FALSE, NULL);
#ifdef WOW_NATIVE_NESTED_FIXTURE
    test.sender_window = CreateWindowExW(0, cls.lpszClassName, L"", 0,
        0, 0, 1, 1, HWND_MESSAGE, NULL, cls.hInstance, &test);
    CHECK(test.sender_window != NULL);
    CHECK(wow_window_words_attach(test.sender_window, &sender_words));
    sender_borrow = wow_window_words_acquire(test.sender_window);
    if (!sender_borrow) ExitProcess(94);
    wow_window_words_cleanup_value(sender_borrow)->thread = binding->thread;
#endif
    watchdog = CreateThread(NULL, 0, cross_watchdog, test.complete, 0, NULL);
    helper = CreateThread(NULL, 0, cross_receiver, &test, 0, &test.receiver_id);
    CHECK(helper && watchdog);
    if (WaitForSingleObject(test.ready, 3000) != WAIT_OBJECT_0) ExitProcess(96);
    CHECK(wow_user_task_lifecycle_yield(lifecycle));
    /* Adoption resolves the HWND's native TID through this same production
     * relation. Check it under the USER lock, without inventing an owner or
     * retaining the returned pointer across a native callout. */
    CHECK(wow_user_runtime_enter(binding));
    {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(test.window, &pid);
        wow_task_order_thread *resolved = binding->thread->host->find_thread(
            binding->thread, tid);
        wow_window_words_binding *words = wow_window_words_acquire(test.window);
        CHECK(pid == GetCurrentProcessId() && tid == test.receiver_id);
        CHECK(resolved && resolved != binding->thread);
        CHECK(resolved && resolved->ppi == binding->thread->ppi &&
            resolved->ptdb && resolved->ptdb->hTaskWow == 0x1235);
        CHECK(words && wow_window_words_cleanup_value(words)->thread == resolved);
        if (words) wow_window_words_release(words);
    }
    CHECK(wow_user_runtime_leave(binding));
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
    test.foreign_entered = CreateEventW(NULL, TRUE, FALSE, NULL);
    test.foreign_first = TRUE;
    /* Exercise the exposed production begin/end boundary, inserting a real
     * external send between begin and the native syscall. No test-written
     * task fields, scheduling record, or psmsCurrent assignment is used. */
    CHECK(wow_user_native_call_begin(&pending_call, test.window));
    CHECK(wow_user_runtime_enter(binding));
    fprintf(stderr, "WOW_NATIVE_FOREIGN_BEGIN sender=%p receiver=%p scheduled=%p owner=%p held=%u\n",
        binding->thread, binding->thread->host->find_thread(binding->thread, test.receiver_id),
        binding->thread->ppi->pwpi->ptiScheduled,
        binding->thread->ppi->pwpi->CSOwningThread, pending_call.held);
    CHECK(wow_user_runtime_leave(binding));
    foreign = CreateThread(NULL, 0, cross_foreign_sender, &test, 0, NULL);
    CHECK(foreign != NULL);
    if (WaitForSingleObject(test.foreign_entered, 3000) != WAIT_OBJECT_0) {
        CHECK(wow_user_runtime_enter(binding));
        fprintf(stderr, "WOW_NATIVE_FOREIGN_STALLED scheduled=%p owner=%p sender_events=%d receiver_events=%d\n",
            binding->thread->ppi->pwpi->ptiScheduled,
            binding->thread->ppi->pwpi->CSOwningThread,
            binding->thread->ptdb->nEvents,
            binding->thread->host->find_thread(binding->thread, test.receiver_id)->ptdb->nEvents);
        CHECK(wow_user_runtime_leave(binding));
        ExitProcess(97);
    }
    CHECK(SendMessageTimeoutA(test.window, WM_APP + 65, 0, 0,
        SMTO_ABORTIFHUNG, 3000, &pending_result) != 0 && pending_result == 114);
    CHECK(wow_user_native_call_end(&pending_call));
#endif
    for (early = 0; early < 2; ++early) {
        DWORD_PTR result = 0;
        unsigned reply = 0;
#ifdef WOW_NATIVE_REPLY_FIXTURE
        reply = early;
#endif
#ifdef WOW_NATIVE_NESTED_FIXTURE
        reply = early ? 2 : 0;
#endif
        fprintf(stderr, "WOW_NATIVE_SEND_BEGIN early=%u\n", reply);
#ifdef WOW_NATIVE_REPLY_FIXTURE
        {
            wow_user_native_call call;
            /* Observe the same production begin/native/end sequence without
             * changing scheduler state or inventing received SMS identity. */
            InterlockedExchange(&test.native_returned, 0);
            CHECK(wow_user_native_call_begin(&call, test.window));
            CHECK(SendMessageTimeoutA(test.window, WM_APP + 65, reply, 0,
                SMTO_ABORTIFHUNG, 3000, &result) != 0 && result == 114);
            InterlockedExchange(&test.native_returned, 1);
            fprintf(stderr, "WOW_NATIVE_TRANSPORT_RETURN early=%u result=%lu\n", reply, result);
            CHECK(wow_user_native_call_end(&call));
        }
#else
        CHECK(wow_native_SendMessageTimeoutA(test.window, WM_APP + 65, reply, 0,
            SMTO_ABORTIFHUNG, 3000, &result) != 0 && result == 114);
#endif
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(!binding->thread->psmsSent && !binding->exclusive_held);
        fprintf(stderr, "WOW_NATIVE_SEND_RETURN early=%u result=%lu\n", reply, result);
        if (reply) SetEvent(test.returned);
    }
#ifdef WOW_NATIVE_NESTED_FIXTURE
    for (early = 0; early < 2; ++early) {
        DWORD_PTR result = 0;
        DWORD error;
        LRESULT sent;
        wow_task_order_message *previous = binding->thread->psmsSent;
        if (early) CHECK(wow_user_runtime_enter(binding));
        SetLastError(ERROR_SUCCESS);
        sent = wow_native_SendMessageTimeoutA(test.window, WM_APP + 67,
            0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 250, &result);
        error = GetLastError();
        CHECK(sent == 0);
        /* Native USER can report failure without an extended error. Do not
         * manufacture an error or treat the output result as valid. */
        CHECK(error == ERROR_TIMEOUT || error == ERROR_SUCCESS);
        CHECK(binding->exclusive_held == (BOOL)early);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->psmsSent == previous);
        CHECK(test.timed_callbacks == early + 1);
        fprintf(stderr, "WOW_NATIVE_TIMEOUT_RETURN held=%u error=%lu callbacks=%u\n",
            early, error, test.timed_callbacks);
        if (early) CHECK(wow_user_runtime_leave(binding));
    }
    for (early = 0; early < 2; ++early) {
        BOOL caught = FALSE;
        wow_task_order_message *previous = binding->thread->psmsSent;
        if (early) CHECK(wow_user_runtime_enter(binding));
        CHECK(wow_native_SendMessageA(test.sender_window, WM_APP + 69, 0, 0) == 117);
        CHECK(GetLastError() == 0x447a4002);
        CHECK(binding->exclusive_held == (BOOL)early);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->psmsSent == previous);
        fprintf(stderr,"WOW_NATIVE_CALLBACK_EXCEPTION_RETURN held=%u caught_in_callback=1 result=117 owner=1\n",early);
        __try {
            (void)wow_native_native_service_fault(test.sender_window, WM_APP + 69, 0, 0);
            CHECK(FALSE);
        } __except (GetExceptionCode() == 0xe0447402 ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught = TRUE;
            CHECK(GetLastError() == 0x447a4002);
        }
        CHECK(caught && binding->exclusive_held == (BOOL)early);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->psmsSent == previous);
        fprintf(stderr, "WOW_NATIVE_EXCEPTION_RETURN held=%u caught=%u owner=1 production_macro=1\n",
            early, caught);
        if (early) CHECK(wow_user_runtime_leave(binding));
    }
    CHECK(native_service_fault_calls == 2);
    {
        DWORD_PTR result = 0;
        wow_task_order_message *previous = binding->thread->psmsSent;
        CHECK(wow_native_SendMessageTimeoutA(test.window, WM_APP + 68,
            0, 0, SMTO_ABORTIFHUNG, 3000, &result) != 0);
        CHECK(result == 116 && test.destroyed_during_send && !IsWindow(test.window));
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->psmsSent == previous && !binding->exclusive_held);
        fprintf(stderr, "WOW_NATIVE_DESTROY_DURING_RETURN result=%lu dead=1 owner=1\n",
            (DWORD)result);
    }
#endif
    {
        wow_user_native_call join;
        /* The helper's exit destroys its window and can reenter its WOW
         * callback. Model the production native-wait boundary: do not keep
         * the sender eligible for guest execution during a raw kernel join.
         * A preceding DirectedYield is an opportunity, not a join barrier. */
        CHECK(wow_user_native_call_begin(&join, test.window));
        __try {
            CHECK(PostThreadMessageW(test.receiver_id, WM_QUIT, 0, 0));
            CHECK(WaitForSingleObject(helper, 3000) == WAIT_OBJECT_0);
        } __finally {
            CHECK(wow_user_native_call_end(&join));
        }
    }
    CHECK(GetExitCodeThread(helper, &code) && !code);
#ifdef WOW_NATIVE_NESTED_FIXTURE
    {
        DWORD_PTR result = 0;
        DWORD error;
        CHECK(!IsWindow(test.window));
        SetLastError(ERROR_SUCCESS);
        CHECK(wow_native_SendMessageTimeoutA(test.window, WM_APP + 65,
            0, 0, SMTO_ABORTIFHUNG, 250, &result) == 0);
        error = GetLastError();
        CHECK(error == ERROR_INVALID_WINDOW_HANDLE);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(!binding->thread->psmsSent && !binding->exclusive_held);
        fprintf(stderr, "WOW_NATIVE_DEAD_TARGET_RETURN error=%lu owner=1\n", error);
    }
#endif
#ifdef WOW_NATIVE_FOREIGN_FIXTURE
    CHECK(WaitForSingleObject(foreign, 3000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(foreign, &code) && !code);
    CHECK(test.delivered == 4);
    CloseHandle(foreign);
    CloseHandle(test.foreign_entered);
#else
    CHECK(test.delivered == 2);
#endif
    SetEvent(test.complete);
    CHECK(WaitForSingleObject(watchdog, 3000) == WAIT_OBJECT_0);
    CloseHandle(helper); CloseHandle(watchdog);
    CloseHandle(test.ready); CloseHandle(test.returned); CloseHandle(test.complete);
    if (sender_borrow) {
        wow_window_words_detach(test.sender_window);
        wow_window_words_release(sender_borrow);
        CHECK(DestroyWindow(test.sender_window));
    }
    #ifdef WOW_NATIVE_NESTED_FIXTURE
    for (early = 0; early < 2; ++early) {
        cross_callout dying = {0};
        DWORD_PTR result = 0;
        LRESULT sent;
        DWORD error;
        wow_task_order_message *previous = binding->thread->psmsSent;
        dying.session_owner = owner;
        dying.runtime = binding->runtime;
        dying.lifecycle = lifecycle;
        dying.sender_thread = binding->thread;
        dying.ready = CreateEventW(NULL, TRUE, FALSE, NULL);
        dying.complete = CreateEventW(NULL, TRUE, FALSE, NULL);
        watchdog = CreateThread(NULL, 0, cross_watchdog, dying.complete, 0, NULL);
        helper = CreateThread(NULL, 0, cross_receiver, &dying, 0, &dying.receiver_id);
        CHECK(helper && watchdog);
        if (WaitForSingleObject(dying.ready, 3000) != WAIT_OBJECT_0) ExitProcess(96);
        CHECK(wow_user_task_lifecycle_yield(lifecycle));
        if (early) CHECK(wow_user_runtime_enter(binding));
        SetLastError(ERROR_SUCCESS);
        sent = wow_native_SendMessageTimeoutA(dying.window, WM_APP + 71,
            0, 0, SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 3000, &result);
        error = GetLastError();
        CHECK(sent == 0 && dying.retired_during_send);
        /* USER may report no extended error for failure; retain the actual
         * code instead of manufacturing a task/owner error. */
        CHECK(error == ERROR_SUCCESS || error == ERROR_TIMEOUT ||
            error == ERROR_INVALID_WINDOW_HANDLE);
        CHECK(binding->exclusive_held == (BOOL)early);
        CHECK(binding->thread->ppi->pwpi->CSOwningThread == binding->thread);
        CHECK(binding->thread->psmsSent == previous);
        if (early) CHECK(wow_user_runtime_leave(binding));
        CHECK(WaitForSingleObject(helper, 3000) == WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(helper, &code) && code == 0);
        CHECK(!IsWindow(dying.window) && !dying.receiver_borrow);
        CHECK(wow_user_runtime_enter(binding));
        CHECK(!binding->thread->host->find_thread(binding->thread, dying.receiver_id));
        CHECK(wow_user_runtime_leave(binding));
        fprintf(stderr, "WOW_NATIVE_RECEIVER_EXIT_RETURN held=%u error=%lu retired=1 owner=1\n",
            early, error);
        SetEvent(dying.complete);
        CHECK(WaitForSingleObject(watchdog, 3000) == WAIT_OBJECT_0);
        CloseHandle(helper); CloseHandle(watchdog);
        CloseHandle(dying.ready); CloseHandle(dying.complete);
    }
    #endif
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
}
#endif

int __cdecl main(int argc,char **argv)
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
    DWORD window_style;

    if(argc==2 && !strcmp(argv[1],"--style-child")) {
        nthook_context context;BOOL found=FALSE;
        if(nthook_context_read(&context,&found) || !found ||
            context.mode!=NATIVE_HOOK_LAUNCHER || context.frontend || context.execution ||
            GetModuleHandleW(L"nthook32.dll"))return 90;
        return (int)(100+context.window_style);
    }

#ifdef WOW_TITLE_POLICY_FIXTURE
    {
        WCHAR name[80];HDESK desktop;
        swprintf_s(name,80,L"WowTitle-%lu",GetCurrentProcessId());
        desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
        CHECK(desktop && SetThreadDesktop(desktop));
        if(!desktop)return 92;
        /* Owned for this short-lived process; all windows end before exit. */
    }
#endif

    CHECK(wowexec != NULL);
    session_initialize(&owner, 42040u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    {
        wow_class_lookup_entry private_class = {0}, user_class = {0};
        HANDLE module = (HANDLE)0x23451234, outside = (HANDLE)0x45671234;
        CHECK(lifecycle.classes.client_module == GetModuleHandleW(L"user32.dll"));
        CHECK(lifecycle.classes.client_module != NULL);
        CHECK(HIWORD(lifecycle.classes.client_module) != HIWORD(module));
        private_class.atomClassName = user_class.atomClassName = 0xc123;
        private_class.hModule = module;
        user_class.hModule = lifecycle.classes.client_module;
        lifecycle.classes.pclsPrivateList = &private_class;
        CHECK(GetClassPtr(0xc123, &lifecycle.classes, module) ==
            &lifecycle.classes.pclsPrivateList);
        CHECK(GetClassPtr(0xc123, &lifecycle.classes, (HANDLE)0x23455678) ==
            &lifecycle.classes.pclsPrivateList);
        CHECK(GetClassPtr(0xc123, &lifecycle.classes, outside) == NULL);
        private_class.next = &user_class;
        CHECK(GetClassPtr(0xc123, &lifecycle.classes, outside) == &private_class.next);
        lifecycle.classes.pclsPrivateList = NULL;
    }
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, wowexec));
    CHECK(lifecycle.registered && lifecycle.process.pwpi != NULL);
    CHECK(!wow_user_task_lifecycle_init_style(&lifecycle, 0x0400, "APP.EXE",
        0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE, 3, 0));
    CHECK(!binding.thread && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!wow_user_task_lifecycle_init_style(&lifecycle, 0x0400, "APP.EXE",
        0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE, WINDOW_STYLE_OFF, 0x4321));
    CHECK(!binding.thread && GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_user_idle_hook_for_current() == NULL);
    {
        wow_class_lookup_entry *entry;
        unsigned published = 0;
        wow_task_class_fail_at = 3;
        CHECK(!wow_user_task_lifecycle_init_style(&lifecycle, 0x0400, "APP.EXE",
            0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE, WINDOW_STYLE_OFF, 0));
        CHECK(!binding.thread && !binding.exclusive_held && !lifecycle.threads);
        CHECK(wow_user_idle_hook_for_current() == NULL);
        for (entry = lifecycle.classes.pclsPublicList; entry; entry = entry->next)
            if (entry->guest_server) ++published;
        CHECK(published == 2 && wow_task_class_publications == 3);
        wow_task_class_fail_at = 0;
    }
#ifdef WOW_INIT_ROLLBACK_FIXTURE
    {
        unsigned failure;
        const int hook_ids[] = {WH_CALLWNDPROCRET, WH_CBT, WH_FOREGROUNDIDLE};
        for (failure = 0; failure < ARRAYSIZE(hook_ids); ++failure) {
            wow_init_hook_fail = hook_ids[failure];
            CHECK(!wow_user_task_lifecycle_init_style(&lifecycle, 0x0400,
                "APP.EXE", 0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE,
                WINDOW_STYLE_OFF, 0));
            CHECK(GetLastError() == ERROR_ACCESS_DENIED);
            CHECK(!wow_init_hooks_live && !binding.thread &&
                !binding.exclusive_held && !lifecycle.threads);
            CHECK(!lifecycle.process.pwpi->ptdbHead);
        }
        wow_init_hook_fail = 0;
        for (failure = 1; failure <= 2; ++failure) {
            wow_init_alloc_calls = 0;
            wow_init_fail_at = failure;
            CHECK(!wow_user_task_lifecycle_init_style(&lifecycle, 0x0400,
                "APP.EXE", 0x1234, 0, 0, 0, 0, 1, 1, SW_HIDE,
                WINDOW_STYLE_OFF, 0));
            CHECK(GetLastError() == (failure == 1 ?
                ERROR_INVALID_NAME : ERROR_NOT_ENOUGH_MEMORY));
            CHECK(wow_init_alloc_calls == failure && !wow_init_name_live);
            CHECK(!binding.thread && !binding.exclusive_held && !lifecycle.threads);
            CHECK(!lifecycle.process.pwpi->ptdbHead);
            CHECK(!wow_init_hooks_live);
        }
        CHECK(wow_init_name_releases == 1);
        wow_init_alloc_calls = wow_init_fail_at = 0;
    }
#endif
    CHECK(wow_user_task_lifecycle_init_style(&lifecycle, 0x0400, "APP.EXE",
        0x1234, 0, FIXTURE_LAUNCH_REQUEST,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_SHOW, WINDOW_STYLE_WIN16, 0));
    {
        HHOOK observer = wow_user_idle_hook_for_current();
        CHECK(observer != NULL);
        CHECK(wow_user_runtime_enter(&binding));
        CHECK(wow_user_idle_hook_for_current() == observer);
        CHECK(binding.exclusive_held);
        CHECK(wow_user_runtime_leave(&binding));
        CHECK(wow_user_idle_hook_for_current() == observer);
    }
    {
        wow_class_lookup_entry *entry;
        unsigned published = 0;
        for (entry = lifecycle.classes.pclsPublicList; entry; entry = entry->next)
            if (entry->guest_server) ++published;
        CHECK(published == 9 && wow_task_class_publications == 10);
        fprintf(stderr, "WOW_STANDARD_CLASS_ADMISSION projected=%u calls=%u retry=1 errors=%u\n",
            published, wow_task_class_publications, errors);
    }
#ifdef WOW_START_NOTIFICATION_FIXTURE
    {
        unsigned retry;
        void *committed = binding.thread;
        notification_owner = &lifecycle;
        CHECK(wow_user_task_lifecycle_notify_start(&lifecycle, 0x1235,
            0x5678, notify_start) == ERROR_INVALID_STATE);
        CHECK(wow_user_task_lifecycle_notify_start(&lifecycle, 0x1234,
            0x5679, notify_start) == ERROR_INVALID_STATE);
        CHECK(notification_calls == 0);
        for (retry = 0; retry < 7; ++retry) {
            CHECK(wow_user_task_lifecycle_notify_start(&lifecycle, 0x1234,
                0x5678, notify_start) == ERROR_RETRY);
            CHECK(binding.thread == committed);
        }
        CHECK(wow_user_task_lifecycle_notify_start(&lifecycle, 0x1234,
            0x5678, notify_start) == ERROR_SUCCESS);
        CHECK(wow_user_task_lifecycle_notify_start(&lifecycle, 0x1234,
            0x5678, notify_start) == ERROR_INVALID_STATE);
        CHECK(notification_calls == 8 && binding.thread == committed);
        fprintf(stderr, "WOW_START_NOTIFICATION identity/reentry/unlocked/retry/commit checked errors=%u\n", errors);
    }
#endif
    CHECK(wow_user_task_lifecycle_style(&lifecycle, &window_style) &&
        window_style == WINDOW_STYLE_WIN16);
#ifdef WOW_TITLE_POLICY_FIXTURE
    verify_task_title();
    {
        style_peer peer={&owner,&runtime,&lifecycle,GetThreadDesktop(GetCurrentThreadId())};
        HANDLE thread=CreateThread(NULL,0,off_title_peer,&peer,0,NULL);DWORD code;
        CHECK(thread);
        if(WaitForSingleObject(thread,15000)!=WAIT_OBJECT_0)ExitProcess(93);
        CHECK(GetExitCodeThread(thread,&code) && !code);CloseHandle(thread);
        /* Re-enter through the production cooperative scheduling boundary
         * after the peer retires; a host wait is not a WOW task switch. */
        CHECK(wow_user_task_lifecycle_yield(&lifecycle));
        CHECK(wow_title_policy(&window_style) && window_style==WINDOW_STYLE_WIN16);
        fprintf(stderr,"WOW_TITLE_POLICY same-runtime Off peer and parent policy retention checked\n");
    }
#endif
    {
        char image[MAX_PATH],target[MAX_PATH],command[MAX_PATH+32],*slash;
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};
        DWORD code=0;
        CHECK(GetModuleFileNameA(NULL,image,sizeof(image)));
        strcpy_s(target,sizeof(target),image);slash=strrchr(target,'\\');
        if(!slash)return 91;
        strcpy_s(slash+1,sizeof(target)-(size_t)(slash+1-target),"run16.exe");
        CHECK(CopyFileA(image,target,FALSE));
        sprintf_s(command,sizeof(command),"\"%s\" --style-child",target);
        CHECK(wow_native_create_process_a(target,command,NULL,NULL,FALSE,
            CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
        if(child.hProcess) {
            CHECK(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeProcess(child.hProcess,&code) && code==100+WINDOW_STYLE_WIN16);
            CloseHandle(child.hThread);CloseHandle(child.hProcess);
        }
        puts("WOW_NATIVE_STYLE_CHILD checked current-task payload");
    }
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
    {
        HANDLE event = CreateEventW(NULL, TRUE, FALSE, NULL);
        wow_task_order_thread *thread = binding.thread;
        CHECK(event != NULL);
        CHECK(wow_user_msg_wait(1, &event, FALSE, 0, 0) == WAIT_TIMEOUT);
        CHECK(lifecycle.process.pwpi->CSOwningThread == thread);
        CHECK(wow_user_msg_wait(1, &event, FALSE, 5, 0) == WAIT_TIMEOUT);
        CHECK(lifecycle.process.pwpi->CSOwningThread == thread && !binding.exclusive_held);
        CHECK(SetEvent(event));
        CHECK(wow_user_msg_wait(1, &event, FALSE, 100, 0) == WAIT_OBJECT_0);
        CHECK(lifecycle.process.pwpi->CSOwningThread == thread && !binding.exclusive_held);
        CHECK(CloseHandle(event));
        event = NULL;
        CHECK(wow_user_msg_wait(1, &event, FALSE, 100, 0) == WAIT_FAILED);
        CHECK(GetLastError() == ERROR_INVALID_HANDLE);
        CHECK(lifecycle.process.pwpi->CSOwningThread == thread && !binding.exclusive_held);
    }
    verify_receive_callout(&binding);
#ifdef WOW_NATIVE_SEND_FIXTURE
    verify_cross_callout(&owner, &binding, &lifecycle);
#endif
    /* Exercise the recovered USER task-order owner after InitTask.  This is
     * deliberately not a synthetic wake: with no pending work, the original
     * yield path must retain its own immediate scheduler semantics. */
    CHECK(wow_user_task_lifecycle_yield(&lifecycle));
    /* WOWEXEC must process a native posted command even when a hardware
     * wake is pending. TRUE skips guest PeekMessage; FALSE admits it. */
    CHECK(PostThreadMessageA(GetCurrentThreadId(), WM_APP + 55, 55, 0));
    /* Host queue observers can clear change bits without removing the
     * command. WOWEXEC's unfiltered message loop must still receive it. */
    (void)GetQueueStatus(QS_ALLINPUT);
    CHECK((GetQueueStatus(QS_ALLINPUT) & ((DWORD)QS_POSTMESSAGE << 16)) != 0);
    CHECK(wow_user_runtime_enter(&binding));
    lifecycle.process.W32PF_Flags |= 0x00010000; /* original W32PF_WAKEWOWEXEC */
    binding.thread->ptdb->nEvents = 0;
    lifecycle.process.shared->nEvents = 0;
    CHECK(wow_user_runtime_leave(&binding));
    CHECK(!wow_user_task_lifecycle_wait(&lifecycle, wowexec));
    CHECK(wow_user_task_lifecycle_message(&lifecycle, &message, NULL,
        WM_APP + 55, WM_APP + 55, PM_REMOVE | PM_NOYIELD, FALSE));
    CHECK(message.message == WM_APP + 55 && message.wParam == 55);
    CHECK(wow_user_runtime_enter(&binding));
    lifecycle.process.W32PF_Flags |= 0x00010000; /* hardware wake only */
    binding.thread->ptdb->nEvents = 0;
    lifecycle.process.shared->nEvents = 0;
    CHECK(wow_user_runtime_leave(&binding));
    CHECK(wow_user_task_lifecycle_wait(&lifecycle, wowexec));
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
    CHECK(PostThreadMessageW(GetCurrentThreadId(), WM_QUIT, 42, 0));
    CHECK(!wow_user_task_lifecycle_messageW(&lifecycle, &message, NULL,
        0, 0, PM_REMOVE, TRUE));
    CHECK(message.message == WM_QUIT && message.wParam == 42);
    CHECK(wow_user_task_lifecycle_messageW(&lifecycle, NULL, NULL,
        0, 0, PM_REMOVE, TRUE) == -1);
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && !binding.exclusive_held);
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
#ifdef WOW_MENU_CLEANUP_FIXTURE
    {
        extern LONG wow_menu_fixture_retire_fail;
        HMENU selected = CreateMenu(), other = CreateMenu();
        CHECK(selected && other);
        CHECK(wow_user_runtime_enter(&binding));
        CHECK(wow_menu_client_register_created(&lifecycle.process, 0x1234, selected));
        CHECK(wow_menu_client_register_created(&lifecycle.process, 0x5678, other));
        CHECK(wow_user_runtime_leave(&binding));
        CHECK(wow_user_task_lifecycle_exit(&lifecycle, (HANDLE)(ULONG_PTR)0x1234,
            0x1234, NULL, 0));
        CHECK(IsMenu(selected) && IsMenu(other));
        wow_menu_fixture_retire_fail = 1;
        CHECK(!wow_user_runtime_unbind(&binding));
        CHECK(lifecycle.cleanup.object_release_failed);
        CHECK(!binding.exclusive_held && binding.thread != NULL);
        CHECK(!IsMenu(selected) && IsMenu(other));
        CHECK(lifecycle.objects->entries[LOWORD(selected)].bType == TYPE_MENU);
        CHECK(wow_user_task_lifecycle_exit(&lifecycle, NULL,
            0x1234, NULL, 0));
        CHECK(!lifecycle.cleanup.object_release_failed);
        CHECK(lifecycle.objects->entries[LOWORD(selected)].bType == TYPE_FREE);
        CHECK(IsMenu(other) && lifecycle.objects->entries[LOWORD(other)].bType == TYPE_MENU);
        CHECK(wow_user_runtime_enter(&binding));
        CHECK(wow_menu_client_destroy_object(&lifecycle.process,
            &lifecycle.objects->entries[LOWORD(other)]));
        CHECK(wow_user_runtime_leave(&binding));
        CHECK(!IsMenu(other));
        fprintf(stderr, "WOW_MENU_CLEANUP errors=%u\n", errors);
    }
#endif
    CHECK(wow_user_task_lifecycle_exit(&lifecycle, (HANDLE)(ULONG_PTR)0x1234,
        0x1234, NULL, 0));
    CHECK(binding.thread != NULL && lifecycle.threads != NULL);
    CHECK(wow_user_task_lifecycle_exit(&lifecycle, (HANDLE)(ULONG_PTR)0x1234,
        0, NULL, 0));
    CHECK(binding.thread != NULL && binding.classes != NULL);
    CHECK(!binding.exclusive_held);
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(binding.thread == NULL && lifecycle.threads == NULL);
    CHECK(wow_user_idle_hook_for_current() == NULL);
#ifdef WOW_INIT_ROLLBACK_FIXTURE
    CHECK(!wow_init_name_live && wow_init_name_releases == 2);
    CHECK(!wow_init_hooks_live && wow_init_hook_failures == 3);
    fprintf(stderr, "WOW_IDLE_OBSERVER_LIFECYCLE errors=%u install_failures=%u live=%u\n",
        errors, wow_init_hook_failures, wow_init_hooks_live);
    fprintf(stderr, "WOW_INIT_ROLLBACK errors=%u failed_allocations=2 name_releases=%u\n",
        errors, wow_init_name_releases);
#endif
    CHECK(!wow_user_task_lifecycle_style(&lifecycle, &window_style));
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    fprintf(stderr, "WOW_USER_TASK_LIFECYCLE errors=%u native_direct=%u\n", errors, native_direct_calls);
    return errors ? 1 : 0;
}
