#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#include "control-client/precomp.h"
#include <stdio.h>
#undef CreateWindowExW
#undef DestroyWindow
#undef pfnGetExpWinVer

/* Actual creator and private layout; native call boundaries are doubles.
 * Retirement destroys real hidden HWNDs. Validation follows native liveness;
 * this fixture does not simulate replacement HWND reuse. */
HWND WINAPI InternalCreateDialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
static DIALOG root;
static DLG data;
static WND child;
static HWND native_root, native_child;
static wow_dialog_context context;
static wow_task_client_info client;
static PFNWOWHANDLERSIN handlers;
__declspec(thread) wow_dialog_context *wow_dialog_current;
static unsigned failures, creates, allocations, releases, queries, updates, shows, destroys;
static HFONT transferred_font;
static BOOL child_only, fail_child, fail_promotion, fail_slot;
enum edge { NONE, ROOT_FONT, ALLOCATE, CREATE_CHILD, CHILD_FONT, CHILD_CODE,
    INITIALIZE, FOCUS_CHILD, DEFAULT_BUTTON, UISTATE, FOREGROUND, SHOW, UPDATE };
static enum edge fault;
static DWORD version;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d edge=%u child=%u\n", __LINE__, fault, child_only); } } while (0)
static BOOL dead(PWND w) { return TestWF(w, WFDESTROYED) || (w->fnid & FNID_STATUS_BITS); }
/* Procedure storage is an explicit transport double in this creator test;
 * native A/W ownership is exercised by wow_dialog_native_owner_probe. */
