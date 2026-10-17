#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#define WOW_ORIGINAL_DIALOG_MANAGER
#include "control-client/precomp.h"
#include <stdio.h>
/* Native fixture-owned identity only; production focus/message services
 * below remain explicit doubles. */
#undef CreateWindowExW
#undef DestroyWindow

/* Actual original dlgmgr2 keyboard/default-button and dlgmgr focus bodies;
 * graph and native calls are explicit doubles, not a second dialog engine. */
BOOL WINAPI wow_original_IsDialogMessageW(HWND, LPMSG);
BOOL WINAPI wow_original_IsDialogMessageA(HWND, LPMSG);
LONG WINAPI DefDlgProcWorker(PWND, UINT, WPARAM, LPARAM, DWORD);
BOOL xxxRestoreDlgFocus(PWND);
static HWND native_target;
static DIALOG dialog_storage;
#define dialog dialog_storage.wnd
static DLG dialog_data;
static WND source;
static BUTNWND target;
static BUTN button;
static unsigned failures, focused, clicked, styles;
static UINT target_code;
enum fault_point { NONE, SOURCE_QUERY, TARGET_QUERY, FOCUS, DEFAULT_ID, CHECK_STATE, STYLE, SELECTION };
static enum fault_point fault;
static BOOL kill_dialog;
static BOOL raise_filter;
#define FILTER_FAULT ((DWORD)0xe0447121)
__declspec(thread) wow_edit_context *wow_edit_current;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d fault=%u root=%u\n", __LINE__, fault, kill_dialog); } } while (0)
static BOOL dead(PWND w) { return TestWF(w, WFDESTROYED) || (w->fnid & FNID_STATUS_BITS); }
/* These focus cases have no application dialog procedure/result. Native
 * storage is tested separately; unexpected dispatch is a failure here. */
