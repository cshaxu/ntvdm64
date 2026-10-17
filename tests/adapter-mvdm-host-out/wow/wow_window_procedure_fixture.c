#include "wow_window_words_binding.h"
#include "wow_user_private_access.h"
#include "wow_task_order_bindings.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"
#include "wow_user_object_bindings.h"
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
static WNDPROC overlay_previous;
static BOOL forward_overlay;
/* Count the real scope implementation, not a success-only replacement.
 * The runner renames only these two exports in its selected lifecycle object.
 * This native procedure fixture has no guest task; scheduling itself is
 * verified by the separate real lifecycle/CCPU suites. */
BOOL WINAPI wow_fixture_real_callback_enter(wow_task_callback_scope *scope);
BOOL WINAPI wow_fixture_real_callback_leave(wow_task_callback_scope *scope);
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{ ++task_enters; return wow_fixture_real_callback_enter(scope); }
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ ++task_leaves; return wow_fixture_real_callback_leave(scope); }
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI callback(HWND hwnd, UINT msg, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    CHECK(((PWW)words)->vpfnWndProc == 0x11112222);
    if (msg == WM_USER) {
        ++calls;
        if (forward_overlay && proc == 0x92300056)
            CHECK((DWORD)CallWindowProcA(overlay_previous, hwnd, msg, wp, lp) == 0x92340056);
        return proc;
    }
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
static BOOL class_ansi_text;
static LRESULT CALLBACK unicode_native(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 84;
    if (msg == WM_SETTEXT) {
        if (class_ansi_text) CHECK(lstrcmpA((LPCSTR)lp, "S40 conversion") == 0);
        else CHECK(lstrcmpW((LPCWSTR)lp, L"S40 conversion") == 0);
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
    wow_window_dispatch_target target = {0x92340056, callback, TRUE}, snapshot;
    wow_class_words_binding *binding, *window_class;
    DWORD words[2] = {0}, previous = 0;
    BOOL previous_guest;
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
    CHECK(wow_class_words_exchange_procedure(binding, (DWORD)callproc, FALSE,
        &previous, &previous_guest));
    CHECK(previous == target.procedure && previous_guest);
    CHECK(wow_class_words_target(binding, &snapshot));
    CHECK(snapshot.procedure == (DWORD)unicode_native && !snapshot.wow_encoded);
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
                (LONG)snapshot.procedure) != 0);
            CHECK(wow_user_get_class_longA(window, GCL_WNDPROC) == (DWORD)unicode_native);
            /* Original SetClassLongA keeps its caller encoding, not the
             * source CPD encoding. WND setter tests below still require
             * native A/W conversion and must not inherit this class rule. */
            class_ansi_text = TRUE;
            CHECK(SendMessageA(window, WM_SETTEXT, 0, (LPARAM)"S40 conversion"));
            previous = wow_user_set_procedure(window, TRUE, target.procedure, TRUE, &previous_guest);
            CHECK(previous == (DWORD)unicode_native && !previous_guest);
            /* SetClassLong does not rewrite already-created WND procedures. */
            CHECK(SendMessageA(window, WM_SETTEXT, 0, (LPARAM)"S40 conversion"));
            CHECK(wow_class_words_exchange_procedure(binding, (DWORD)callproc, FALSE,
                &previous, &previous_guest));
            CHECK(previous == target.procedure && previous_guest);
            CHECK(wow_class_words_target(binding, &snapshot));
            CHECK(snapshot.procedure == (DWORD)unicode_native && !snapshot.wow_encoded);
            CHECK(unicode_text_calls == texts + 2 && task_enters == enters);
            class_ansi_text = FALSE;
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
    BOOL previous_guest;
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
    CHECK(SendMessageA(hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    unchanged = wow_user_set_procedure(hwnd, FALSE, 0x92340056, TRUE, &previous_guest);
    CHECK(unchanged == (DWORD)ansi_view && !previous_guest);
    unchanged = wow_user_set_procedure(hwnd, FALSE, (DWORD)ansi_view, FALSE, &previous_guest);
    CHECK(unchanged == 0x92340056 && previous_guest);
    CHECK(!wow_window_dispatch_bound(hwnd, WM_USER, 0, 0,
        0xffff1234, callback, &result));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && result == 0x12345678);
    CHECK(calls == 0);
    wow_window_words_detach(hwnd);
    CHECK(!wow_user_set_procedure(hwnd, FALSE, 0x92340056, TRUE, &previous_guest));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    CHECK(!wow_user_set_procedure(hwnd, TRUE, 0x92340056, TRUE, &previous_guest));
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
/* The source class may retire after a successful WND assignment. Original
 * xxxSetWindowData stores the resolved address/encoding, not the source CPD.
 * Two independent destination classes also prevent a shared carrier from
 * accidentally making destination B depend on destination A's lifetime.
 * These native boundary fixtures do not prove guest execution or scheduling. */
static unsigned transferred_text_calls;
static LRESULT CALLBACK transferred_ansi(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_SETTEXT) {
        CHECK(lstrcmpA((LPCSTR)lp, "transferred target") == 0);
        ++transferred_text_calls;
        return 91;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK transferred_unicode(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_SETTEXT) {
        CHECK(lstrcmpW((LPCWSTR)lp, L"transferred target") == 0);
        ++transferred_text_calls;
        return 92;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
static void transferred_callproc_contract(BOOL unicode_source)
{
    WNDCLASSEXW source_class = {sizeof(source_class)};
    WNDCLASSEXA destination_class = {sizeof(destination_class)};
    const char *names[2] = {"S3_TRANSFER_DEST_A", "S3_TRANSFER_DEST_B"};
    HWND source = NULL, destination[2] = {0};
    BOOL destination_registered[2] = {FALSE, FALSE};
    WW words = {0};
    wow_window_dispatch_target target = {0x92340056, callback, TRUE};
    WNDPROC token;
    LONG previous;
    BOOL previous_guest;
    unsigned i, before = transferred_text_calls, live_errors;
    source_class.lpfnWndProc = unicode_source ? transferred_unicode : transferred_ansi;
    source_class.hInstance = GetModuleHandleA(NULL);
    source_class.lpszClassName = L"S3_TRANSFER_SOURCE";
    /* Use the same wide class registration shape; the ANSI variant must be
     * registered through A, otherwise the test itself lies about encoding. */
    if (unicode_source) {
        CHECK(RegisterClassExW(&source_class));
        source = CreateWindowExW(0, source_class.lpszClassName, L"", 0,
            0, 0, 16, 16, NULL, NULL, source_class.hInstance, NULL);
    } else {
        WNDCLASSEXA ansi_class = {sizeof(ansi_class)};
        ansi_class.lpfnWndProc = transferred_ansi;
        ansi_class.hInstance = source_class.hInstance;
        ansi_class.lpszClassName = "S3_TRANSFER_SOURCE";
        CHECK(RegisterClassExA(&ansi_class));
        source = CreateWindowExA(0, ansi_class.lpszClassName, "", 0,
            0, 0, 16, 16, NULL, NULL, ansi_class.hInstance, NULL);
    }
    CHECK(source != NULL);
    if (!source) goto cleanup;
    token = (WNDPROC)(unicode_source ? GetWindowLongA(source, GWL_WNDPROC) :
        GetWindowLongW(source, GWL_WNDPROC));
    CHECK(token != NULL && token != source_class.lpfnWndProc);
    words.vpfnWndProc = 0x11112222;
    destination_class.lpfnWndProc = native;
    destination_class.hInstance = source_class.hInstance;
    for (i = 0; i != 2; ++i) {
        destination_class.lpszClassName = names[i];
        destination_registered[i] = RegisterClassExA(&destination_class) != 0;
        CHECK(destination_registered[i]);
        if (!destination_registered[i]) goto cleanup;
        destination[i] = CreateWindowExA(0, names[i], "", 0,
            0, 0, 16, 16, NULL, NULL, destination_class.hInstance, NULL);
        CHECK(destination[i] != NULL);
        if (!destination[i]) goto cleanup;
        CHECK(wow_window_words_attach_target(destination[i], &words, NULL, &target));
        CHECK(SetWindowLongA(destination[i], GWL_WNDPROC,
            (LONG)wow_window_entry_for_guest(&target)) != 0);
        previous = (LONG)wow_user_set_procedure(destination[i], FALSE, (DWORD)token,
            FALSE, &previous_guest);
        CHECK((DWORD)previous == target.procedure && previous_guest);
        if ((DWORD)previous != target.procedure) goto cleanup;
    }
    /* Separate encoding from source lifetime. A W-query token for an ANSI
     * procedure is not automatically an A-query token merely because our
     * forwarding gateway calls CallWindowProcA. Original SetWindowData
     * consumes CPD.wType and must preserve both entry encodings. Keep the
     * source alive for this first set of assertions. */
    live_errors = errors;
    for (i = 0; i != 2; ++i) {
        CHECK(SendMessageA(destination[i], WM_SETTEXT, 0,
            (LPARAM)"transferred target\0\0") == (unicode_source ? 92 : 91));
        CHECK(SendMessageW(destination[i], WM_SETTEXT, 0,
            (LPARAM)L"transferred target") == (unicode_source ? 92 : 91));
    }
    CHECK(transferred_text_calls == before + 4);
    fprintf(stderr, "WOW_TRANSFERRED_CALLPROC_LIVE unicode=%u text_calls=%u errors=%u\n",
        unicode_source, transferred_text_calls - before, errors - live_errors);
    before = transferred_text_calls;
    CHECK(DestroyWindow(source));
    source = NULL;
    CHECK(UnregisterClassW(source_class.lpszClassName, source_class.hInstance));
    for (i = 0; i != 2; ++i) {
        CHECK(SendMessageA(destination[i], WM_SETTEXT, 0,
            (LPARAM)"transferred target") == (unicode_source ? 92 : 91));
        CHECK(SendMessageW(destination[i], WM_SETTEXT, 0,
            (LPARAM)L"transferred target") == (unicode_source ? 92 : 91));
        CHECK(DestroyWindow(destination[i]));
        destination[i] = NULL;
        CHECK(UnregisterClassA(names[i], destination_class.hInstance));
        destination_registered[i] = FALSE;
    }
    CHECK(transferred_text_calls == before + 4);
    fprintf(stderr, "WOW_TRANSFERRED_CALLPROC unicode=%u source_retired=1 independent_destinations=2 text_calls=%u errors=%u\n",
        unicode_source, transferred_text_calls - before, errors);
    return;
cleanup:
    for (i = 0; i != 2; ++i) {
        if (destination[i]) DestroyWindow(destination[i]);
        if (destination_registered[i]) UnregisterClassA(names[i], source_class.hInstance);
    }
    if (source) DestroyWindow(source);
    UnregisterClassW(source_class.lpszClassName, source_class.hInstance);
}
int __cdecl main(void)
{
    HWND windows[2];
    WW words = {0};
    wow_window_dispatch_target target = {0x92340056, callback, TRUE};
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread runtime_thread = {0};
    wow_class_lookup_context classes = {0};
    wow_class_client_context client = {0};
    wow_task_order_process process = {0};
    wow_task_order_thread thread = {0};
    static wow_user_object_table objects;
    WNDPROC saved_entry;
    HHOOK terminal;
    BOOL previous_guest;
    DWORD previous, value;
    LONG host_top;
    unsigned i;
    /* A real runtime carrier with no TDB: scope behavior remains the actual
     * inactive-task branch. Scheduling/guest instructions have separate
     * CCPU coverage. No ownership or dispatch function is success-stubbed. */
    client.classes = &classes; client.callback = callback; client.thread = &thread;
    process.classes = &classes; process.objects = &objects; thread.ppi = &process;
    CHECK(wow_user_runtime_bind(&runtime_thread, &runtime, &thread, &client));
    terminal = SetWindowsHookExA(WH_CALLWNDPROCRET, wow_window_terminal_observer,
        NULL, GetCurrentThreadId());
    CHECK(terminal != NULL);
    if (!terminal || !runtime_thread.runtime) return 3;
    native_callproc_contract();
    words.vpfnWndProc = 0x11112222;
    saved_entry = wow_window_entry_for_guest(&target);
    CHECK(saved_entry != NULL);
    for (i = 0; i != 2; ++i) {
        windows[i] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
            NULL, NULL, GetModuleHandleA(NULL), NULL);
        CHECK(windows[i] != NULL);
        CHECK(wow_window_words_attach_target(windows[i], &words, NULL, &target));
        CHECK(SetWindowLongA(windows[i], GWL_WNDPROC, (LONG)saved_entry) != 0);
    }
    CHECK(wow_window_procedure_read(windows[0], &value) && value == 0x92340056);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)saved_entry);
    CHECK(SetWindowSubclass(windows[0], overlay, 1, 0));
    host_top = GetWindowLongA(windows[0], GWL_WNDPROC);
    CHECK(host_top != (LONG)saved_entry);
    CHECK(wow_user_get_procedure(windows[0], FALSE, &previous_guest) == (DWORD)host_top);
    CHECK(!previous_guest);
    previous = wow_user_set_procedure(windows[0], FALSE, 0x92300056, TRUE, &previous_guest);
    CHECK(previous == (DWORD)host_top && !previous_guest);
    overlay_previous = (WNDPROC)previous;
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) != host_top);
    CHECK((DWORD)wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0x92300056);
    forward_overlay = TRUE;
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == 0x92300056);
    CHECK((DWORD)SendMessageA(windows[1], WM_USER, 0, 0) == 0x92340056);
    CHECK(calls == 3 && overlay_calls == 1);
    CHECK((DWORD)CallWindowProcA(saved_entry, windows[0], WM_USER, 0, 0) == target.procedure);
    CHECK(calls == 4); /* Saved entry did not select the newest HWND target. */
    forward_overlay = FALSE;
    previous = wow_user_set_procedure(windows[0], FALSE, (DWORD)native, FALSE, &previous_guest);
    CHECK(previous == 0x92300056 && previous_guest);
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == (LONG)native);
    CHECK(SendMessageA(windows[0], WM_USER, 0, 0) == 42);
    CHECK(!wow_user_set_procedure(windows[0], FALSE, 0, TRUE, &previous_guest));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)native);
    previous = wow_user_set_procedure(windows[0], FALSE, target.procedure, TRUE, &previous_guest);
    CHECK(previous == (DWORD)native && !previous_guest);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == target.procedure);
    CHECK(calls == 5);
    previous = wow_user_set_procedure(windows[0], FALSE, (DWORD)host_top, FALSE, &previous_guest);
    CHECK(previous == target.procedure && previous_guest);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == target.procedure);
    CHECK(calls == 6 && overlay_calls == 2);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == host_top);
    for (i = 0; i != 2; ++i) CHECK(DestroyWindow(windows[i]));
    CHECK(!wow_window_procedure_read(windows[0], &value));
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    windows[0] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(windows[0] != NULL);
    CHECK(SetWindowLongA(windows[0], GWL_WNDPROC, (LONG)saved_entry) != 0);
    CHECK(CallWindowProcA(saved_entry, windows[0], WM_CREATE, 0, 0) == -1);
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_window_words_attach(windows[0], &words));
    CHECK((DWORD)CallWindowProcA(saved_entry, windows[0], WM_USER, 0, 0) == target.procedure);
    previous = wow_user_set_procedure(windows[0], FALSE, (DWORD)native, FALSE, &previous_guest);
    CHECK(previous == target.procedure && previous_guest);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)native);
    CHECK(DestroyWindow(windows[0]));
    CHECK(!wow_window_words_acquire(windows[0])); /* Native-only destruction still retires. */
    CHECK(task_enters && task_enters == task_leaves);
    transferred_callproc_contract(FALSE);
    transferred_callproc_contract(TRUE);
    CHECK(UnhookWindowsHookEx(terminal));
    CHECK(wow_user_runtime_unbind(&runtime_thread));
    fprintf(stderr, "WOW_WINDOW_PROCEDURE errors=%u tagged_calls=%u\n", errors, calls);
    fprintf(stderr, "WOW_NATIVE_SUBCLASS errors=%u forwarded=%u preserved=1 missing_owner_rejected=1\n", errors, overlay_calls);
    return errors ? 3 : 0;
}
