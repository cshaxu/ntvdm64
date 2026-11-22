#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#define WOW_ORIGINAL_DIALOG_MANAGER
#include "control-client/precomp.h"
#include <stdio.h>

/* Link the actual original dmmnem body and production BUTN scope. Only
 * native dispatch, graph traversal and final allocator are doubled. */
static WND root, start, desktop, disabled[513];
static BOOL exhausted;
static BUTNWND button;
static BUTN data;
static LOOKASIDE pool;
static unsigned failures, queries, texts, focuses, clicks, commands, frees;
static unsigned destroy_query, destroy_flash;
static BOOL destroy_text, destroy_focus, throw_focus, destroy_click, retired;
static UINT code;
__declspec(thread) wow_edit_context *wow_edit_current;
#define FAULT ((DWORD)0xe0447120)
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
int FindMnemChar(LPWSTR, WCHAR, BOOL, BOOL);
PWND xxxGNM_FindNextMnem(PWND, PWND, WCHAR);
PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{ CHECK(window == button.wnd.head.h); return retired ? NULL : &button.wnd; }
__declspec(noreturn) VOID WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{ printf("fatal %s %lu\n", stage, error); ExitProcess(99); }
void WINAPI FreeLookasideEntry(PLOOKASIDE owner, PVOID value)
{
    CHECK(owner == &pool && value == &data);
    CHECK(!(data.buttonState & BST_DONTCLICK));
    ++frees; data.buttonState = 0xdead;
}
static void destroy(void)
{
    SetWF(&button.wnd, WFDESTROYED);
    button.wnd.fnid |= FNID_CLEANEDUP_BIT;
    data.spwnd = NULL;
    retired = TRUE;
    button.wnd.spwndParent = &desktop;
}
PWND _GetChildControl(PWND dialog, PWND child)
{ CHECK(dialog == &root); return child ? child : &start; }
PWND _GetNextDlgGroupItem(PWND dialog, PWND child, BOOL previous)
{
    CHECK(dialog == &root && !previous);
    if (exhausted) return child == &start ? &disabled[0] : &disabled[(child - disabled + 1) % 513];
    if (retired) return &start;
    return child == &button.wnd ? &start : &button.wnd;
}
PWND _NextControl(PWND dialog, PWND child, UINT flags)
{ (void)flags; return _GetNextDlgGroupItem(dialog, child, FALSE); }
PWND _GetNextDlgTabItem(PWND dialog, PWND child, BOOL previous)
{ (void)child; CHECK(dialog == &root && !previous); return &button.wnd; }
LRESULT WINAPI wow_native_SendMessageW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)lp;
    if (window == desktop.head.h) return 0;
    if (exhausted) { CHECK(message == WM_GETDLGCODE); ++queries; return 0; }
    if (retired && window == button.wnd.head.h) return 0;
    if (window == root.head.h) { CHECK(message == WM_COMMAND); ++commands; return 0; }
    if (window == start.head.h) { CHECK(message == WM_GETDLGCODE); return DLGC_WANTCHARS; }
    CHECK(window == button.wnd.head.h);
    if (message == WM_GETDLGCODE) {
        if (++queries == destroy_query) destroy();
        return code;
    }
    if (message == BM_SETSTATE) {
        if (destroy_flash == (wp ? 1u : 2u)) destroy();
        return 0;
    }
    CHECK(message == BM_CLICK);
    ++clicks;
    if (destroy_click) destroy();
    return 0;
}
int WINAPI wow_native_GetWindowTextW(HWND window, LPWSTR text, int count)
{
    CHECK((exhausted || window == button.wnd.head.h) && count >= 6);
    if (retired) { text[0] = 0; return 0; }
    ++texts; lstrcpyW(text, L"&Test");
    if (destroy_text) destroy();
    return 5;
}
VOID DlgSetFocus(HWND window)
{
    CHECK(window == button.wnd.head.h);
    if (retired) return;
    ++focuses;
    if (code & DLGC_BUTTON) CHECK(data.buttonState & BST_DONTCLICK);
    if (destroy_focus) {
        destroy();
        if (code & DLGC_BUTTON) {
            wow_button_free_state(&pool, &data);
            CHECK(!frees); /* The actual original caller still owns BUTN. */
        }
    }
    if (throw_focus) RaiseException(FAULT, 0, 0, NULL);
}
static void setup(UINT dialog_code)
{
    ZeroMemory(&root, sizeof(root)); ZeroMemory(&start, sizeof(start));
    ZeroMemory(&button, sizeof(button)); ZeroMemory(&data, sizeof(data));
    root.head.h = (HWND)(ULONG_PTR)1; start.head.h = (HWND)(ULONG_PTR)2;
    desktop.head.h = (HWND)(ULONG_PTR)4; exhausted = FALSE;
    button.wnd.head.h = (HWND)(ULONG_PTR)3;
    button.wnd.fnid = FNID_BUTTON; button.wnd.spwndParent = &root;
    button.wnd.spmenu = (PMENU)(ULONG_PTR)9;
    button.pbutn = &data; data.spwnd = &button.wnd;
    queries = texts = focuses = clicks = commands = frees = 0;
    destroy_query = destroy_flash = 0;
    destroy_text = destroy_focus = throw_focus = destroy_click = retired = FALSE;
    code = dialog_code;
}
static PVOID WINAPI fixture_button_data(struct tagWND *window)
{
    return ((PBUTNWND)window)->pbutn;
}

