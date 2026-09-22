#include "wow_user_task_lifecycle.h"
#include "wow_class_words_binding.h"
#include "wow_window_creation_binding.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/session/session.h"

#ifndef CW_FLAGS_ANSI
#define CW_FLAGS_ANSI 0x00000001u
#endif

#include <stdio.h>

static unsigned errors;
static unsigned dispatched;
#define CHECK(value) do { if (!(value)) { ++errors; \
    printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

static VOID WINAPI end_task(void)
{
    ++errors;
}

static DWORD WINAPI window_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    UNREFERENCED_PARAMETER(window);
    UNREFERENCED_PARAMETER(wp);
    UNREFERENCED_PARAMETER(lp);
    CHECK(procedure == 0x92350020u && words != NULL);
    if (message == WM_APP + 120) {
        ++dispatched;
        return 0x4a20;
    }
    return (DWORD)DefWindowProcA(window, message, wp, lp);
}

typedef struct child_task_context {
    session *session;
    wow_user_runtime *runtime;
    wow_user_task_lifecycle *lifecycle;
    HANDLE ready;
    HANDLE start;
    HANDLE complete;
    HANDLE cleanup;
    HWND window;
} child_task_context;

static DWORD WINAPI child_window_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    UNREFERENCED_PARAMETER(window);
    UNREFERENCED_PARAMETER(wp);
    UNREFERENCED_PARAMETER(lp);
    CHECK(procedure == 0x92350021u && words != NULL);
    if (message == WM_APP + 121) {
        ++dispatched;
        return 0x4a21;
    }
    return (DWORD)DefWindowProcA(window, message, wp, lp);
}

