#include "precomp.h"
#include "wow_idle_hook_binding.h"
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* The witness pumps a native window, not a WOW window projection. */
#undef GetMessageA
#undef ShowWindow
#undef DestroyWindow
#undef DispatchMessageA
#undef GetForegroundWindow
#undef CallNextHookEx

/* The focused fixture supplies only the hook-registration ABI; idle-chain
 * forwarding must link directly to USER, without a guest-publication shim. */
HHOOK WINAPI wow_native_SetWindowsHookExA(int id, HOOKPROC proc,
    HINSTANCE module, DWORD thread_id)
{
    typedef HHOOK (WINAPI *set_hook_fn)(int, HOOKPROC, HINSTANCE, DWORD);
    set_hook_fn set_hook = (set_hook_fn)GetProcAddress(
        GetModuleHandleA("user32.dll"), "SetWindowsHookExA");
    return set_hook ? set_hook(id, proc, module, thread_id) : NULL;
}

PTD gptdTaskHead;
CRITICAL_SECTION gcsWOW;
static wow_user_runtime_thread runtime;
static wow_task_order_thread thread;
static wow_task_order_entry entry;
static unsigned errors, task_depth, borrow_depth, calls;
static BOOL foreign_context, deny_task, deny_borrow, nested, exceptional;
static unsigned next_calls;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d\n", __LINE__); } } while (0)

/* Bounded order/registration witness only: no global native hook is installed,
 * and mocked scope helpers do not prove production scheduler ownership. */
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void)
{
    return foreign_context ? NULL : &runtime;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    ZeroMemory(scope, sizeof(*scope));
    if (deny_task) return FALSE;
    ++task_depth;
    return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth + 1);
    --task_depth;
    return TRUE;
}
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth + 1);
    if (deny_borrow) return FALSE;
    ++borrow_depth;
    return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(task_depth && task_depth == borrow_depth);
    --borrow_depth;
}
LRESULT CALLBACK WowOriginalForegroundIdleHook(int code, WPARAM wp, LPARAM lp)
{
    ++calls;
    CHECK(code == HC_ACTION && wp == 0 && lp == 0);
    CHECK(task_depth && task_depth == borrow_depth);
    if (nested && task_depth == 1) wow_user_idle_hook_dispatch();
    if (exceptional) RaiseException(0xe0447014, 0, 0, NULL);
    SetLastError(0xe0447015);
    return 93;
}

static LRESULT CALLBACK next_idle(int code, WPARAM wp, LPARAM lp)
{
    if (code >= 0) ++next_calls;
    return CallNextHookEx(NULL, code, wp, lp);
}

/* Real USER chain, not a direct HookProc call. Only a fresh owned desktop is
 * activated, and its prior input desktop is restored before destruction. */
typedef struct native_chain_context {
    TD *registered;
    HDESK desktop;
} native_chain_context;

static DWORD WINAPI native_chain(void *argument)
{
    native_chain_context *context = argument;
    TD *registered = context->registered;
    HDESK previous = OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP);
    HDESK desktop;
    HWND window = NULL;
    HHOOK first = NULL, next = NULL;
    char name[64];
    unsigned phase, before, after;
    BOOL switched = FALSE;
    sprintf_s(name, sizeof(name), "wow-idle-chain-%lu", GetCurrentProcessId());
    desktop = CreateDesktopA(name, NULL, NULL, 0, GENERIC_ALL, NULL);
    context->desktop = desktop;
    thread.native_thread_id = GetCurrentThreadId();
    CHECK(previous && desktop);
    if (!previous || !desktop) goto done;
    if (!SetThreadDesktop(desktop)) {
        CHECK(FALSE);
        goto done;
    }
    window = CreateWindowA("STATIC", "owned idle chain", WS_OVERLAPPEDWINDOW,
        0, 0, 240, 140, NULL, NULL, GetModuleHandle(NULL), NULL);
    CHECK(window != NULL);
    if (!window) goto done;
    next = SetWindowsHookExA(WH_FOREGROUNDIDLE, next_idle, NULL, GetCurrentThreadId());
    first = SetWindowsHookExA(WH_FOREGROUNDIDLE, WK32ForegroundIdleHook, NULL, GetCurrentThreadId());
    CHECK(first && next);
    switched = SwitchDesktop(desktop);
    CHECK(switched);
    if (!switched || !first || !next) goto done;
    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    CHECK(GetForegroundWindow() == window);
    for (phase = 0; phase != 2; ++phase) {
        unsigned ticks = 0;
        MSG message;
        registered->hIdleHook = phase ? NULL : first;
        before = calls; after = next_calls;
        CHECK(SetTimer(window, 1, 40, NULL) != 0);
        while (ticks < 6 && GetMessage(&message, NULL, 0, 0) > 0) {
            if (message.message == WM_TIMER) ++ticks;
            TranslateMessage(&message); DispatchMessage(&message);
        }
        KillTimer(window, 1);
        CHECK(next_calls > after);
        CHECK(phase ? calls == before : calls - before == next_calls - after);
    }
    before = next_calls;
    registered->hIdleHook = first;
    wow_user_idle_hook_dispatch();
    CHECK(next_calls == before);
