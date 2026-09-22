#include "wow_window_creation_binding.h"
#include "wow_creation_owner_fixture.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

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
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static void verify_gateway_failure_cleanup(void)
{
    HWND window = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 8, 8,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    WW words = {0};
    wow_window_dispatch_target target = {0, NULL};
    wow_window_words_binding *borrow;
    BOOL caught = FALSE;
    CHECK(window != NULL); if (!window) return;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    target.procedure = (DWORD)wow_window_native_proc;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    target.procedure = 0x81234567;
    CHECK(!wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_words_acquire(window) == NULL);
    CHECK(wow_window_native_proc(window, WM_CREATE, 0, 0) == -1);
    CHECK(GetLastError() == ERROR_INVALID_DATA);
    target.callback = raising_callback;
    CHECK(wow_window_words_attach_target(window, &words, NULL, &target));
    borrow = wow_window_words_acquire(window);
    CHECK(borrow != NULL);
    /* Direct invocation keeps SEH under test control, not USER's WNDPROC
     * exception policy. The STATIC window itself is still alive afterward. */
    __try {
        wow_window_native_proc(window, WM_NCDESTROY, 0, 0);
    } __except (GetExceptionCode() == 0xE0420040 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught);
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
    wow_window_dispatch_target target = {0x81234567, callback};
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
static void verify_mdi_creation(void)
{
    HWND frame, client, child;
    CLIENTCREATESTRUCT mdi = {NULL, 100};
    CREATESTRUCTA request = {0};
    CREATESTRUCTW wide;
    WW words = {0};
    wow_window_dispatch_target target = {0x81234567, callback};
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
    if (child) { CHECK(GetParent(child) == client); CHECK(DestroyWindow(child)); }
    CopyMemory(&wide, &request, sizeof(wide));
    wide.lpszClass = L"S40_NATIVE_CREATE_BINDING"; wide.lpszName = L"";
    child = wow_window_create_boundW(wow_fixture_creation_owner(&context), atom, &wide, &words, &target);
    CHECK(child != NULL);
    if (child) { CHECK(GetParent(child) == client); CHECK(DestroyWindow(child)); }
    CHECK(mdi_created == 2);
    CHECK(DestroyWindow(frame));
}
int __cdecl main(void)
{
    WNDCLASSEXA cls = {sizeof(cls)};
    UNICODE_STRING captured_name;
    HWND window;
    ULONG slot;
    verify_gateway_failure_cleanup();
    instance = (HINSTANCE)0x23451234;
    cls.hInstance = instance; cls.lpszClassName = name; cls.lpfnWndProc = wow_window_native_proc;
    RtlInitUnicodeString(&captured_name, L"S40_NATIVE_CREATE_BINDING");
    atom = wow_class_words_register(&context, &cls, &captured_name, class_words, NULL);
    CHECK(atom != 0); if (!atom) return 3;
    veto_hook = SetWindowsHookExA(WH_CBT, reject_hook, NULL, GetCurrentThreadId());
    CHECK(veto_hook != NULL); if (!veto_hook) return 3;
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
    verify_mdi_creation();
    CHECK(wow_class_words_unregister(&context, atom, instance, NULL));
    CHECK(context.pclsPrivateList == NULL && context.pclsPublicList == NULL);
    CHECK(nested == 1 && rejected == 1 && vetoed == 1 && destroyed >= 4);
    for (slot=0;slot<=fixture_creation_objects.last_handle;++slot)
        CHECK(fixture_creation_objects.entries[slot].bType==TYPE_FREE);
    printf("WOW_NATIVE_CREATION_BINDING errors=%u callbacks=%u nested=%u rejected=%u vetoed=%u destroyed=%u table_empty=1\n",
        errors, callbacks, nested, rejected, vetoed, destroyed);
    return errors ? 3 : 0;
}