static DWORD WINAPI child_task(void *parameter)
{
    child_task_context *context = parameter;
    wow_user_runtime_thread binding = {0};
    wow_task_order_thread *thread;
    wow_window_dispatch_target target = { 0x92350021u, child_window_callback };
    WNDCLASSEXA window_class = { sizeof(window_class) };
    CREATESTRUCTA request = {0};
    UNICODE_STRING name;
    DWORD words[2] = { 0x12350021u, 0 };
    WW window_words = {0};
    ATOM atom = 0;
    DWORD result = 1;

    if (!session_thread_bind_owned(context->session,
            SESSION_THREAD_BINDING_ORIGINAL_WORKER) ||
            !wow_user_runtime_bind(&binding, context->runtime, NULL, NULL) ||
            !wow_user_task_lifecycle_init(context->lifecycle, 0x0400,
                "CHILD.EXE", 0x4321, 0, 0, (DWORD)CW_USEDEFAULT,
                (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
                (DWORD)CW_USEDEFAULT, SW_SHOW)) goto done;
    thread = wow_user_runtime_current()->thread;
    window_class.lpfnWndProc = wow_window_native_proc;
    window_class.hInstance = GetModuleHandleA(NULL);
    window_class.lpszClassName = "S40_CHILD_WINDOW";
    RtlInitUnicodeString(&name, L"S40_CHILD_WINDOW");
    atom = wow_class_words_register_target(&context->lifecycle->classes,
        &window_class, &name, words, NULL, &target);
    if (!atom) goto done;
    request.hInstance = window_class.hInstance;
    request.lpszClass = window_class.lpszClassName;
    request.lpszName = "";
    request.cx = request.cy = 32;
    context->window = wow_window_create_boundA(thread, atom, &request,
        &window_words, &target);
    if (!context->window) goto done;
    SetEvent(context->ready);
    if (WaitForSingleObject(context->start, 5000) != WAIT_OBJECT_0 ||
            !wow_user_task_lifecycle_yield(context->lifecycle)) goto done;
    SetEvent(context->complete);
    if (WaitForSingleObject(context->cleanup, 5000) != WAIT_OBJECT_0) goto done;
    if (!DestroyWindow(context->window) ||
            !wow_class_words_unregister(&context->lifecycle->classes, atom,
                window_class.hInstance, NULL) ||
            !wow_user_task_lifecycle_cleanup(context->lifecycle, 0x4321))
        goto done;
    context->window = NULL;
    atom = 0;
    result = 0;
done:
    if (result) ++errors;
    if (context->window) DestroyWindow(context->window);
    if (atom) wow_class_words_unregister(&context->lifecycle->classes, atom,
        window_class.hInstance, NULL);
    if (wow_user_runtime_current() == &binding)
        (void)wow_user_runtime_unbind(&binding);
    (void)session_thread_unbind(context->session);
    return result;
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    wow_window_dispatch_target target = { 0x92350020u, window_callback };
    WNDCLASSEXA window_class = { sizeof(window_class) };
    CREATESTRUCTA request = {0};
    UNICODE_STRING name;
    DWORD words[2] = { 0x12350020u, 0 };
    WW window_words = {0};
    HANDLE wowexec = CreateEventW(NULL, FALSE, FALSE, NULL);
    wow_task_order_thread *thread;
    wow_window_words_binding *borrow;
    child_task_context child = {0};
    HANDLE child_thread;
    DWORD child_code;
    DWORD_PTR child_reply = 0;
    ATOM atom;
    HWND window;
    DWORD procedure = 0;

    session_initialize(&owner, 42040u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, wowexec));
    CHECK(wow_user_task_lifecycle_init(&lifecycle, 0x0400, "WINDOW.EXE",
        0x1234, 0, 0, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_SHOW));

    thread = wow_user_runtime_current()->thread;
    window_class.lpfnWndProc = wow_window_native_proc;
    window_class.hInstance = GetModuleHandleA(NULL);
    window_class.lpszClassName = "S40_TASK_WINDOW";
    RtlInitUnicodeString(&name, L"S40_TASK_WINDOW");
    atom = wow_class_words_register_target(&lifecycle.classes, &window_class,
        &name, words, NULL, &target);
    CHECK(atom != 0);
    request.hInstance = window_class.hInstance;
    request.lpszClass = window_class.lpszClassName;
    request.lpszName = "";
    request.cx = request.cy = 32;
    window_words.vpfnWndProc = target.procedure;
    window = wow_user_create_window_ex(0, request.lpszClass, request.lpszName,
        request.style, request.x, request.y, request.cx, request.cy,
        request.hwndParent, request.hMenu, request.hInstance,
        request.lpCreateParams, CW_FLAGS_ANSI, window_words.adwWOW);
    CHECK(window != NULL);
    borrow = window ? wow_window_words_acquire(window) : NULL;
    CHECK(borrow != NULL);
    CHECK(window && lifecycle.objects->entries[LOWORD(window)].pOwner == thread);

    /* A second actual lifecycle task owns its own USER32 queue.  The main
     * thread sends synchronously; the target task alone runs the selected
     * original Yield/Receive path to reply. */
    child.session = &owner;
    child.runtime = &runtime;
    child.lifecycle = &lifecycle;
    child.ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    child.start = CreateEventW(NULL, TRUE, FALSE, NULL);
    child.complete = CreateEventW(NULL, TRUE, FALSE, NULL);
    child.cleanup = CreateEventW(NULL, TRUE, FALSE, NULL);
    child_thread = CreateThread(NULL, 0, child_task, &child, 0, NULL);
    CHECK(child.ready && child.start && child.complete && child.cleanup && child_thread);
    CHECK(WaitForSingleObject(child.ready, 5000) == WAIT_OBJECT_0 && child.window);
    CHECK(SetEvent(child.start));
    CHECK(SendMessageTimeoutA(child.window, WM_APP + 121, 0, 0,
        SMTO_ABORTIFHUNG, 5000, &child_reply));
    CHECK(child_reply == 0x4a21 && dispatched == 1);
    CHECK(WaitForSingleObject(child.complete, 5000) == WAIT_OBJECT_0);
    CHECK(SetEvent(child.cleanup));
    CHECK(WaitForSingleObject(child_thread, 5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(child_thread, &child_code) && child_code == 0);
    CHECK(CloseHandle(child_thread));
    CHECK(CloseHandle(child.ready));
    CHECK(CloseHandle(child.start));
    CHECK(CloseHandle(child.complete));
    CHECK(CloseHandle(child.cleanup));

    /* This is the actual selected taskman route: a native posted message is
     * consumed only when original xxxUserYield calls xxxReceiveMessages for
     * the current scheduled 16-bit task.  No fixture-side queue drain is
    * allowed here. */
    CHECK(PostMessageA(window, WM_APP + 120, 0, 0));
    /* Original xxxSleepTask returns FALSE after the normal re-selection of
     * the current 16-bit task.  Its BOOL is not a success predicate. */
    SetLastError(ERROR_SUCCESS);
    (void)wow_user_task_lifecycle_wait(&lifecycle, wowexec);
    CHECK(GetLastError() == ERROR_SUCCESS);
    CHECK(wow_user_task_lifecycle_yield(&lifecycle));
    CHECK(dispatched == 2);

    /* Original task cleanup must neutralize this task's callable window
     * procedure before the task's original DestroyTask removes its TDB. */
    CHECK(wow_user_task_lifecycle_wow_cleanup(&lifecycle,
        (HANDLE)(ULONG_PTR)0x1234, 0x1234, NULL, 0));
    CHECK(wow_window_procedure_read(window, &procedure));
    CHECK(procedure == (DWORD)DefWindowProcA);
    CHECK(DestroyWindow(window));
    CHECK(wow_class_words_unregister(&lifecycle.classes, atom,
        window_class.hInstance, NULL));
    wow_window_words_release(borrow);

    CHECK(wow_user_task_lifecycle_cleanup(&lifecycle, 0x1234));
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    printf("WOW_USER_TASK_WINDOW_LIFECYCLE errors=%u\n", errors);
    return errors ? 1 : 0;
}
