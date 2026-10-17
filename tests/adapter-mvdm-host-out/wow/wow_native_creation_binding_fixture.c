#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_client_view_layout.h"
#include "wow_window_client_state_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_class_client_bindings.h"
#include "wow_user_private_access.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include <stdio.h>
#include <string.h>
static unsigned errors, callbacks, nested, rejected, destroyed, vetoed;
static wow_class_lookup_context context;
static ATOM atom;
static HINSTANCE instance;
static const char name[] = "S40_NATIVE_CREATE_BINDING";
static DWORD class_words[2] = {0x12340056, 0x11223344};
static HHOOK veto_hook;
static BOOL veto;
static unsigned mdi_created;
static wow_window_words_binding *rejected_borrow, *vetoed_borrow;
static DWORD WINAPI raising_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    UNREFERENCED_PARAMETER(window); UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wp); UNREFERENCED_PARAMETER(lp);
    UNREFERENCED_PARAMETER(procedure);
    if (wow_user_borrow_window(window) != words) ++errors;
    RaiseException(0xE0420040, 0, 0, NULL);
    return 0;
}
static DWORD WINAPI callback(HWND, UINT, UINT, LONG, DWORD, PVOID);
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static void verify_gateway_failure_cleanup(void)
{
    HWND window = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 8, 8,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    WW words = {0};
    wow_window_dispatch_target target = {0, NULL, TRUE};
    wow_window_words_binding *borrow;
    WNDPROC entry;
    CWPRETSTRUCT terminal = {0};
    BOOL caught = FALSE;
    CHECK(window != NULL); if (!window) return;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    target.procedure = (DWORD)wow_window_class_creation_proc;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    target.procedure = 0x81234567;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_words_acquire(window) == NULL);
    target.callback = raising_callback;
    entry = wow_window_entry_for_guest(&target);
    CHECK(entry != NULL);
    CHECK(CallWindowProcA(entry, window, WM_CREATE, 0, 0) == -1);
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_window_words_attach_target(window, &words, NULL, &target));
    borrow = wow_window_words_acquire(window);
    CHECK(borrow != NULL);
    /* Direct invocation keeps SEH under test control, not USER's WNDPROC
     * exception policy. The STATIC window itself is still alive afterward. */
    __try {
        CallWindowProcA(entry, window, WM_NCDESTROY, 0, 0);
    } __except (GetExceptionCode() == 0xE0420040 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught);
    /* An arbitrary saved-procedure call is not native message completion.
     * The separate terminal owner, not this callback, retires the identity. */
    CHECK(wow_window_words_cleanup_value(borrow)->storage == borrow);
    terminal.hwnd = window; terminal.message = WM_NCDESTROY;
    (void)wow_window_terminal_observer(HC_ACTION, 0, (LPARAM)&terminal);
    CHECK(wow_user_borrow_window(window) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(wow_window_words_acquire(window) == NULL);
    CHECK(IsWindow(window));
    CHECK(wow_window_words_value(borrow) != NULL);
    wow_window_words_release(borrow);
    CHECK(DestroyWindow(window));
}
static HWND create(unsigned identity)
{
    CREATESTRUCTA request = {0};
    WW words = {0};
    wow_window_dispatch_target target = {0x81234567, callback, TRUE};
    words.adwWOW[0] = identity;
    request.hInstance = instance; request.lpszClass = name; request.lpszName = "";
    request.cx = request.cy = 32; request.style = WS_OVERLAPPEDWINDOW;
    request.lpCreateParams = (LPVOID)0x12345678;
    return wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &request, &words, &target);
}
static DWORD WINAPI callback(HWND window, UINT message, UINT wp, LONG lp,
    DWORD procedure, PVOID raw_words)
{
    WW *words = raw_words;
    wow_window_words_binding *borrow = wow_window_words_acquire(window);
    DWORD *wc = wow_window_class_words_value(borrow);
    LONG result;
    ++callbacks;
    CHECK(procedure == 0x81234567 && borrow);
    CHECK(wc && wc[0] == class_words[0] && wc[1] == class_words[1]);
    CHECK(words == wow_window_words_value(borrow));
    CHECK(wow_window_words_cleanup_value(borrow)->thread==wow_fixture_creation_owner(&context));
    CHECK(fixture_creation_objects.entries[LOWORD(window)].phead==wow_window_words_cleanup_value(borrow));
    CHECK(fixture_creation_objects.entries[LOWORD(window)].wUniq==HIWORD(window));
    CHECK(words->hInstance == instance);
    if (message == WM_NCCREATE) {
        CREATESTRUCTA *cs = (CREATESTRUCTA *)lp;
        if (words->adwWOW[0] == 7) {
            PWND state = wow_window_client_validate_hwnd(window);
            /* The counted original creator must expose WND before the
             * application procedure, even for a nonstandard dialog class. */
            CHECK(state != NULL);
            if (state) {
                CHECK(state->head.h == window);
                CHECK(state->dwExpWinVer == 0x030a);
                CHECK(state->hModule == instance);
            }
        }
        if (cs->dwExStyle & WS_EX_MDICHILD) {
            MDICREATESTRUCTA *mdi = cs->lpCreateParams;
            CHECK(mdi && mdi->lParam == 0x12345678);
            ++mdi_created;
        } else CHECK(cs->lpCreateParams == (LPVOID)0x12345678);
        CHECK(GetWindowLongA(window, GWL_USERDATA) == 0);
        SetWindowLongA(window, GWL_USERDATA, words->adwWOW[0]);
        if (words->adwWOW[0] == 1) {
            HWND child = create(2);
            CHECK(child != NULL);
            if (child) { ++nested; CHECK(DestroyWindow(child)); }
            CHECK(GetWindowLongA(window, GWL_USERDATA) == 1);
        }
        if (words->adwWOW[0] == 3) {
            ++rejected;
            rejected_borrow=wow_window_words_acquire(window);
            wow_window_words_release(borrow);
            return FALSE;
        }
    }
    if (words->dwExStyle & WS_EX_MDICHILD)
        result = DefMDIChildProcA(window, message, wp, lp);
    else result = DefWindowProcA(window, message, wp, lp);
    if (message == WM_NCDESTROY) {
        ++destroyed;
        /* Product native gateway owns terminal detach after this callback. */
        CHECK(wc[1] == class_words[1]);
    }
    wow_window_words_release(borrow);
    return result;
}
static LRESULT CALLBACK reject_hook(int code, WPARAM wp, LPARAM lp)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    CHECK(runtime && !runtime->exclusive_held);
    if (code == HCBT_CREATEWND && veto &&
            (ATOM)GetClassLongA((HWND)wp, GCW_ATOM) == atom) {
        ++vetoed;
        vetoed_borrow=wow_window_words_acquire((HWND)wp);
        CHECK(vetoed_borrow!=NULL);
        CHECK(wow_window_words_cleanup_value(vetoed_borrow)->thread==wow_fixture_creation_owner(&context));
        return 1;
    }
    return CallNextHookEx(veto_hook, code, wp, lp);
}
#ifdef WOW_CREATION_UNWIND_FIXTURE
VOID WINAPI wow_fixture_creation_unwind_arm(void);
BOOL WINAPI wow_fixture_creation_unwind_complete(void);
static void verify_creation_unwind(void)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    BOOL caught = FALSE;
    HWND window;
    CHECK(runtime && !runtime->exclusive_held);
    wow_fixture_creation_unwind_arm();
    __try {
        window = create(8);
        if (window) DestroyWindow(window);
        CHECK(FALSE); /* The fault must reach the outer native-create finally. */
    } __except (GetExceptionCode() == (DWORD)0xe0424474 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && GetLastError() == 7447);
    CHECK(!runtime->exclusive_held);
    CHECK(wow_fixture_creation_unwind_complete());
    window = create(9);
    CHECK(window != NULL);
    if (window) CHECK(DestroyWindow(window));
    CHECK(!runtime->exclusive_held && wow_fixture_creation_unwind_complete());
    fprintf(stderr, "WOW_CREATION_UNWIND errors=%u tls=1 hooks=0 scope_release=1 restored=1 retry=1\n", errors);
}
#endif
static void verify_creation_encoding(HWND window, BOOL expected)
{
    wow_window_words_binding *borrow = wow_window_words_acquire(window);
    BOOL ansi = !expected;
    CHECK(borrow != NULL);
    if (!borrow) return;
    SetLastError(0x1234);
    CHECK(wow_window_words_creation_ansi(borrow, &ansi));
    CHECK(ansi == expected && GetLastError() == 0x1234);
    CHECK(wow_window_words_set_creation_ansi(borrow, expected));
    CHECK(!wow_window_words_set_creation_ansi(borrow, !expected));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(wow_window_words_creation_ansi(borrow, &ansi) && ansi == expected);
    wow_window_words_release(borrow);
}