int __cdecl main(void)
{
    if (!wow_button_bind(fixture_button_data, FreeLookasideEntry)) return 3;
    unsigned i;
    BOOL caught;
    WCHAR label[] = L"  Save && &Close";
    CHECK(FindMnemChar(label, L'c', FALSE, TRUE) == 1);
    CHECK(FindMnemChar(label, L's', TRUE, FALSE) == 0x80);
    for (i = 1; i <= 2; ++i) {
        setup(DLGC_BUTTON); destroy_query = i;
        CHECK(!xxxGotoNextMnem(&root, &start, L't'));
        CHECK(queries == i && !focuses && !clicks && !commands);
    }
    setup(DLGC_BUTTON); destroy_text = TRUE;
    CHECK(xxxGotoNextMnem(&root, &start, L't') == &button.wnd && texts == 1 && !focuses);
    for (i = 1; i <= 2; ++i) {
        setup(DLGC_BUTTON | DLGC_DEFPUSHBUTTON); destroy_flash = i;
        CHECK(xxxGotoNextMnem(&root, &start, L't') == (PWND)1 && !commands);
    }
    setup(DLGC_BUTTON | DLGC_DEFPUSHBUTTON);
    CHECK(xxxGotoNextMnem(&root, &start, L't') == (PWND)1 && commands == 1 && !focuses);
    setup(DLGC_BUTTON);
    CHECK(xxxGotoNextMnem(&root, &start, L't') == &button.wnd);
    CHECK(focuses == 1 && clicks == 1 && !data.buttonState);
    setup(DLGC_BUTTON); destroy_focus = TRUE;
    CHECK(!xxxGotoNextMnem(&root, &start, L't') && frees == 1 && !clicks);
    setup(0); destroy_focus = TRUE;
    CHECK(xxxGotoNextMnem(&root, &start, L't') == &button.wnd && focuses == 1);
    setup(DLGC_BUTTON); destroy_click = TRUE;
    CHECK(xxxGotoNextMnem(&root, &start, L't') == &button.wnd && clicks == 1);
    for (i = 0; i < 2; ++i) {
        setup(DLGC_BUTTON); destroy_focus = i; throw_focus = TRUE; caught = FALSE;
        __try { (void)xxxGotoNextMnem(&root, &start, L't'); }
        __except (GetExceptionCode() == FAULT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
        CHECK(caught && !clicks && frees == i);
        CHECK(i ? data.buttonState == 0xdead : data.buttonState == 0);
    }
    setup(DLGC_BUTTON); /* Verify the exception left no stale scope. */
    CHECK(xxxGotoNextMnem(&root, &start, L't') == &button.wnd && clicks == 1);
    setup(0); exhausted = TRUE;
    ZeroMemory(disabled, sizeof(disabled));
    for (i = 0; i < 513; ++i) {
        disabled[i].head.h = (HWND)(ULONG_PTR)(100 + i);
        disabled[i].spwndParent = &root;
        SetWF(&disabled[i], WFDISABLED);
    }
    CHECK(!xxxGotoNextMnem(&root, &start, L't'));
    CHECK(texts == 512 && queries == 1024 && !focuses && !clicks);
    printf("original-mnemonic-lifetime failures=%u\n", failures);
    return failures ? 1 : 0;
}
