#include "wow_window_words_binding.h"
#include "wow_user_private_access.h"
#include "wow_task_order_bindings.h"
#include <stdio.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)
static unsigned errors, calls;
static unsigned unicode_text_calls;
static unsigned task_enters, task_leaves;
static unsigned overlay_calls;
/* Procedure-storage fixture only; task scheduling is verified by the real
 * lifecycle/CCPU suites, not this explicit callback-scope mock. */
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{ (void)scope; ++task_enters; return TRUE; }
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ (void)scope; ++task_leaves; return TRUE; }
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI callback(HWND hwnd, UINT msg, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    CHECK(((PWW)words)->vpfnWndProc == 0x11112222);
    if (msg == WM_USER) { ++calls; return proc; }
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK native(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 42;
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK overlay(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
    UINT_PTR id, DWORD_PTR data)
{
    (void)data;
    if (msg == WM_USER) ++overlay_calls;
    if (msg == WM_NCDESTROY) RemoveWindowSubclass(hwnd, overlay, id);
    return DefSubclassProc(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK unicode_native(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 84;
    if (msg == WM_SETTEXT) {
        CHECK(lstrcmpW((LPCWSTR)lp, L"S40 conversion") == 0);
        ++unicode_text_calls;
        return TRUE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
static void class_callproc_contract(WNDPROC callproc)
{
    static const WCHAR name_text[] = L"S40_CLASS_CALLPROC";
    UNICODE_STRING name = {sizeof(name_text) - sizeof(WCHAR), sizeof(name_text),
        (PWSTR)name_text};
    wow_class_lookup_context classes = {0};
    WNDCLASSEXA cls = {sizeof(cls)};
    wow_window_dispatch_target target = {0x92340056, callback}, snapshot;
    wow_class_words_binding *binding, *window_class;
    DWORD words[2] = {0}, previous = 0;
    WW window_words = {0};
    HWND window;
    ATOM atom;
    unsigned enters = task_enters, texts = unicode_text_calls;
    cls.lpfnWndProc = native;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpszClassName = "S40_CLASS_CALLPROC";
    atom = wow_class_words_register_target(&classes, &cls, &name,
        words, NULL, &target);
    CHECK(atom != 0);
    if (!atom) return;
    binding = wow_class_words_acquire(&classes, atom, cls.hInstance);
    CHECK(binding != NULL);
    if (!binding) return;
    CHECK(HIWORD(callproc) == 0xffff); /* Actual USER A/W identity, not a mock. */
    CHECK(wow_class_words_exchange_procedure(binding, (DWORD)callproc, &previous));
    CHECK(previous == target.procedure);
    CHECK(wow_class_words_target(binding, &snapshot));
    CHECK(snapshot.procedure == (DWORD)callproc);
    CHECK(!((DWORD)wow_class_words_entry(binding)->lpfnWndProc & 0x80000000));
    window = CreateWindowExA(0, cls.lpszClassName, "", 0,
        0, 0, 16, 16, NULL, NULL, cls.hInstance, NULL);
    CHECK(window != NULL);
    if (window) {
        window_class = wow_class_words_acquire(&classes, atom, cls.hInstance);
        CHECK(window_class != NULL);
        if (!wow_window_words_attach_target(window, &window_words,
                window_class, &snapshot)) {
            CHECK(FALSE);
            wow_class_words_release(window_class);
        } else {
            CHECK(SetWindowLongA(window, GWL_WNDPROC,
                (LONG)wow_window_native_proc) != 0);
            CHECK(wow_user_get_class_longA(window, GCL_WNDPROC) == (DWORD)callproc);
            CHECK(SendMessageA(window, WM_SETTEXT, 0, (LPARAM)"S40 conversion"));
            previous = wow_user_set_class_longA(window, GCL_WNDPROC, target.procedure);
            CHECK(previous == (DWORD)callproc);
            /* SetClassLong does not rewrite already-created WND procedures. */
            CHECK(SendMessageA(window, WM_SETTEXT, 0, (LPARAM)"S40 conversion"));
            CHECK(wow_class_words_exchange_procedure(binding, (DWORD)callproc, &previous));
            CHECK(previous == target.procedure);
            CHECK(wow_class_words_target(binding, &snapshot));
            CHECK(snapshot.procedure == (DWORD)callproc);
            CHECK(unicode_text_calls == texts + 2 && task_enters == enters);
        }
        CHECK(DestroyWindow(window));
    }
    wow_class_words_release(binding);
    CHECK(wow_class_words_unregister(&classes, atom, cls.hInstance, NULL));
    CHECK(classes.pclsPrivateList == NULL);
    fprintf(stderr, "WOW_CLASS_CALLPROC restored=1 isolated=1 errors=%u\n", errors);
}
static void native_callproc_contract(void)
{
    WNDCLASSEXW cls = {sizeof(cls)};
    HWND hwnd;
    WNDPROC ansi_view, previous;
    LONG restored;
    WW words = {0};
    wow_window_dispatch_target target = {0, callback};
    DWORD unchanged = 0x12345678;
    LONG result = 0x12345678;
    cls.lpfnWndProc = unicode_native;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpszClassName = L"S40_UNICODE_CALLPROC";
    CHECK(RegisterClassExW(&cls) != 0);
    hwnd = CreateWindowExW(0, cls.lpszClassName, L"", 0,
        0, 0, 16, 16, NULL, NULL, cls.hInstance, NULL);
    CHECK(hwnd != NULL);
    if (!hwnd) return;
    CHECK(IsWindowUnicode(hwnd));
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    ansi_view = (WNDPROC)GetWindowLongA(hwnd, GWL_WNDPROC);
    CHECK(ansi_view != NULL);
    /* Dialog/metadata-only enrollment is not a procedure owner. Its public
     * native procedure must remain visible and replaceable through USER. */
    CHECK(wow_window_words_attach(hwnd, &words));
    CHECK((WNDPROC)wow_user_get_window_longA(hwnd, GWL_WNDPROC) == ansi_view);
    previous = (WNDPROC)wow_user_set_window_longA(hwnd, GWL_WNDPROC, (LONG)native);
    CHECK(previous == ansi_view);
    CHECK(wow_user_get_window_longA(hwnd, GWL_WNDPROC) == (LONG)native);
    CHECK((WNDPROC)wow_user_set_window_longA(hwnd, GWL_WNDPROC,
        (LONG)previous) == native);
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    wow_window_words_detach(hwnd);
    /* A native callproc token must go through CallWindowProc, never the
     * original DispatchClientMessage raw-procedure entry or a guest callback.
     * Do not assume modern USER exposes NT4's FFFF token representation. */
    CHECK(CallWindowProcA(ansi_view, hwnd, WM_USER, 0, 0) == 84);
    CHECK(CallWindowProcA(ansi_view, hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    /* The binding accepts a real public identity but normalizes the original
     * cleanup/dispatch field. Raw CPD is still forbidden at direct dispatch. */
    target.procedure = (DWORD)ansi_view;
    CHECK(wow_window_words_attach_target(hwnd, &words, NULL, &target));
    CHECK(wow_window_procedure_read(hwnd, &unchanged) && unchanged == (DWORD)ansi_view);
    CHECK(wow_window_native_proc(hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    CHECK(wow_window_procedure_exchange(hwnd, 0x92340056, &unchanged));
    CHECK(unchanged == (DWORD)ansi_view);
    CHECK(wow_window_procedure_exchange(hwnd, (DWORD)ansi_view, &unchanged));
    CHECK(unchanged == 0x92340056);
    CHECK(!wow_window_dispatch_bound(hwnd, WM_USER, 0, 0,
        0xffff1234, callback, &result));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && result == 0x12345678);
    CHECK(calls == 0);
    wow_window_words_detach(hwnd);
    CHECK(!wow_user_set_window_longA(hwnd, GWL_WNDPROC, 0x92340056));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    CHECK(!wow_user_set_class_longA(hwnd, GCL_WNDPROC, 0x92340056));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK((WNDPROC)GetClassLongW(hwnd, GCL_WNDPROC) == unicode_native);
    previous = (WNDPROC)wow_user_set_window_longA(hwnd, GWL_WNDPROC, (LONG)native);
    CHECK(previous != NULL);
    CHECK(!IsWindowUnicode(hwnd));
    CHECK(CallWindowProcA(previous, hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    restored = wow_user_set_window_longA(hwnd, GWL_WNDPROC, (LONG)previous);
    CHECK((WNDPROC)restored == native);
    CHECK(IsWindowUnicode(hwnd));
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    CHECK(SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)L"S40 conversion"));
    CHECK(unicode_text_calls == 4);
    class_callproc_contract(ansi_view);
    fprintf(stderr, "WOW_NATIVE_CALLPROC ansi=%08lx previous=%08lx native=%08lx text_calls=%u\n",
        (DWORD)ansi_view, (DWORD)previous, (DWORD)unicode_native, unicode_text_calls);
    CHECK(DestroyWindow(hwnd));
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
}
int __cdecl main(void)
{
    HWND windows[2];
    WW words = {0};
    wow_window_dispatch_target target = {0x92340056, callback};
    DWORD previous, value;
    LONG host_top;
    unsigned i;
    native_callproc_contract();
    words.vpfnWndProc = 0x11112222;
    for (i = 0; i != 2; ++i) {
        windows[i] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
            NULL, NULL, GetModuleHandleA(NULL), NULL);
        CHECK(windows[i] != NULL);
        CHECK(wow_window_words_attach_target(windows[i], &words, NULL, &target));
        CHECK(SetWindowLongA(windows[i], GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    }
    CHECK(wow_window_procedure_read(windows[0], &value) && value == 0x92340056);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)wow_window_native_proc);
    CHECK(SetWindowSubclass(windows[0], overlay, 1, 0));
    host_top = GetWindowLongA(windows[0], GWL_WNDPROC);
    CHECK(host_top != (LONG)wow_window_native_proc);
    CHECK((DWORD)wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0x92340056);
    previous = (DWORD)wow_user_set_window_longA(windows[0], GWL_WNDPROC, 0x92300056);
    CHECK(previous == 0x92340056);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == host_top);
    CHECK((DWORD)wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0x92300056);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == 0x92300056);
    CHECK((DWORD)SendMessageA(windows[1], WM_USER, 0, 0) == 0x92340056);
    previous = (DWORD)wow_user_set_window_longA(windows[0], GWL_WNDPROC, (LONG)native);
    CHECK(previous == 0x92300056);
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == (LONG)native);
    CHECK(SendMessageA(windows[0], WM_USER, 0, 0) == 42);
    value = 0xabcdef01;
    CHECK(!wow_window_procedure_exchange(windows[0], (DWORD)wow_window_native_proc, &value));
    CHECK(value == 0xabcdef01 && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_procedure_exchange(windows[0], target.procedure, &previous));
    CHECK(previous == (DWORD)native);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == target.procedure);
    CHECK(calls == 3);
    CHECK(overlay_calls == 3);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == host_top);
    for (i = 0; i != 2; ++i) CHECK(DestroyWindow(windows[i]));
    CHECK(!wow_window_procedure_read(windows[0], &value));
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    windows[0] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(windows[0] != NULL);
    CHECK(SetWindowLongA(windows[0], GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_window_words_attach(windows[0], &words));
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    CHECK(GetLastError() == ERROR_INVALID_DATA);
    CHECK(!wow_user_set_window_longA(windows[0], GWL_WNDPROC, (LONG)native));
    CHECK(GetLastError() == ERROR_INVALID_DATA);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)wow_window_native_proc);
    wow_window_words_detach(windows[0]);
    CHECK(DestroyWindow(windows[0]));
    CHECK(task_enters && task_enters == task_leaves);
    fprintf(stderr, "WOW_WINDOW_PROCEDURE errors=%u tagged_calls=%u\n", errors, calls);
    fprintf(stderr, "WOW_NATIVE_SUBCLASS errors=%u forwarded=%u preserved=1 missing_owner_rejected=1\n", errors, overlay_calls);
    return errors ? 3 : 0;
}