static void verify_mdi_creation(void)
{
    HWND frame, client, child;
    HHOOK observer;
    MDICREATESTRUCTA implicit = {0};
    CLIENTCREATESTRUCT mdi = {NULL, 100};
    CREATESTRUCTA request = {0};
    CREATESTRUCTW wide;
    WW words = {0};
    wow_window_dispatch_target target = {0x81234567, callback, TRUE};
    frame = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
        0, 0, 80, 80, NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(frame != NULL); if (!frame) return;
    client = CreateWindowExA(0, "MDICLIENT", "", WS_CHILD | MDIS_ALLCHILDSTYLES,
        0, 0, 64, 64, frame, NULL, GetModuleHandleA(NULL), &mdi);
    CHECK(client != NULL); if (!client) { DestroyWindow(frame); return; }
    request.dwExStyle = WS_EX_MDICHILD;
    request.style = WS_CHILD | WS_OVERLAPPEDWINDOW;
    request.hwndParent = client; request.hInstance = instance;
    request.lpszClass = name; request.lpszName = "";
    request.cx = request.cy = 24;
    request.lpCreateParams = (PVOID)0x12345678;
    words.adwWOW[0] = 6;
    child = wow_window_create_boundA(wow_fixture_creation_owner(&context), atom, &request, &words, &target);
    CHECK(child != NULL);
    if (child) verify_creation_encoding(child, TRUE);
    if (child) { CHECK(GetParent(child) == client); CHECK(DestroyWindow(child)); }
    CopyMemory(&wide, &request, sizeof(wide));
    wide.lpszClass = L"S40_NATIVE_CREATE_BINDING"; wide.lpszName = L"";
    child = wow_window_create_boundW(wow_fixture_creation_owner(&context), atom, &wide, &words, &target);
    CHECK(child != NULL);
    if (child) verify_creation_encoding(child, FALSE);
    if (child) { CHECK(GetParent(child) == client); CHECK(DestroyWindow(child)); }
    observer = SetWindowsHookExA(WH_CBT, wow_window_creation_observer, NULL,
        GetCurrentThreadId());
    CHECK(observer != NULL);
    if (observer) {
        implicit.szClass = name;
        implicit.szTitle = "";
        implicit.hOwner = instance;
        implicit.cx = implicit.cy = 24;
        implicit.style = WS_CHILD | WS_OVERLAPPEDWINDOW;
        implicit.lParam = 0x12345678;
        /* USER creates this HWND, not wow_window_create_boundA/W. */
        child = (HWND)SendMessageA(client, WM_MDICREATE, 0, (LPARAM)&implicit);
        CHECK(child != NULL);
        if (child) {
            CHECK(GetParent(child) == client);
            CHECK(GetWindowLongA(child, GWL_USERDATA) == 0);
            CHECK(DestroyWindow(child));
        }
        CHECK(UnhookWindowsHookEx(observer));
    }
    CHECK(mdi_created == 3);
    CHECK(DestroyWindow(frame));
}
static void verify_counted_dialog_creation(void)
{
    WCHAR class_text[] = L"S40_NATIVE_CREATE_BINDING";
    /* Original ssend.c LARGECOPYSTRINGLPWSTRA captures Length/2 + 1
     * characters, including the terminator supplied by RtlInitLargeUnicodeString.
     * Trailing capacity may contain data, but the terminator is not optional. */
    WCHAR title_text[] = {'A', 'B', 0, 'X', 'Y', 0};
    WCHAR actual[8];
    LARGE_STRING class_name = {0}, title = {0};
    DWORD words[3] = {7, 0x12345678, 0x76543210};
    DWORD version = 0x1234030a;
    DWORD saved_compat = fixture_creation_thread.dwCompatFlags;
    unsigned i;
    class_name.Buffer = class_text;
    class_name.Length = sizeof(class_text) - sizeof(WCHAR);
    class_name.MaximumLength = sizeof(class_text);
    title.Buffer = title_text;
    title.Length = 2 * sizeof(WCHAR);
    title.MaximumLength = sizeof(title_text);
    fixture_creation_thread.dwCompatFlags = 0x1234;
    /* Exercise the real enrollment, not just the native leaf. Both class
     * forms use counted Unicode text with independent creator encoding.
     * No get_version callback is installed: this original entry already
     * receives the captured version and must not query it a second time. */
    for (i = 0; i != 4; ++i) {
        HWND window = wow_user_create_dialog_window_ex(
            (i & 1) ? WS_EX_ANSICREATOR : 0,
            (i & 2) ? (PLARGE_STRING)(ULONG_PTR)atom : &class_name,
            &title, WS_OVERLAPPEDWINDOW, 0, 0, 32, 32, NULL, NULL,
            instance, (LPVOID)0x12345678, version, words);
        wow_window_words_binding *borrow;
        wow_window_creation_facts facts = {0};
        CHECK(window != NULL);
        if (!window) continue;
        verify_creation_encoding(window, (i & 1) != 0);
        ZeroMemory(actual, sizeof(actual));
        {
            int length = GetWindowTextW(window, actual, ARRAYSIZE(actual));
            fprintf(stderr, "WOW_COUNTED_TITLE case=%u length=%d text=%04x,%04x,%04x,%04x unicode=%d\n",
                i, length, actual[0], actual[1], actual[2], actual[3], IsWindowUnicode(window));
            CHECK(length == 2);
        }
        CHECK(actual[0] == L'A' && actual[1] == L'B' && actual[2] == 0);
        borrow = wow_window_words_acquire(window);
        CHECK(borrow != NULL);
        if (borrow) {
            PWND state = wow_window_words_client_state_value(borrow);
            ULONG backing = mvdm_softpc_wow_page_domain_borrow_window((ULONG)window);
            CHECK(backing != 0);
            if (backing)
                CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) == borrow);
            CHECK(wow_window_words_creation_facts(borrow, &facts));
            CHECK(facts.module_version == version);
            CHECK(facts.compatibility_flags == 0x1234);
            CHECK(!memcmp(wow_window_words_value(borrow)->adwWOW,
                words, sizeof(words)));
            CHECK(state != NULL);
            if (state) {
                CHECK(state->head.h == window && state->dwExpWinVer == LOWORD(version));
                CHECK(!!TestWF(state, WFANSICREATOR) == ((i & 1) != 0));
            }
            CHECK(DestroyWindow(window));
            CHECK(wow_window_words_cleanup_value(borrow)->thread == NULL);
            CHECK(wow_window_words_acquire(window) == NULL);
            if (backing) {
                /* Retirement removes identity, not storage still borrowed by
                 * an original control's retained relationship generation. */
                CHECK(mvdm_softpc_wow_page_domain_window_host_storage(backing) == borrow);
                CHECK(wow_window_words_client_state_value(borrow) == state);
                CHECK(mvdm_softpc_wow_page_domain_release_window(backing));
            }
            wow_window_words_release(borrow);
        } else CHECK(DestroyWindow(window));
    }
    fixture_creation_thread.dwCompatFlags = saved_compat;
}

