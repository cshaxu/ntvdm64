#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#include "control-client/precomp.h"
#include <stdio.h>
#undef CreateWindowExW
#undef DestroyWindow
#undef pfnGetExpWinVer

/* Actual dlgbegin body and production private layout. Native creation and
 * messages are explicit doubles. Real hidden HWNDs deliberately remain valid
 * after simulated retirement, so IsWindow alone cannot satisfy assertions. */
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
static BOOL child_only, fail_child;
enum edge { NONE, ROOT_FONT, ALLOCATE, CREATE_CHILD, CHILD_FONT, CHILD_CODE,
    INITIALIZE, FOCUS_CHILD, DEFAULT_BUTTON, FOREGROUND, SHOW, UPDATE };
static enum edge fault;
static DWORD version;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d edge=%u child=%u\n", __LINE__, fault, child_only); } } while (0)
static BOOL dead(PWND w) { return TestWF(w, WFDESTROYED) || (w->fnid & FNID_STATUS_BITS); }
/* Procedure storage is an explicit transport double in this creator test;
 * native A/W ownership is exercised by wow_dialog_native_owner_probe. */
LONG WINAPI wow_dialog_host_set(HWND w, int index, LONG value, BOOL ansi)
{ CHECK(w == native_root && index == DWL_DLGPROC); (void)value; (void)ansi; return 0; }
static void retire(enum edge at)
{
    PWND target;
    if (fault != at) return;
    target = child_only ? &child : &root.wnd;
    SetWF(target, WFDESTROYED); target->fnid |= FNID_CLEANEDUP_BIT;
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
PWND WINAPI wow_window_client_validate_window(HWND w)
{ return w == native_root ? &root.wnd : w == native_child ? &child : NULL; }
PWND WINAPI wow_window_client_validate_window_no_rip(HWND w)
{ return wow_window_client_validate_window(w); }
BOOL ValidateDialogPwnd(PWND w)
{ CHECK(w == &root.wnd && !dead(w)); root.pdlg = &data; SetWF(w, WFDIALOGWINDOW); return TRUE; }
DWORD WINAPI WOWDlgInit(HWND w, LONG value) { CHECK(w == native_root && !dead(&root.wnd)); return value; }
HANDLE WINAPI GetEditDS(void) { ++allocations; retire(ALLOCATE); return (HANDLE)0x1234; }
VOID WINAPI ReleaseEditDS(HANDLE segment) { CHECK(segment == (HANDLE)0x1234); ++releases; }
BOOL _AdjustWindowRectEx(LPRECT r, LONG style, BOOL menu, DWORD ex)
{ (void)r; (void)style; CHECK(!menu); (void)ex; return TRUE; }
void RepositionRect(LPRECT r, DWORD style, DWORD ex) { (void)r; (void)style; (void)ex; }
PWND _GetNextDlgTabItem(PWND w, PWND previous, BOOL back)
{ CHECK(w == &root.wnd && !dead(w)); (void)previous; (void)back; return dead(&child) ? NULL : &child; }
void DlgSetFocus(HWND w) { CHECK(w == native_child && !dead(&root.wnd) && !dead(&child)); retire(FOCUS_CHILD); }
void xxxCheckDefPushButton(PWND w, HWND old, HWND next)
{ CHECK(w == &root.wnd && !dead(w)); (void)old; (void)next; retire(DEFAULT_BUTTON); }
LONG WINAPI wow_window_client_send_message(PWND w, UINT msg, WPARAM wp, LPARAM lp, BOOL ansi)
{
    CHECK(w == &root.wnd && !dead(w) && root.pdlg); (void)wp; (void)lp; (void)ansi;
    if (msg == WM_SETFONT) retire(ROOT_FONT);
    else { CHECK(msg == WM_INITDIALOG); retire(INITIALIZE); }
    return TRUE;
}
LRESULT WINAPI wow_native_SendMessageW(HWND w, UINT msg, WPARAM wp, LPARAM lp)
{
    CHECK(w == native_child && !dead(&child) && !dead(&root.wnd)); (void)wp; (void)lp;
    if (msg == WM_SETFONT) { retire(CHILD_FONT); return 0; }
    CHECK(msg == WM_GETDLGCODE); ++queries; retire(CHILD_CODE); return DLGC_DEFPUSHBUTTON;
}
HWND WINAPI wow_user_create_dialog_window_ex(DWORD ex, PLARGE_STRING cls,
    PLARGE_STRING title, DWORD style, int x, int y, int cx, int cy,
    HWND parent, HMENU menu, HANDLE instance, LPVOID parameter, DWORD ver, LPDWORD words)
{
    (void)cls; (void)title; (void)x; (void)y; (void)cx; (void)cy; (void)menu;
    (void)instance; (void)parameter; (void)ver; (void)words;
    ++creates;
    if (!parent) { root.wnd.style = style; root.wnd.ExStyle = ex; return native_root; }
    CHECK(parent == native_root && !dead(&root.wnd));
    child.style = style; child.ExStyle = ex; retire(CREATE_CHILD);
    return fail_child ? NULL : native_child;
}
BOOL WINAPI wow_native_ShowWindow(HWND w, int command)
{ CHECK(w == native_root && !dead(&root.wnd)); (void)command; ++shows; retire(SHOW); return TRUE; }
BOOL WINAPI wow_native_UpdateWindow(HWND w)
{ CHECK(w == native_root && !dead(&root.wnd)); ++updates; retire(UPDATE); return TRUE; }
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
    ZeroMemory(&root, sizeof(root)); ZeroMemory(&data, sizeof(data)); ZeroMemory(&child, sizeof(child));
    root.wnd.head.h = native_root; root.wnd.fnid = FNID_DIALOG; root.wnd.cbwndExtra = DLGWINDOWEXTRA;
    child.head.h = native_child; child.spwndParent = &root.wnd;
    fault = at; child_only = child_fault; fail_child = FALSE; version = app_version;
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
    context.server.atomSysClass[ICLS_EDIT] = 0x81; context.screen.cyPixelsPerInch = 96;
    context.server.rcWork.right = 1024; context.server.rcWork.bottom = 768;
    wow_dialog_current = &context;
    native_root = CreateWindowExW(0, L"STATIC", L"creator fixture", 0, 0, 0, 10, 10, HWND_MESSAGE, NULL, NULL, NULL);
    native_child = CreateWindowExW(0, L"STATIC", L"child fixture", 0, 0, 0, 10, 10, HWND_MESSAGE, NULL, NULL, NULL);
    if (!native_root || !native_child) return 2;
    setup(NONE, FALSE, 0x30a); CHECK(create(0, TRUE, TRUE, FALSE) == native_root);
    CHECK(creates == 2 && allocations == 1 && queries == 1 && shows == 1 && updates == 1);
    release_transferred(); CHECK(releases == 1);
    for (edge = ROOT_FONT; edge <= UPDATE; ++edge) {
        for (legacy = 0; legacy < 2; ++legacy) for (exempt = 0; exempt < 2; ++exempt) {
            setup((enum edge)edge, FALSE, legacy ? 0x30a : 0x400);
            result = create(exempt ? SCDLG_NOREVALIDATE : 0, edge != FOREGROUND, TRUE, FALSE);
            CHECK(!root.pdlg);
            if (edge == INITIALIZE || edge == FOCUS_CHILD || edge == DEFAULT_BUTTON)
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
        CHECK(result == (exempt ? native_root : NULL));
        CHECK(destroys == (exempt ? 0u : 1u)); release_transferred();
    }
    CHECK(DestroyWindow(native_child)); CHECK(DestroyWindow(native_root));
    printf("original-creator-lifetime failures=%u\n", failures);
    return failures ? 1 : 0;
}