LONG WINAPI wow_dialog_host_get(HWND w, int index, BOOL ansi)
{ CHECK(w == dialog.head.h && (index == DWL_MSGRESULT || index == DWL_DLGPROC)); (void)ansi; return 0; }
BOOL WINAPI wow_dialog_host_initialize(HWND w)
{ (void)w; CHECK(FALSE); return FALSE; }
LRESULT WINAPI wow_dialog_host_call(HWND w, UINT m, WPARAM a, LPARAM b, BOOL ansi)
{ (void)w; (void)m; (void)a; (void)b; (void)ansi; CHECK(FALSE); return 0; }
LRESULT WINAPI wow_dialog_host_notify(HWND w, UINT m, WPARAM a, LPARAM b, BOOL ansi)
{ (void)w; (void)m; (void)a; (void)b; (void)ansi; CHECK(FALSE); return 0; }
static void callback(enum fault_point point)
{
    PWND w;
    if (fault != point) return;
    w = kill_dialog ? &dialog : point == SOURCE_QUERY ? &source : &target.wnd;
    SetWF(w, WFDESTROYED); w->fnid |= FNID_CLEANEDUP_BIT;
    if (kill_dialog) dialog_storage.pdlg = NULL;
}
PWND WINAPI wow_window_client_validate_window(HWND w)
{
    if (w == dialog.head.h) return &dialog;
    if (w == source.head.h) return &source;
    if (w == target.wnd.head.h) return &target.wnd;
    return NULL;
}
PWND WINAPI wow_window_client_validate_window_no_rip(HWND w)
{ return wow_window_client_validate_window(w); }
PWND WINAPI wow_window_client_validate_hwnd(HWND w)
{ return wow_window_client_validate_window(w); }
__declspec(noreturn) VOID WINAPI wow_user_native_call_failed(const char *where, DWORD error)
{ printf("fatal %s %lu\n", where, error); ExitProcess(99); }
void WINAPI FreeLookasideEntry(PLOOKASIDE pool, PVOID value)
{ (void)pool; (void)value; CHECK(FALSE); }
BOOL _IsChild(PWND parent, PWND child)
{ return parent == &dialog && (child == &source || child == &target.wnd); }
PWND _GetDlgItem(PWND parent, int id)
{ CHECK(parent == &dialog && !dead(parent)); return id == IDOK ? &target.wnd : NULL; }
PWND _GetChildControl(PWND parent, PWND child)
{ CHECK(parent == &dialog && !dead(parent)); return child; }
PWND _NextControl(PWND parent, PWND child, UINT flags)
{ (void)flags; CHECK(parent == &dialog && !dead(parent)); return child == &target.wnd ? &source : &target.wnd; }
PWND _GetNextDlgTabItem(PWND parent, PWND child, BOOL previous)
{ (void)previous; CHECK(parent == &dialog && child == &source && !dead(parent)); return &target.wnd; }
PWND _GetNextDlgGroupItem(PWND parent, PWND child, BOOL previous)
{ return _GetNextDlgTabItem(parent, child, previous); }
PWND xxxGotoNextMnem(PWND parent, PWND child, WCHAR character)
{ (void)parent; (void)child; (void)character; CHECK(FALSE); return NULL; }
BOOL WINAPI wow_native_CallMsgFilterW(LPMSG message, int code)
{
    CHECK(code == MSGF_DIALOGBOX);
    if (raise_filter) {
        CHECK(message->wParam == 0x00420041);
        RaiseException(FILTER_FAULT, 0, 0, NULL);
    }
    return FALSE;
}
HWND WINAPI wow_native_GetFocus(void) { return source.head.h; }
LRESULT WINAPI wow_native_DispatchMessageW(const MSG *message)
{ (void)message; CHECK(FALSE); return 0; }
LONG WINAPI wow_default_window_proc(PWND w, UINT m, WPARAM wp, LPARAM lp, DWORD ansi)
{ (void)w; (void)m; (void)wp; (void)lp; (void)ansi; CHECK(FALSE); return 0; }
HWND WINAPI wow_native_SetFocus(HWND window)
{
    CHECK(window == target.wnd.head.h && !dead(&dialog) && !dead(&target.wnd));
    ++focused; callback(FOCUS);
    return source.head.h;
}
LRESULT WINAPI wow_native_SendMessageW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    PWND node = wow_window_client_validate_window(window);
    (void)lp;
    CHECK(node && !dead(node) && !dead(&dialog));
    if (message == WM_GETDLGCODE) {
        callback(node == &source ? SOURCE_QUERY : TARGET_QUERY);
        return node == &target.wnd ? target_code : 0;
    }
    if (message == DM_GETDEFID) {
        CHECK(node == &dialog); callback(DEFAULT_ID); return MAKELONG(IDOK, DC_HASDEFID);
    }
    if (message == BM_GETCHECK) { callback(CHECK_STATE); return 0; }
    if (message == EM_SETSEL) { callback(SELECTION); return 0; }
    if (message == BM_CLICK) { ++clicked; return 0; }
    if (message == BM_SETSTYLE) {
        ++styles; target_code = wp == BS_DEFPUSHBUTTON ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON;
        callback(STYLE); return 0;
    }
    CHECK(FALSE); return 0;
}
static void setup(UINT code)
{
    ZeroMemory(&dialog_storage, sizeof(dialog_storage)); ZeroMemory(&source, sizeof(source));
    ZeroMemory(&dialog_data, sizeof(dialog_data));
    dialog_storage.pdlg = &dialog_data;
    dialog.fnid = FNID_DIALOG;
    dialog.cbwndExtra = DLGWINDOWEXTRA;
    SetWF(&dialog, WFDIALOGWINDOW);
    ZeroMemory(&target, sizeof(target)); ZeroMemory(&button, sizeof(button));
    dialog.head.h = (HWND)(ULONG_PTR)1; source.head.h = (HWND)(ULONG_PTR)2;
    target.wnd.head.h = native_target; target.wnd.fnid = FNID_BUTTON;
    target.pbutn = &button; button.spwnd = &target.wnd;
    target.wnd.spmenu = (PMENU)(ULONG_PTR)IDOK;
    target.wnd.style = BS_AUTORADIOBUTTON;
    target_code = code; focused = clicked = styles = 0; fault = NONE; kill_dialog = raise_filter = FALSE;
}
static BOOL key(UINT message, WPARAM keycode)
{
    MSG input = {0}; input.hwnd = source.head.h;
    input.message = message; input.wParam = keycode;
    return wow_original_IsDialogMessageW(dialog.head.h, &input);
}
int __cdecl main(void)
{
    unsigned root, kind;
    BOOL caught = FALSE;
    MSG ansi = {0};
    const UINT codes[] = {0, DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON, DLGC_RADIOBUTTON};
    native_target = CreateWindowExW(0, L"STATIC", L"focus lifetime identity",
        0, 0, 0, 1, 1, HWND_MESSAGE, NULL, GetModuleHandleW(NULL), NULL);
    if (!native_target) return 2;
    for (root = 0; root < 2; ++root) {
        setup(0); fault = SOURCE_QUERY; kill_dialog = root;
        CHECK(key(WM_KEYDOWN, VK_TAB) && !focused && !styles);
        setup(0); fault = SOURCE_QUERY; kill_dialog = root;
        CHECK(key(WM_CHAR, 't') && !focused && !styles);
        setup(0); fault = FOCUS; kill_dialog = root;
        CHECK(key(WM_KEYDOWN, VK_TAB) && focused == 1 && !styles);
        for (kind = 0; kind < ARRAYSIZE(codes); ++kind) {
            setup(codes[kind]); fault = FOCUS; kill_dialog = root;
            CHECK(key(WM_KEYDOWN, VK_RIGHT) && focused == 1 && !styles && !clicked);
            setup(codes[kind]); fault = TARGET_QUERY; kill_dialog = root;
            CHECK(key(WM_KEYDOWN, VK_RIGHT) && !focused && !styles && !clicked);
        }
        setup(DLGC_RADIOBUTTON); fault = CHECK_STATE; kill_dialog = root;
        CHECK(key(WM_KEYDOWN, VK_RIGHT) && focused == 1 && !clicked);
        setup(0); fault = DEFAULT_ID; kill_dialog = root;
        CHECK(key(WM_KEYDOWN, VK_TAB) && focused == 1 && !styles && !clicked);
    }
    setup(DLGC_HASSETSEL); fault = TARGET_QUERY;
    DlgSetFocus(target.wnd.head.h);
    CHECK(!focused);
    setup(DLGC_HASSETSEL); fault = SELECTION;
    DlgSetFocus(target.wnd.head.h);
    CHECK(!focused);
    setup(DLGC_HASSETSEL);
    DlgSetFocus(target.wnd.head.h);
    CHECK(focused == 1);
    setup(DLGC_RADIOBUTTON);
    CHECK(key(WM_KEYDOWN, VK_RIGHT) && focused == 1 && clicked == 1);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    CHECK(key(WM_KEYDOWN, VK_RIGHT) && focused == 1 && styles == 1 && !button.buttonState);
    setup(0); raise_filter = TRUE;
    ansi.hwnd = source.head.h; ansi.message = WM_CHAR; ansi.wParam = 0x4241;
    __try { (void)wow_original_IsDialogMessageA(dialog.head.h, &ansi); }
    __except (GetExceptionCode() == FILTER_FAULT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && ansi.wParam == 0x4241);
    /* Actual default-dialog callers must stop too: the helper returning
     * safely does not keep its caller's PDLG alive. These are explicit
     * terminal-state doubles, not evidence of real HWND reuse. */
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    CHECK(DefDlgProcWorker(&dialog, DM_SETDEFID, IDOK, 0, FALSE) == TRUE);
    CHECK(dialog_data.result == IDOK && styles == 1);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    fault = TARGET_QUERY; kill_dialog = TRUE;
    CHECK(DefDlgProcWorker(&dialog, DM_SETDEFID, IDOK, 0, FALSE) == TRUE);
    CHECK(!dialog_storage.pdlg && !dialog_data.result && !styles);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    fault = STYLE; kill_dialog = TRUE;
    CHECK(DefDlgProcWorker(&dialog, DM_SETDEFID, IDOK, 0, FALSE) == TRUE);
    CHECK(!dialog_storage.pdlg && !dialog_data.result && styles == 1);
    for (root = 0; root < 2; ++root) {
        setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
        fault = FOCUS; kill_dialog = root;
        CHECK(DefDlgProcWorker(&dialog, WM_NEXTDLGCTL,
            (WPARAM)target.wnd.head.h, TRUE, FALSE) == TRUE);
        CHECK(focused == 1 && !styles);
    }
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    dialog_data.hwndFocusSave = target.wnd.head.h;
    CHECK(xxxRestoreDlgFocus(&dialog));
    CHECK(focused == 1 && !dialog_data.hwndFocusSave);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    dialog_data.hwndFocusSave = target.wnd.head.h;
    fault = TARGET_QUERY;
    CHECK(!xxxRestoreDlgFocus(&dialog));
    CHECK(!focused && !dialog_data.hwndFocusSave && dialog_storage.pdlg);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    dialog_data.hwndFocusSave = target.wnd.head.h;
    fault = TARGET_QUERY; kill_dialog = TRUE;
    CHECK(!xxxRestoreDlgFocus(&dialog));
    CHECK(!focused && !dialog_storage.pdlg);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    dialog_data.hwndFocusSave = target.wnd.head.h;
    fault = FOCUS; kill_dialog = TRUE;
    CHECK(xxxRestoreDlgFocus(&dialog));
    CHECK(focused == 1 && !dialog_storage.pdlg);
    setup(DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON);
    dialog_data.hwndFocusSave = target.wnd.head.h;
    fault = TARGET_QUERY; kill_dialog = TRUE;
    (void)DefDlgProcWorker(&dialog, WM_SETFOCUS, 0, 0, FALSE);
    CHECK(!focused && !dialog_storage.pdlg);
    CHECK(DestroyWindow(native_target));
    printf("original-keyboard-lifetime failures=%u\n", failures);
    return failures ? 1 : 0;
}