static unsigned control_version_calls;
static HANDLE control_version_module;
static BOOL control_version_raise;
static DWORD WINAPI control_module_version(HANDLE module)
{
    CHECK(module == control_version_module);
    ++control_version_calls;
    if (control_version_raise)
        RaiseException(0xe0424473u, 0, 0, NULL);
    return 0x1234030a;
}

static void verify_control_creation_inputs(void)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_class_client_context client = {0}, *previous;
    unsigned i;
    CHECK(runtime != NULL);
    if (!runtime) return;
    previous = runtime->classes;
    client.classes = &context;
    client.thread = &fixture_creation_thread;
    client.get_version = control_module_version;
    runtime->classes = &client;
    __try {
        control_version_calls = 0;
        control_version_module = instance;
        control_version_raise = FALSE;
        for (i = 0; i != 2; ++i) {
            HWND window = wow_control_create_window_ex(0, MAKEINTATOM(atom),
                NULL, WS_OVERLAPPEDWINDOW, 0, 0, 32, 32, NULL, NULL,
                instance, (LPVOID)0x12345678, i ? CW_FLAGS_ANSI : 0, NULL);
            wow_window_words_binding *borrow;
            wow_window_creation_facts facts = {0};
            CHECK(window != NULL);
            CHECK(control_version_calls == i + 1);
            if (!window) continue;
            verify_creation_encoding(window, i != 0);
            borrow = wow_window_words_acquire(window);
            CHECK(borrow != NULL);
            if (borrow) {
                WW *words = wow_window_words_value(borrow);
                CHECK(wow_window_words_creation_facts(borrow, &facts));
                CHECK(facts.module_version == 0x030a);
                CHECK(words->adwWOW[0] == 0 && words->adwWOW[1] == 0 &&
                    words->adwWOW[2] == 0);
                CHECK(DestroyWindow(window));
                CHECK(wow_window_words_cleanup_value(borrow)->thread == NULL);
                wow_window_words_release(borrow);
            } else CHECK(DestroyWindow(window));
        }
        /* Query-only fault boundary: a null module must resolve to the
         * current executable before any native creation/enrollment. The
         * fixture class uses a synthetic module, so this does not claim
         * native null-module class resolution or creation acceptance. */
        control_version_module = GetModuleHandleW(NULL);
        control_version_raise = TRUE;
        {
            BOOL caught = FALSE;
            __try {
                (void)wow_control_create_window_ex(0, MAKEINTATOM(atom), NULL,
                    0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, 0, NULL);
            } __except (GetExceptionCode() == 0xe0424473u ?
                    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
                caught = TRUE;
            }
            CHECK(caught);
            CHECK(control_version_calls == 3);
            CHECK(wow_user_runtime_current() == runtime);
            CHECK(runtime->classes == &client);
        }
        control_version_raise = FALSE;
        control_version_module = instance;
        CHECK(!wow_control_create_window_ex(WS_EX_MDICHILD, MAKEINTATOM(atom),
            NULL, 0, 0, 0, 0, 0, NULL, NULL, instance, NULL, 0, NULL));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(!wow_control_create_window_ex(0, MAKEINTATOM(atom), TEXT("title"),
            0, 0, 0, 0, 0, NULL, NULL, instance, NULL, 0, NULL));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(!wow_control_create_window_ex(0, TEXT("S40_NATIVE_CREATE_BINDING"),
            NULL, 0, 0, 0, 0, 0, NULL, NULL, instance, NULL, 0, NULL));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        CHECK(!wow_control_create_window_ex(0, MAKEINTATOM(atom), NULL,
            0, 0, 0, 0, 0, NULL, NULL, instance, NULL, ~CW_FLAGS_ANSI, NULL));
        CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        {
            DWORD words[3] = {0};
            CHECK(!wow_control_create_window_ex(0, MAKEINTATOM(atom), NULL,
                0, 0, 0, 0, 0, NULL, NULL, instance, NULL, 0, words));
            CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
        }
        client.get_version = NULL;
        CHECK(!wow_control_create_window_ex(0, MAKEINTATOM(atom), NULL,
            0, 0, 0, 0, 0, NULL, NULL, instance, NULL, 0, NULL));
        CHECK(GetLastError() == ERROR_INVALID_STATE);
        CHECK(control_version_calls == 3);
    } __finally {
        control_version_raise = FALSE;
        runtime->classes = previous;
    }
}

