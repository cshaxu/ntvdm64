#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
#include "wow_user_private_access.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_class_client_bindings.h"
#include <stdio.h>

static unsigned errors, calls;
static BOOL mutate_during_create;
static wow_class_lookup_context *nested_context;
static const CREATESTRUCTA *nested_request;
static const WW *nested_words;
static ATOM nested_atom;
static HWND nested_window;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD set_guest_class(HWND window, DWORD procedure)
{
    BOOL previous_guest;
    return wow_user_set_procedure(window, TRUE, procedure, TRUE, &previous_guest);
}
static DWORD WINAPI callback(HWND w, UINT m, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    CHECK(words != NULL);
    ++calls;
    if (m == WM_NCCREATE && mutate_during_create) {
        mutate_during_create = FALSE;
        CHECK(proc == 0x94500090);
        CHECK(set_guest_class(w, 0x956000a0) == 0x94500090);
        nested_window = wow_window_create_boundA(wow_fixture_creation_owner(nested_context), nested_atom,
            nested_request, nested_words, NULL);
        CHECK(nested_window != NULL);
        if (nested_window)
            CHECK((DWORD)SendMessageA(nested_window, WM_APP, 0, 0) == 0x956000a0);
        CHECK((DWORD)wow_user_get_window_longA(w, GWL_WNDPROC) == 0x94500090);
    }
    if (m == WM_APP) return proc;
    return (DWORD)DefWindowProcA(w, m, wp, lp);
}
int __cdecl main(void)
{
    wow_class_lookup_context context = {0};
    WNDCLASSEXA cls = {sizeof(cls)};
    UNICODE_STRING name;
    DWORD wc[2] = {0x12340056, 0};
    WW ww = {0};
    CREATESTRUCTA create = {0};
    wow_window_dispatch_target target = {0x92300056, callback, TRUE};
    wow_window_dispatch_target override = {0x93400078, callback, TRUE}, bad = target;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_task_lifecycle lifecycle = {0};
    wow_task_order_state task_state = {0};
    wow_task_order_entry task = {0};
    wow_user_runtime_thread runtime_thread = {0};
    wow_class_client_context client = {0};
    HHOOK terminal;
    HWND first, second, third = NULL, pending = NULL;
    ATOM atom;
    RtlInitUnicodeString(&name, L"S40_CLASS_PROCEDURE");
    cls.lpfnWndProc = wow_window_class_creation_proc;
    cls.hInstance = GetModuleHandleA(NULL); cls.lpszClassName = "S40_CLASS_PROCEDURE";
    bad.procedure = 0;
    bad.wow_encoded = FALSE;
    CHECK(!wow_class_words_register_target(&context, &cls, &name, wc, NULL, &bad));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    bad = target; bad.callback = NULL;
    CHECK(!wow_class_words_register_target(&context, &cls, &name, wc, NULL, &bad));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    atom = wow_class_words_register_target(&context, &cls, &name, wc, NULL, &target);
    CHECK(atom != 0);
    if (!atom) return 1;
    client.classes = &context; client.callback = callback;
    client.thread = wow_fixture_creation_owner(&context);
    /* Same explicit native-fixture task identity as native-creation. Menu
     * publication consumes this identity even when no menu is attached. */
    fixture_creation_process.pwpi = &task_state;
    task_state.CSOwningThread = &fixture_creation_thread;
    fixture_creation_thread.ptdb = &task;
    task.hTaskWow = 0x4321;
    runtime.lifecycle = &lifecycle;
    CHECK(wow_user_runtime_bind(&runtime_thread, &runtime, client.thread, &client));
    terminal = SetWindowsHookExA(WH_CALLWNDPROCRET, wow_window_terminal_observer,
        NULL, GetCurrentThreadId());
    CHECK(terminal != NULL);
    /* Capture target by value: a caller's temporary cannot become the class. */
    target.procedure = 0;
    create.hInstance = cls.hInstance; create.lpszClass = cls.lpszClassName;
    create.lpszName = ""; create.hwndParent = HWND_MESSAGE;
    create.cx = create.cy = 20;
    first = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, NULL);
    second = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, &override);
    CHECK(first && second);
    if (first && second) {
        extern int __cdecl wow_typed_procedure_callers(HWND);
        CHECK(!wow_typed_procedure_callers(first));
        CHECK(GetClassLongA(first, GCL_WNDPROC) == (DWORD)wow_window_class_creation_proc);
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == 0x92300056);
        CHECK((DWORD)context.pclsPrivateList->lpfnWndProc == 0x92300056);
        CHECK(wow_user_get_class_longA(second, GCL_WNDPROC) == 0x92300056);
        CHECK((DWORD)SendMessageA(first, WM_APP, 0, 0) == 0x92300056);
        CHECK((DWORD)SendMessageA(second, WM_APP, 0, 0) == override.procedure);
        CHECK((DWORD)wow_user_get_window_longA(second, GWL_WNDPROC) == override.procedure);
        CHECK(set_guest_class(first, 0x94500090) == 0x92300056);
        CHECK(wow_user_get_class_longA(second, GCL_WNDPROC) == 0x94500090);
        CHECK((DWORD)context.pclsPrivateList->lpfnWndProc == 0x94500090);
        CHECK((DWORD)SendMessageA(first, WM_APP, 0, 0) == 0x92300056);
        CHECK((DWORD)SendMessageA(second, WM_APP, 0, 0) == override.procedure);
        third = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, NULL);
        CHECK(third != NULL);
        if (third) CHECK((DWORD)SendMessageA(third, WM_APP, 0, 0) == 0x94500090);
        CHECK(set_guest_class(first, 0xffff1234) == 0x94500090);
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == (LONG)0xffff1234);
        CHECK(set_guest_class(first, 0x94500090) == 0xffff1234);
        CHECK(!wow_user_set_class_longA(first, GCL_WNDPROC, (LONG)wow_window_class_creation_proc));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == 0x94500090);
        {
            BOOL previous_guest;
            CHECK(wow_user_set_procedure(first, FALSE, (DWORD)DefWindowProcA,
                FALSE, &previous_guest) == 0x92300056 && previous_guest);
        }
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == 0x94500090);
        CHECK((DWORD)wow_user_get_window_longA(first, GWL_WNDPROC) == (DWORD)DefWindowProcA);
        nested_context = &context; nested_atom = atom;
        nested_request = &create; nested_words = &ww;
        mutate_during_create = TRUE;
        pending = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, NULL);
        CHECK(pending && !mutate_during_create && nested_window);
        if (pending) CHECK((DWORD)SendMessageA(pending, WM_APP, 0, 0) == 0x94500090);
        CHECK(wow_user_get_class_longA(second, GCL_WNDPROC) == 0x956000a0);
        CHECK((DWORD)SendMessageA(second, WM_APP, 0, 0) == override.procedure);
    }
    if (first) CHECK(DestroyWindow(first));
    if (second) CHECK(DestroyWindow(second));
    if (third) CHECK(DestroyWindow(third));
    if (pending) CHECK(DestroyWindow(pending));
    if (nested_window) CHECK(DestroyWindow(nested_window));
    CHECK(wow_class_words_unregister(&context, atom, cls.hInstance, NULL));
    first = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 20, 20,
        HWND_MESSAGE, NULL, cls.hInstance, NULL);
    CHECK(first != NULL);
    if (first) {
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) ==
            GetClassLongA(first, GCL_WNDPROC));
        CHECK(DestroyWindow(first));
    }
    CHECK(UnhookWindowsHookEx(terminal));
    CHECK(wow_user_runtime_unbind(&runtime_thread));
    fprintf(stderr,"WOW_CLASS_PROCEDURE errors=%u callbacks=%u\n", errors, calls);
    return errors != 0;
}
