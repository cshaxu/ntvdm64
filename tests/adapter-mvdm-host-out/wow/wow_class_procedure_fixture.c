#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
#include "wow_user_private_access.h"
#include <stdio.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

static unsigned errors, calls;
static BOOL mutate_during_create;
static wow_class_lookup_context *nested_context;
static const CREATESTRUCTA *nested_request;
static const WW *nested_words;
static ATOM nested_atom;
static HWND nested_window;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI callback(HWND w, UINT m, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    CHECK(words != NULL);
    ++calls;
    if (m == WM_NCCREATE && mutate_during_create) {
        mutate_during_create = FALSE;
        CHECK(proc == 0x94500090);
        CHECK(wow_user_set_class_longA(w, GCL_WNDPROC, 0x956000a0) == 0x94500090);
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
    wow_window_dispatch_target target = {0x92300056, callback};
    wow_window_dispatch_target override = {0x93400078, callback}, bad = target;
    HWND first, second, third = NULL, pending = NULL;
    ATOM atom;
    RtlInitUnicodeString(&name, L"S40_CLASS_PROCEDURE");
    cls.lpfnWndProc = wow_window_native_proc;
    cls.hInstance = GetModuleHandleA(NULL); cls.lpszClassName = "S40_CLASS_PROCEDURE";
    bad.procedure = 0xffff1234;
    CHECK(!wow_class_words_register_target(&context, &cls, &name, wc, NULL, &bad));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    bad = target; bad.callback = NULL;
    CHECK(!wow_class_words_register_target(&context, &cls, &name, wc, NULL, &bad));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    atom = wow_class_words_register_target(&context, &cls, &name, wc, NULL, &target);
    CHECK(atom != 0);
    if (!atom) return 1;
    /* Capture target by value: a caller's temporary cannot become the class. */
    target.procedure = 0;
    create.hInstance = cls.hInstance; create.lpszClass = cls.lpszClassName;
    create.lpszName = ""; create.hwndParent = HWND_MESSAGE;
    create.cx = create.cy = 20;
    first = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, NULL);
    second = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, &override);
    CHECK(first && second);
    if (first && second) {
        CHECK(GetClassLongA(first, GCL_WNDPROC) == (DWORD)wow_window_native_proc);
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == 0x92300056);
        CHECK((DWORD)context.pclsPrivateList->lpfnWndProc == 0x92300056);
        CHECK(wow_user_get_class_longA(second, GCL_WNDPROC) == 0x92300056);
        CHECK((DWORD)SendMessageA(first, WM_APP, 0, 0) == 0x92300056);
        CHECK((DWORD)SendMessageA(second, WM_APP, 0, 0) == override.procedure);
        CHECK((DWORD)wow_user_get_window_longA(second, GWL_WNDPROC) == override.procedure);
        CHECK(wow_user_set_class_longA(first, GCL_WNDPROC, 0x94500090) == 0x92300056);
        CHECK(wow_user_get_class_longA(second, GCL_WNDPROC) == 0x94500090);
        CHECK((DWORD)context.pclsPrivateList->lpfnWndProc == 0x94500090);
        CHECK((DWORD)SendMessageA(first, WM_APP, 0, 0) == 0x92300056);
        CHECK((DWORD)SendMessageA(second, WM_APP, 0, 0) == override.procedure);
        third = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &create, &ww, NULL);
        CHECK(third != NULL);
        if (third) CHECK((DWORD)SendMessageA(third, WM_APP, 0, 0) == 0x94500090);
        CHECK(!wow_user_set_class_longA(first, GCL_WNDPROC, (LONG)0xffff1234));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(!wow_user_set_class_longA(first, GCL_WNDPROC, (LONG)wow_window_native_proc));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(wow_user_get_class_longA(first, GCL_WNDPROC) == 0x94500090);
        CHECK((DWORD)wow_user_set_window_longA(first, GWL_WNDPROC,
            (LONG)DefWindowProcA) == 0x92300056);
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
    printf("WOW_CLASS_PROCEDURE errors=%u callbacks=%u\n", errors, calls);
    return errors != 0;
}