static void verify_dialog_enrollment_target(void)
{
    WNDCLASSEXA native = {sizeof(native)};
    UNICODE_STRING captured;
    HINSTANCE module = GetModuleHandleA(NULL);
    HANDLE saved_module = context.client_module;
    HWND parent = NULL, child = NULL;
    WW words = {0};
    CREATESTRUCTA request = {0};
    wow_window_creation_enrollment enrollment = {0};
    DWORD procedure = 0;
    DWORD native_procedure = 0;
    DWORD native_restored = 0;
    BOOL imported = FALSE, attached = FALSE;
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    unsigned held;

    /* Test enrollment/rollback only. Do not invoke the staged original
     * dialog manager or pretend this native witness runs guest code. */
    context.client_module = module;
    CHECK(GetClassInfoExA(NULL, MAKEINTATOM(32770), &native));
    if (!native.lpfnWndProc) goto done;
    RtlInitUnicodeString(&captured, L"#32770");
    imported = wow_class_words_import_native(&context, &native, &captured,
        32770, FNID_DIALOG, NULL);
    CHECK(imported);
    if (!imported) goto done;
    parent = CreateWindowExA(0, "STATIC", "", WS_POPUP,
        0, 0, 16, 16, NULL, NULL, module, NULL);
    CHECK(parent != NULL);
    if (!parent) goto done;
    attached = wow_window_words_attach(parent, &words);
    CHECK(attached);
    if (!attached) goto done;
    child = CreateWindowExA(0, MAKEINTATOM(32770), "", WS_CHILD,
        0, 0, 8, 8, parent, NULL, module, NULL);
    CHECK(child != NULL);
    if (!child) goto done;
    native_procedure = (DWORD)GetWindowLongA(child, GWL_WNDPROC);
    CHECK(native_procedure != 0);
    {
        /* Native-only negative control: no project enrollment or metadata
         * exists on this child yet. Observe USER's own get/set round trip
         * before attributing address normalization to our rollback. */
        DWORD previous = (DWORD)SetWindowLongA(child, GWL_WNDPROC, (LONG)DefWindowProcA);
        DWORD restored;
        CHECK(previous != 0);
        CHECK(SetWindowLongA(child, GWL_WNDPROC, (LONG)previous) != 0);
        restored = (DWORD)GetWindowLongA(child, GWL_WNDPROC);
        native_restored = restored;
        CHECK(native_restored == previous);
        fprintf(stderr, "WOW_NATIVE_DIALOG_ROUNDTRIP initial=%08lx previous=%08lx restored=%08lx defdlg=%p class=%p\n",
            native_procedure, previous, restored, DefDlgProcA, native.lpfnWndProc);
        CHECK(DestroyWindow(child));
        child = CreateWindowExA(0, MAKEINTATOM(32770), "", WS_CHILD,
            0, 0, 8, 8, parent, NULL, module, NULL);
        CHECK(child != NULL);
        if (!child) goto done;
        native_procedure = (DWORD)GetWindowLongA(child, GWL_WNDPROC);
    }
    request.hwndParent = parent;
    request.hInstance = module;
    request.style = WS_CHILD;
    for (held = 0; held < 2; ++held) {
        DWORD before = (DWORD)GetWindowLongA(child, GWL_WNDPROC);
        BOOL unicode_before = IsWindowUnicode(child);
        CHECK(runtime && !runtime->exclusive_held);
        if (held) CHECK(wow_user_runtime_enter(runtime));
        CHECK(wow_window_creation_enroll_internal(wow_fixture_creation_owner(&context),
            child, &request, &enrollment));
        CHECK(runtime->exclusive_held == (BOOL)held);
        CHECK(enrollment.borrow != NULL);
        if (enrollment.borrow) {
            CHECK(wow_window_procedure_read(child, &procedure));
            fprintf(stderr, "WOW_IMPLICIT_DIALOG_TARGET held=%u initial=%08lx before=%08lx target=%08lx previous=%p unicode=%d\n",
                held, native_procedure, before, procedure, enrollment.previous_native, unicode_before);
            /* Native implicit creation must not gain an original PDLG
             * owner just because its parent has a WOW identity. */
            CHECK(procedure == before);
            CHECK(enrollment.previous_native == NULL);
            CHECK((DWORD)enrollment.installed_native == before);
        }
        /* This native-owned child was not subclassed. Unlike the independent
         * native round-trip control above, rollback must not normalize its
         * untouched procedure or change its actual encoding. */
        wow_window_creation_finish(&enrollment, FALSE);
        CHECK(runtime->exclusive_held == (BOOL)held);
        CHECK(GetPropW(child, L"NTVDM.WOW.WindowWords") == NULL);
        fprintf(stderr, "WOW_IMPLICIT_DIALOG_ROLLBACK held=%u after=%08lx unicode=%d\n",
            held, (DWORD)GetWindowLongA(child, GWL_WNDPROC), IsWindowUnicode(child));
        CHECK((DWORD)GetWindowLongA(child, GWL_WNDPROC) == before);
        CHECK(IsWindowUnicode(child) == unicode_before);
        if (held) CHECK(wow_user_runtime_leave(runtime));
        {
            WCHAR title[32] = {0};
            CHECK(SetWindowTextA(child, "native rollback"));
            CHECK(GetWindowTextW(child, title, ARRAYSIZE(title)) == 15);
            CHECK(lstrcmpW(title, L"native rollback") == 0);
        }
    }
done:
    if (enrollment.borrow) wow_window_creation_finish(&enrollment, FALSE);
    if (child) CHECK(DestroyWindow(child));
    if (attached) wow_window_words_detach(parent);
    if (parent) CHECK(DestroyWindow(parent));
    if (imported) CHECK(wow_class_words_forget_native(&context));
    context.client_module = saved_module;
}
static void verify_sampled_generation_reuse(void)
{
    HWND window = CreateWindowExA(0, "STATIC", "generation probe", 0,
        0, 0, 8, 8, NULL, NULL, GetModuleHandleA(NULL), NULL);
    ULONG first_words[3] = {0x11112222, 0, 0};
    ULONG next_words[3] = {0x33334444, 0, 0};
    WW *first, *next;
    wow_window_words_binding *first_binding, *related;
    wow_user_borrow_scope outer, inner;
    BOOL published = FALSE, entered = FALSE;
    CHECK(window != NULL);
    if (!window) return;
    /* Deterministic page-domain retirement/republication of one real HWND.
     * This models handle reuse at the cache boundary, not Windows allocator
     * reuse or guest execution. Both generations use the real CCPU owner. */
    published = mvdm_softpc_wow_page_domain_publish_handle(LOWORD(window),
        HIWORD(window), TYPE_WINDOW, 0, (ULONG)window, 0, first_words);
    CHECK(published);
    if (!published) goto done;
    entered = wow_user_borrow_enter(&outer);
    CHECK(entered);
    if (!entered) goto done;
    first = wow_user_borrow_window(window);
    CHECK(first && first->adwWOW[0] == first_words[0]);
    CHECK(wow_user_borrow_window(window) == first);
    first_binding = wow_user_borrow_observed_binding(window);
    related = first_binding;
    CHECK(first_binding && wow_user_borrow_window_relation(first_binding, 2, &related));
    CHECK(related == NULL); /* Empty child is not a failed lookup. */
    related = first_binding;
    CHECK(!wow_user_borrow_window_relation(first_binding, 4, &related));
    CHECK(related == first_binding); /* Invalid edge leaves output intact. */
    if (!mvdm_softpc_wow_page_domain_retire_handle(LOWORD(window), NULL)) {
        CHECK(FALSE); goto done;
    }
    published = FALSE;
    published = mvdm_softpc_wow_page_domain_publish_handle(LOWORD(window),
        HIWORD(window), TYPE_WINDOW, 0, (ULONG)window, 0, next_words);
    CHECK(published);
    if (!published) goto done;
    if (wow_user_borrow_enter(&inner)) {
        next = wow_user_borrow_window(window);
        CHECK(next && next != first && next->adwWOW[0] == next_words[0]);
        CHECK(wow_window_words_value(wow_user_borrow_observed_binding(window)) == next);
        CHECK(first && first->adwWOW[0] == first_words[0]);
        CHECK(wow_user_borrow_leave(&inner));
    } else CHECK(FALSE);
    /* Leaving the inner scope does not make the older outer alias current. */
    next = wow_user_borrow_window(window);
    CHECK(next && next != first && next->adwWOW[0] == next_words[0]);
    CHECK(wow_window_words_value(wow_user_borrow_observed_binding(window)) == next);
    CHECK(first && first->adwWOW[0] == first_words[0]);
    related = first_binding;
    CHECK(wow_user_borrow_window_relation(first_binding, 2, &related) && !related);
done:
    if (published)
        CHECK(mvdm_softpc_wow_page_domain_retire_handle(LOWORD(window), NULL));
    if (entered) CHECK(wow_user_borrow_leave(&outer));
    CHECK(DestroyWindow(window));
}