LONG WINAPI wow_dialog_host_set(HWND w, int index, LONG value, BOOL ansi)
{ CHECK(w == native_root && index == DWL_DLGPROC); (void)value; (void)ansi; if (fail_slot) SetLastError(ERROR_INVALID_INDEX); return 0; }
static void retire(enum edge at)
{
    PWND target;
    if (fault != at) return;
    target = child_only ? &child : &root.wnd;
    SetWF(target, WFDESTROYED); target->fnid |= FNID_CLEANEDUP_BIT;
    CHECK(DestroyWindow(child_only ? native_child : native_root));
    if (!child_only) {
        if (data.hUserFont) {
            transferred_font = data.hUserFont;
            CHECK(DeleteObject(data.hUserFont)); data.hUserFont = NULL;
        }
        if (data.hData) { ++releases; data.hData = NULL; }
        root.pdlg = NULL;
    }
}
static DWORD WINAPI expected_version(HANDLE module) { (void)module; return version; }
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void) { return &handlers; }
const wow_task_client_info *WINAPI wow_edit_client_info(void) { return &client; }
BOOL WINAPI wow_edit_native_ensure_gui_thread(void) { return TRUE; }
int WINAPI wow_edit_native_GdiGetCharDimensions(HDC dc, TEXTMETRICW *tm, LPINT height)
{ (void)dc; (void)tm; *height = 16; return 8; }
int WINAPI wow_edit_native_GetTextFaceAliasW(HDC dc, int count, LPWSTR face)
{
    typedef int (WINAPI *alias_fn)(HDC, int, LPWSTR);
    alias_fn alias = (alias_fn)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GetTextFaceAliasW");
    CHECK(alias != NULL);
    return alias ? alias(dc, count, face) : 0;
}
PWND WINAPI wow_window_client_validate_window(HWND w)
{ return !IsWindow(w) ? NULL : w == native_root ? &root.wnd : w == native_child ? &child : NULL; }
PWND WINAPI wow_window_client_validate_window_no_rip(HWND w)
{ return wow_window_client_validate_window(w); }
BOOL ValidateDialogPwnd(PWND w)
{ CHECK(w == &root.wnd && !dead(w)); if (fail_promotion) return FALSE; root.pdlg = &data; SetWF(w, WFDIALOGWINDOW); return TRUE; }
VOID WINAPI TellWOWThehDlg(HWND w) { CHECK(w == native_root && !dead(&root.wnd)); }
HANDLE WINAPI GetEditDS(void) { ++allocations; retire(ALLOCATE); return (HANDLE)0x1234; }
VOID WINAPI ReleaseEditDS(HANDLE segment) { CHECK(segment == (HANDLE)0x1234); ++releases; }
BOOL RealAdjustWindowRectEx(LPRECT r, DWORD style, BOOL menu, DWORD ex)
{ (void)r; (void)style; CHECK(!menu); (void)ex; return TRUE; }
void RepositionRect(PMONITOR monitor, LPRECT r, DWORD style, DWORD ex)
{ CHECK(monitor != NULL); (void)r; (void)style; (void)ex; }
static MONITORINFO monitor_info = {sizeof(MONITORINFO), {0,0,1024,768}, {0,0,1024,768}, MONITORINFOF_PRIMARY};
MONITORINFO *WINAPI wow_dialog_monitor(HMONITOR monitor)
{ return monitor ? &monitor_info : NULL; }
MONITORINFO *WINAPI wow_dialog_primary_monitor(void) { return &monitor_info; }
HWND WINAPI wow_native_GetForegroundWindow(void) { return NULL; }
/* This fixture has no foreground owner; these accesses must remain unused. */
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID)
{ CHECK(FALSE); return NULL; }
DWORD WINAPI wow_native_GetWindowThreadProcessId(HWND window, LPDWORD process)
{ (void)window; (void)process; CHECK(FALSE); return 0; }
DWORD GetAppCompatFlags2(WORD version) { (void)version; return 0; }
PWND _GetNextDlgTabItem(PWND w, PWND previous, BOOL back)
{ CHECK(w == &root.wnd && !dead(w)); (void)previous; (void)back; return dead(&child) ? NULL : &child; }
void DlgSetFocus(HWND w) { CHECK(w == native_child && !dead(&root.wnd) && !dead(&child)); retire(FOCUS_CHILD); }
void xxxCheckDefPushButton(PWND w, HWND old, HWND next)
{ CHECK(w == &root.wnd); (void)old; (void)next; if (IsWindow(native_root)) retire(DEFAULT_BUTTON); }
LONG WINAPI wow_window_client_send_message(PWND w, UINT msg, WPARAM wp, LPARAM lp, BOOL ansi)
{
    CHECK(w == &root.wnd && !dead(w) && root.pdlg); (void)wp; (void)lp; (void)ansi;
    if (msg == WM_SETFONT) retire(ROOT_FONT);
    else if (msg == WM_CHANGEUISTATE) retire(UISTATE);
    else { CHECK(msg == WM_INITDIALOG); retire(INITIALIZE); }
    return TRUE;
}
LRESULT WINAPI wow_native_SendMessageW(HWND w, UINT msg, WPARAM wp, LPARAM lp)
{
    CHECK(w == native_child && !dead(&root.wnd)); (void)wp; (void)lp;
    if (dead(&child)) return 0; /* Invalid HWND: native USER sends no callback. */
    if (msg == WM_SETFONT) { retire(CHILD_FONT); return 0; }
    CHECK(msg == WM_GETDLGCODE); ++queries; retire(CHILD_CODE); return DLGC_DEFPUSHBUTTON;
}
HWND WINAPI wow_user_create_dialog_window_ex(DWORD ex, PLARGE_STRING cls,
    PLARGE_STRING title, DWORD style, int x, int y, int cx, int cy,
    HWND parent, HMENU menu, HANDLE instance, LPVOID parameter, DWORD ver)
{
    (void)cls; (void)title; (void)x; (void)y; (void)cx; (void)cy; (void)menu;
    (void)instance; (void)parameter; (void)ver;
    ++creates;
    if (!parent) { root.wnd.style = style; root.wnd.ExStyle = ex; return native_root; }
    CHECK(parent == native_root && !dead(&root.wnd));
    child.style = style; child.ExStyle = ex; retire(CREATE_CHILD);
    return fail_child ? NULL : native_child;
}
BOOL WINAPI wow_native_ShowWindow(HWND w, int command)
{ CHECK(w == native_root && !dead(&root.wnd)); (void)command; ++shows; retire(SHOW); return TRUE; }
BOOL WINAPI wow_native_UpdateWindow(HWND w)
{ CHECK(w == native_root); if (!IsWindow(w)) return FALSE; ++updates; retire(UPDATE); return TRUE; }
BOOL WINAPI wow_native_SetForegroundWindow(HWND w)
{ CHECK(w == native_root && !dead(&root.wnd)); retire(FOREGROUND); return TRUE; }
BOOL WINAPI wow_native_DestroyWindow(HWND w)
{ CHECK(w == native_root && !dead(&root.wnd)); ++destroys; return TRUE; }
BOOL WINAPI wow_native_SetWindowContextHelpId(HWND w, DWORD id)
{ (void)w; (void)id; CHECK(FALSE); return FALSE; }
HMENU WINAPI wow_menu_native_GetSystemMenu(HWND w, BOOL reset)
{ (void)w; (void)reset; CHECK(FALSE); return NULL; }
BOOL WINAPI wow_menu_native_DestroyMenu(HMENU menu) { (void)menu; CHECK(FALSE); return FALSE; }
HMENU WINAPI wow_user_load_menuW(HINSTANCE module, LPCWSTR name)
{ (void)module; (void)name; CHECK(FALSE); return NULL; }
BOOL WINAPI wow_dialog_set_system_menu(HWND w) { (void)w; CHECK(FALSE); return FALSE; }
POINT WINAPI wow_dialog_cursor_position(void) { POINT p = {0}; CHECK(FALSE); return p; }