done:
    registered->hIdleHook = NULL;
    if (first) CHECK(UnhookWindowsHookEx(first));
    if (next) CHECK(UnhookWindowsHookEx(next));
    if (switched) CHECK(SwitchDesktop(previous));
    if (window) DestroyWindow(window);
    if (previous) CloseDesktop(previous);
    fprintf(stderr, "WOW_IDLE_NATIVE_CHAIN errors=%u next=%u registered_and_unregistered=1 direct_not_forwarded=1\n", errors, next_calls);
    return 0;
}

int __cdecl main(int argc, char **argv)
{
    TD first = {0}, second = {0};
    BOOL caught = FALSE;
    BOOL logic_only = argc == 2 && !strcmp(argv[1], "--logic-only");
    InitializeCriticalSection(&gcsWOW);
    runtime.thread = &thread;
    thread.ptdb = &entry;
    thread.native_thread_id = GetCurrentThreadId();
    SetLastError(0xe0447015);
    CHECK(!wow_user_idle_hook_registered(thread.native_thread_id));
    CHECK(GetLastError() == 0xe0447015);
    gptdTaskHead = &first;
    first.ptdNext = &second;
    CHECK(!wow_user_idle_hook_registered(thread.native_thread_id));
    second.hIdleHook = (HHOOK)(ULONG_PTR)1; /* Mock original TD state only. */
    second.dwThreadID = thread.native_thread_id;
    CHECK(!wow_user_idle_hook_registered(thread.native_thread_id)); /* Published but not initialized. */
    CHECK(GetLastError() == 0xe0447015);
    second.hThread = (HANDLE)(ULONG_PTR)1;
    CHECK(wow_user_idle_hook_registered(thread.native_thread_id));
    CHECK(wow_user_idle_hook_registered(0));
    (void)WK32ForegroundIdleHook(HC_ACTION, 0, 0);
    CHECK(GetLastError() == 0xe0447015);
    nested = TRUE;
    wow_user_idle_hook_dispatch();
    nested = FALSE;
    exceptional = TRUE;
    __try { wow_user_idle_hook_dispatch(); }
    __except (GetExceptionCode() == 0xe0447014 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    exceptional = FALSE;
    CHECK(caught && calls == 4 && !task_depth && !borrow_depth);
    deny_task = TRUE;
    wow_user_idle_hook_dispatch();
    deny_task = FALSE;
    deny_borrow = TRUE;
    wow_user_idle_hook_dispatch();
    deny_borrow = FALSE;
    foreign_context = TRUE;
    (void)WK32ForegroundIdleHook(HC_ACTION, 0, 0);
    foreign_context = FALSE;
    thread.ptdb = NULL;
    (void)WK32ForegroundIdleHook(HC_ACTION, 0, 0);
    CHECK(calls == 4 && !task_depth && !borrow_depth);
    second.hIdleHook = NULL;
    CHECK(!wow_user_idle_hook_registered(thread.native_thread_id));
    thread.ptdb = &entry;
    first.dwThreadID = thread.native_thread_id + 1;
    first.hThread = (HANDLE)(ULONG_PTR)2;
    first.hIdleHook = (HHOOK)(ULONG_PTR)2;
    CHECK(!wow_user_idle_hook_registered(thread.native_thread_id));
    CHECK(wow_user_idle_hook_registered(0));
    (void)WK32ForegroundIdleHook(HC_ACTION, 0, 0);
    /* Original registration is worker-shared: a foreign live TD's intent
     * causes this selected task to receive INT 2F:1689. */
    CHECK(calls == 5 && !task_depth && !borrow_depth);
    first.hIdleHook = NULL;
    (void)WK32ForegroundIdleHook(HC_ACTION, 0, 0);
    CHECK(calls == 5 && !task_depth && !borrow_depth);
    fprintf(stderr, "WOW_IDLE_HOOK_GUARD errors=%u calls=%u nested=1 exception=1 denied=2 foreign=2 retired=1 worker_shared=1\n",
        errors, calls);
    if (logic_only) {
        fprintf(stderr, "WOW_IDLE_NATIVE_CHAIN unavailable=logic-only\n");
    } else {
        native_chain_context context = { &second, NULL };
        HANDLE worker = CreateThread(NULL, 0, native_chain, &context, 0, NULL);
        CHECK(worker != NULL);
        if (worker) {
            if (WaitForSingleObject(worker, INFINITE) != WAIT_OBJECT_0) {
                fprintf(stderr, "FAIL idle test thread wait error=%lu\n", GetLastError());
                ExitProcess(1); /* Do not release a context still used by the thread. */
            }
            CHECK(CloseHandle(worker));
        }
        if (context.desktop) CHECK(CloseDesktop(context.desktop));
    }
    gptdTaskHead = NULL;
    DeleteCriticalSection(&gcsWOW);
    return errors ? 1 : 0;
}