static DWORD collision_target, collision_calls;
static DWORD WINAPI collision_version(HANDLE module)
{ (void)module; return 0x030a; }
static DWORD WINAPI collision_callback(HWND w, UINT m, UINT p, LONG l, DWORD proc, PVOID words)
{
    CHECK(proc == collision_target && words != NULL);
    ++collision_calls;
    if(m == WM_APP) return proc;
    return (DWORD)DefWindowProcA(w,m,p,l);
}
static LRESULT CALLBACK collision_native(HWND w, UINT m, WPARAM p, LPARAM l)
{
    (void)w; (void)p; (void)l;
    return m == WM_NCCREATE ? TRUE : m == WM_APP ? 0x2345 : 0;
}
static void verify_live_cpd_guest_origin(wow_class_client_context *client, DWORD token)
{
    WNDCLASSA request = {0};
    CREATESTRUCTA creation = {0};
    wow_class_words_binding *binding;
    wow_window_dispatch_target target = {0};
    WW window_words = {0};
    DWORD words[2] = {token & ~WOW_WINDOW_PROC_TAG, 0}, previous;
    BOOL previous_guest;
    HWND window;
    ATOM atom;
    unsigned before;
    DWORD saved_target = collision_target;
    if (!(words[0] & WOW_WINDOW_SELECTOR_VIRTUAL_BIT))
        words[0] |= WOW_WINDOW_PROC_TAG | WOW_WINDOW_SELECTOR_VIRTUAL_BIT;
    request.hInstance = instance; request.lpfnWndProc = (WNDPROC)token;
    request.lpszClassName = "S3_LIVE_CPD_GUEST_ORIGIN";
    /* Exactly the same numeric word is a known guest registration first,
     * then a real native CPD assignment. No bit-based intent inference. */
    atom = wow_class_client_register_guest(client, &request, words);
    CHECK(atom != 0); if (!atom) return;
    binding = wow_class_words_acquire(&context, atom, instance);
    CHECK(binding && wow_class_words_target(binding, &target));
    CHECK(target.procedure == token && target.wow_encoded);
    collision_target = token;
    creation.hInstance = instance; creation.lpszClass = request.lpszClassName;
    creation.lpszName = ""; creation.style = WS_POPUP; creation.cx = creation.cy = 8;
    window = wow_window_create_boundA(wow_fixture_creation_owner(&context),
        atom, &creation, &window_words, NULL);
    CHECK(window != NULL);
    if (window) {
        CHECK((DWORD)SendMessageA(window, WM_APP, 0, 0) == token);
        previous = wow_user_set_procedure(window, FALSE, token, FALSE, &previous_guest);
        CHECK(previous == token && previous_guest);
        before = collision_calls;
        CHECK(SendMessageA(window, WM_APP, 0, 0) == 0x2345);
        CHECK(collision_calls == before);
        previous = wow_user_set_procedure(window, FALSE, token, TRUE, &previous_guest);
        CHECK(previous != 0 && !previous_guest);
        CHECK(wow_user_get_procedure(window, FALSE, &previous_guest) == token && previous_guest);
        CHECK((DWORD)SendMessageA(window, WM_APP, 0, 0) == token);
        CHECK(DestroyWindow(window));
    }
    wow_class_words_release(binding);
    CHECK(wow_class_client_unregister(client, request.lpszClassName, instance));
    collision_target = saved_target;
    fprintf(stderr, "WOW_CLASS_TOKEN_ORIGIN errors=%u guest_native_same_word=1 native_replaced=1 guest_restored=1\n", errors);
}
static void verify_class_token_identity(void)
{
    WNDCLASSEXW source = {sizeof(source)};
    WNDCLASSA request = {0};
    wow_class_client_context client = {0};
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_class_client_context *previous_client = runtime->classes;
    BOOL previous_guest, client_selected = FALSE;
    DWORD words[2] = {0x7fff0b98u,0}, token, resolved, previous;
    WW ww = {0};
    CREATESTRUCTA creation = {0};
    HWND source_window = NULL, window = NULL;
    ATOM source_atom, registered = 0;
    wow_class_words_binding *class_binding = NULL;
    wow_window_dispatch_target snapshot = {0};
    unsigned before;
    source.hInstance=instance; source.lpfnWndProc=collision_native;
    source.lpszClassName=L"S3_NATIVE_TOKEN_SOURCE";
    source_atom=RegisterClassExW(&source);
    CHECK(source_atom); if(!source_atom)return;
    source_window=CreateWindowExW(0,MAKEINTATOM(source_atom),L"",WS_POPUP,
        0,0,8,8,NULL,NULL,instance,NULL);
    CHECK(source_window); if(!source_window)goto done;
    token=(DWORD)GetWindowLongA(source_window,GWL_WNDPROC);
    CHECK(HIWORD(token)==0xffff);
    SetLastError(0x2468);
    CHECK(wow_native_class_resolve_procedure(token,&resolved));
    CHECK(resolved==(DWORD)collision_native && GetLastError()==0x2468);
    CHECK(wow_native_class_resolve_procedure(0xffff0b98u,&resolved));
    CHECK(resolved==0xffff0b98u);
    client.classes=&context; client.dwExpWinVer=0x030a;
    client.get_version=collision_version; client.callback=collision_callback;
    client.thread=&fixture_creation_thread;
    runtime->classes=&client; client_selected=TRUE;
    request.hInstance=instance;request.lpszClassName="S3_COLLISION_CLASS";
    request.lpfnWndProc=(WNDPROC)0xffff0b98u;
    collision_target=0xffff0b98u;
    registered=wow_class_client_register_guest(&client,&request,words);
    CHECK(registered);if(!registered)goto done;
    class_binding=wow_class_words_acquire(&context,registered,instance);
    CHECK(class_binding && wow_class_words_target(class_binding,&snapshot));
    CHECK(snapshot.procedure==collision_target && snapshot.wow_encoded);
    creation.hInstance=instance;creation.lpszClass=request.lpszClassName;
    creation.lpszName="";creation.style=WS_POPUP;creation.cx=creation.cy=8;
    window=wow_window_create_boundA(wow_fixture_creation_owner(&context),registered,&creation,&ww,NULL);
    CHECK(window);if(!window)goto done;
    CHECK((DWORD)SendMessageA(window,WM_APP,0,0)==collision_target);
    previous=wow_user_set_procedure(window,FALSE,token,FALSE,&previous_guest);
    CHECK(previous==collision_target && previous_guest);
    before=collision_calls;
    CHECK(SendMessageA(window,WM_APP,0,0)==0x2345);
    CHECK(collision_calls==before);
    previous=wow_user_set_procedure(window,FALSE,collision_target,TRUE,&previous_guest);
    CHECK(previous && !previous_guest);
    CHECK(wow_native_class_resolve_procedure(previous,&resolved) && resolved==(DWORD)collision_native);
    CHECK((DWORD)SendMessageA(window,WM_APP,0,0)==collision_target);
    CHECK(wow_class_words_exchange_procedure(class_binding,token,FALSE,&previous,&previous_guest) &&
        previous==collision_target && previous_guest);
    CHECK(wow_class_words_target(class_binding,&snapshot) && !snapshot.wow_encoded);
    CHECK(wow_class_words_exchange_procedure(class_binding,collision_target,TRUE,&previous,&previous_guest) &&
        previous==(DWORD)collision_native && !previous_guest);
    CHECK(wow_class_words_target(class_binding,&snapshot) && snapshot.wow_encoded);
done:
    if(window)CHECK(DestroyWindow(window));
    wow_class_words_release(class_binding);
    if(registered)CHECK(wow_class_client_unregister(&client,request.lpszClassName,instance));
    /* Native CPD registration exercises original normalization, not just
     * the native service directly. Keep the source owner alive throughout. */
    if(source_window){
        verify_live_cpd_guest_origin(&client, token);
        request.lpfnWndProc=(WNDPROC)token;
        registered=wow_class_client_register(&client,&request,words);
        CHECK(registered);
        if(registered){
            class_binding=wow_class_words_acquire(&context,registered,instance);
            CHECK(class_binding && wow_class_words_target(class_binding,&snapshot));
            CHECK(snapshot.procedure==(DWORD)collision_native && !snapshot.wow_encoded);
            wow_class_words_release(class_binding);
            CHECK(wow_class_client_unregister(&client,request.lpszClassName,instance));
        }
        CHECK(DestroyWindow(source_window));
    }
    if(client_selected)runtime->classes=previous_client;
    CHECK(UnregisterClassW(MAKEINTATOM(source_atom),instance));
    fprintf(stderr,"WOW_CLASS_TOKEN_IDENTITY errors=%u callbacks=%lu\n",errors,collision_calls);
}