static __declspec(align(4)) BYTE resource[256];
static void setup(enum edge at, BOOL child_fault, DWORD app_version)
{
    if (IsWindow(native_child)) CHECK(DestroyWindow(native_child));
    if (IsWindow(native_root)) CHECK(DestroyWindow(native_root));
    native_root = CreateWindowExW(0, L"STATIC", L"creator", 0, 0, 0, 10, 10, HWND_MESSAGE, NULL, NULL, NULL);
    native_child = CreateWindowExW(0, L"STATIC", L"child", WS_CHILD, 0, 0, 10, 10, native_root, NULL, NULL, NULL);
    CHECK(native_root && native_child);
    ZeroMemory(&root, sizeof(root)); ZeroMemory(&data, sizeof(data)); ZeroMemory(&child, sizeof(child));
    root.wnd.head.h = native_root; root.wnd.fnid = FNID_DIALOG; root.wnd.cbwndExtra = DLGWINDOWEXTRA;
    child.head.h = native_child; child.spwndParent = &root.wnd;
    fault = at; child_only = child_fault; fail_child = FALSE; version = app_version;
    context.server.bKeyboardPref = at != UISTATE;
    context.server.PUSIFlags = PUSIF_UIEFFECTS;
    client.dwExpWinVer = version; transferred_font = NULL;
    creates = allocations = releases = queries = updates = shows = destroys = 0;
}
static HWND create(UINT flags, BOOL wow, BOOL font, BOOL nofail)
{
    LPDLGTEMPLATE t = (LPDLGTEMPLATE)resource;
    LPDLGITEMTEMPLATE item;
    WORD *p;
    unsigned offset = 24;
    ZeroMemory(resource, sizeof(resource));
    t->style = WS_POPUP | WS_VISIBLE | (font ? DS_SETFONT : 0) | (nofail ? DS_NOFAILCREATE : 0);
    if (fault == FOREGROUND) t->style |= DS_SETFOREGROUND;
    t->cdit = 1; t->cx = 80; t->cy = 40;
    if (font) { p = (WORD *)(resource + offset); *p++ = 8;
        CopyMemory(p, L"Arial", sizeof(L"Arial")); offset += 2 + sizeof(L"Arial"); }
    offset = (offset + 3) & ~3u;
    item = (LPDLGITEMTEMPLATE)(resource + offset);
    item->style = WS_CHILD | WS_TABSTOP; item->cx = 20; item->cy = 10; item->id = IDOK;
    p = (WORD *)((BYTE *)item + sizeof(*item)); *p++ = 0xffff; *p++ = 0x81; *p++ = 0; *p++ = 0;
    return InternalCreateDialog((HANDLE)(ULONG_PTR)(wow ? 1 : 0), t, sizeof(resource), NULL,
        (DLGPROC)(ULONG_PTR)1, 17, flags);
}
static void release_transferred(void)
{
    if (data.hUserFont) { CHECK(DeleteObject(data.hUserFont)); data.hUserFont = NULL; }
    if (data.hData) { ++releases; data.hData = NULL; }
}
int __cdecl main(void)
{
    unsigned edge, legacy, exempt;
    HWND result;
    handlers.pfnGetExpWinVer = expected_version;
    context.server.cxSysFontChar = 8; context.server.cySysFontChar = 16;
    context.server.atomSysClass[ICLS_EDIT] = 0x81;
    context.server.dmLogPixels = 96;
    context.server.bKeyboardPref = TRUE;
    context.server.rcWork.right = 1024; context.server.rcWork.bottom = 768;
    wow_dialog_current = &context;
    setup(NONE, FALSE, 0x30a); fail_promotion = TRUE;
    result = create(0, TRUE, TRUE, FALSE);
    printf("promotion-failed result=%p creates=%u destroys=%u\n", result, creates, destroys);
    CHECK(!result && creates == 1 && destroys == 1);
    fail_promotion = FALSE;
    setup(NONE, FALSE, 0x30a); fail_slot = TRUE;
    result = create(0, TRUE, TRUE, FALSE);
    printf("slot-failed result=%p creates=%u destroys=%u\n", result, creates, destroys);
    CHECK(!result && creates == 1 && destroys == 1);
    fail_slot = FALSE; SetLastError(0);
    setup(NONE, FALSE, 0x30a); CHECK(create(0, TRUE, TRUE, FALSE) == native_root);
    CHECK(creates == 2 && allocations == 1 && queries == 1 && shows == 1 && updates == 1);
    release_transferred(); CHECK(releases == 1);
    for (edge = ROOT_FONT; edge <= UPDATE; ++edge) {
        for (legacy = 0; legacy < 2; ++legacy) for (exempt = 0; exempt < 2; ++exempt) {
            setup((enum edge)edge, FALSE, legacy ? 0x30a : 0x400);
            result = create(exempt ? SCDLG_NOREVALIDATE : 0, edge != FOREGROUND, TRUE, FALSE);
            CHECK(!root.pdlg);
            if (edge == FOCUS_CHILD || edge == DEFAULT_BUTTON)
                CHECK(result == (legacy ? native_root : NULL));
            else if (edge == FOREGROUND) CHECK(!result);
            else CHECK(result == (exempt ? native_root : NULL));
            CHECK(!destroys);
            if (allocations) CHECK(releases == 1);
            if (edge <= SHOW) CHECK(!updates);
            release_transferred();
        }
    }
    for (edge = CHILD_FONT; edge <= CHILD_CODE; ++edge) for (exempt = 0; exempt < 2; ++exempt) {
        setup((enum edge)edge, TRUE, 0x30a);
        result = create(0, TRUE, TRUE, exempt);
        CHECK(result == native_root);
        CHECK(!destroys); release_transferred();
    }
    if (IsWindow(native_child)) CHECK(DestroyWindow(native_child));
    if (IsWindow(native_root)) CHECK(DestroyWindow(native_root));
    printf("original-creator-lifetime failures=%u\n", failures);
    return failures ? 1 : 0;
}