int __cdecl main(void)
{
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_task_order_state task_state = {0};
    wow_task_order_entry task = {0};
    WNDCLASSEXA cls = {sizeof(cls)};
    wow_window_dispatch_target target = {0x81234567, callback, TRUE};
    HHOOK terminal_observer = NULL;
    UNICODE_STRING captured_name;
    HWND window;
    ULONG slot;
    /* Same controlled task setup as the CCPU callback-borrow fixture. Native
     * menu publication requires a task identity even with no attached menu.
     * This is not execution of original guest instructions. */
    fixture_creation_process.pwpi = &task_state;
    task_state.CSOwningThread = &fixture_creation_thread;
    fixture_creation_thread.ptdb = &task;
    task.hTaskWow = 0x4321u;
    runtime.lifecycle = &lifecycle;
    CHECK(wow_user_runtime_bind(&binding, &runtime,
        wow_fixture_creation_owner(&context), NULL));
    if (!binding.runtime) return 3;
    terminal_observer = SetWindowsHookExA(WH_CALLWNDPROCRET, wow_window_terminal_observer,
        NULL, GetCurrentThreadId());
    CHECK(terminal_observer != NULL);
    if (!terminal_observer) goto done;
    verify_gateway_failure_cleanup();
    instance = (HINSTANCE)0x23451234;
    cls.hInstance = instance; cls.lpszClassName = name; cls.lpfnWndProc = wow_window_class_creation_proc;
    RtlInitUnicodeString(&captured_name, L"S40_NATIVE_CREATE_BINDING");
    atom = wow_class_words_register_target(&context, &cls, &captured_name,
        class_words, NULL, &target);
    CHECK(atom != 0); if (!atom) goto done;
    {
        wow_class_words_binding *registered = wow_class_words_acquire(
            &context, atom, instance);
        CHECK(registered != NULL);
        CHECK(wow_class_words_publish_guest(wow_class_words_entry(registered), &cls));
        wow_class_words_release(registered);
    }
    veto_hook = SetWindowsHookExA(WH_CBT, reject_hook, NULL, GetCurrentThreadId());
    CHECK(veto_hook != NULL); if (!veto_hook) goto done;
    window = create(1); CHECK(window != NULL);
    if (window) CHECK(DestroyWindow(window));
    window = create(3); CHECK(window == NULL);
    if (window) DestroyWindow(window);
    CHECK(rejected_borrow!=NULL && wow_window_words_cleanup_value(rejected_borrow)->thread==NULL);
    wow_window_words_release(rejected_borrow);
    veto = TRUE; window = create(4); CHECK(window == NULL);
    if (window) DestroyWindow(window);
    CHECK(vetoed_borrow!=NULL && wow_window_words_cleanup_value(vetoed_borrow)->thread==NULL);
    wow_window_words_release(vetoed_borrow);
    veto = FALSE;
    /* A vetoed attempt must leave no pending scope that steals this create. */
    window = create(5); CHECK(window != NULL);
    if (window) CHECK(DestroyWindow(window));
    CHECK(UnhookWindowsHookEx(veto_hook));
#ifdef WOW_CREATION_UNWIND_FIXTURE
    verify_creation_unwind();
#endif
    verify_mdi_creation();
    verify_counted_dialog_creation();
    verify_control_creation_inputs();
    verify_dialog_enrollment_target();
    verify_sampled_generation_reuse();
    verify_class_token_identity();
    CHECK(wow_class_words_unregister(&context, atom, instance, NULL));
    CHECK(context.pclsPrivateList == NULL && context.pclsPublicList == NULL);
    CHECK(nested == 1 && rejected == 1 && vetoed == 1 && destroyed >= 4);
    for (slot=0;slot<=fixture_creation_objects.last_handle;++slot)
        CHECK(fixture_creation_objects.entries[slot].bType==TYPE_FREE);
done:
    if (terminal_observer) CHECK(UnhookWindowsHookEx(terminal_observer));
    CHECK(wow_user_runtime_unbind(&binding));
    fprintf(stderr,"WOW_NATIVE_CREATION_BINDING errors=%u callbacks=%u nested=%u rejected=%u vetoed=%u destroyed=%u table_empty=1\n",
        errors, callbacks, nested, rejected, vetoed, destroyed);
    return errors ? 3 : 0;
}
