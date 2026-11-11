#include "wow_user_client_view_layout.h"
#include "wow_window_client_state_bindings.h"
#include "wow_default_window_bindings.h"
#include "wow_task_init_bindings.h"
#include <stdio.h>
PWND _GetNextDlgTabItem(PWND, PWND, BOOL);
PWND _GetNextDlgGroupItem(PWND, PWND, BOOL);
PWND _GetDlgItem(PWND, int);
PWND _FindDlgItem(PWND, DWORD);
VOID xxxCheckDefPushButton(PWND, HWND, HWND);
BOOL _AdjustWindowRectEx(LPRECT, LONG, BOOL, DWORD);
BOOL NeedsWindowEdge(DWORD, DWORD, BOOL);
__declspec(thread) wow_default_context *wow_default_current;
static wow_task_client_info client;
const wow_task_client_info *WINAPI wow_edit_client_info(void) { return &client; }
static WND desktop, root, first, container, nested, disabled, last, outside;
static int failures;
static DWORD root_help, first_help, container_help, desktop_help;
static UINT first_code, last_code, style_writes;
static DWORD default_id;
/* Only the unavailable property input is doubled. The linked original
 * help.c, not the test, chooses parent/owner order and desktop termination. */
DWORD WINAPI wow_window_client_context_help(PWND state)
{
    if (state == &root) return root_help;
    if (state == &first) return first_help;
    if (state == &container) return container_help;
    if (state == &desktop) return desktop_help;
    return 0;
}
#define CHECK(x) do { if (!(x)) { printf("FAIL navigation line %d\n", __LINE__); ++failures; } } while (0)
PWND WINAPI wow_window_client_validate_window(HWND window)
{
    PWND nodes[] = {&root, &first, &container, &nested, &last};
    unsigned i;
    for (i = 0; i < ARRAYSIZE(nodes); ++i)
        if (window && nodes[i]->head.h == window) return nodes[i];
    return NULL;
}
LRESULT WINAPI wow_native_SendMessageW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    PWND node = wow_window_client_validate_window(window);
    UINT *code = node == &first ? &first_code : node == &last ? &last_code : NULL;
    CHECK(node != NULL);
    if (message == WM_GETDLGCODE) return code ? *code : 0;
    if (message == DM_GETDEFID) {
        CHECK(node == &root);
        return MAKELONG(default_id, DC_HASDEFID);
    }
    CHECK(message == BM_SETSTYLE && code && lp == TRUE);
    if (message == BM_SETSTYLE && code) {
        CHECK(wp == BS_PUSHBUTTON || wp == BS_DEFPUSHBUTTON);
        *code = wp == BS_DEFPUSHBUTTON ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON;
        ++style_writes;
    }
    return 0;
}
static void child(PWND node, PWND parent)
{
    node->head.pSelf = (PBYTE)node;
    node->style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    node->spwndParent = parent;
}
static void verify_dialog_controls(void)
{
    PWND nodes[] = {&root, &first, &container, &nested, &last};
    unsigned i;
    for (i = 0; i < ARRAYSIZE(nodes); ++i) {
        ZeroMemory(nodes[i], sizeof(*nodes[i]));
        nodes[i]->head.h = (HWND)(ULONG_PTR)(i + 1);
    }
    root.spwndParent = &desktop;
    child(&first, &root); child(&container, &root);
    child(&nested, &container); child(&last, &root);
    root.spwndChild = &first; first.spwndNext = &container;
    container.spwndNext = &last; container.spwndChild = &nested;
    container.ExStyle = WS_EX_CONTROLPARENT;
    first.spmenu = (PMENU)1; last.spmenu = (PMENU)2; nested.spmenu = (PMENU)3;
    CHECK(_GetDlgItem(NULL, 1) == NULL);
    CHECK(_GetDlgItem(&root, 1) == &first);
    CHECK(_GetDlgItem(&root, 3) == NULL);
    CHECK(_FindDlgItem(&root, 3) == NULL); /* Pre-4.0 direct-child contract. */
    SetWF(&root, WFWIN40COMPAT);
    CHECK(_FindDlgItem(&root, 3) == &nested);
    CHECK(_FindDlgItem(&root, 999) == NULL);
    default_id = 1; first_code = DLGC_DEFPUSHBUTTON;
    last_code = DLGC_UNDEFPUSHBUTTON; style_writes = 0;
    xxxCheckDefPushButton(&root, first.head.h, last.head.h);
    CHECK(first_code == DLGC_UNDEFPUSHBUTTON && last_code == DLGC_DEFPUSHBUTTON);
    CHECK(style_writes == 2);
    xxxCheckDefPushButton(&root, last.head.h, nested.head.h);
    CHECK(first_code == DLGC_DEFPUSHBUTTON && last_code == DLGC_UNDEFPUSHBUTTON);
    first.style |= WS_DISABLED;
    first_code = DLGC_UNDEFPUSHBUTTON;
    xxxCheckDefPushButton(&root, NULL, nested.head.h);
    CHECK(first_code == DLGC_UNDEFPUSHBUTTON); /* Disabled default not restyled. */
    first.style &= ~WS_DISABLED;
    first_code = DLGC_DEFPUSHBUTTON;
    xxxCheckDefPushButton(&root, last.head.h, last.head.h);
    CHECK(first_code == DLGC_UNDEFPUSHBUTTON && last_code == DLGC_DEFPUSHBUTTON);
}
static void verify_dialog_groups(void)
{
    PWND nodes[] = {&first, &container, &nested, &last};
    unsigned i;
    ZeroMemory(&root, sizeof(root));
    root.spwndParent = &desktop;
    root.spwndChild = &first;
    for (i = 0; i < ARRAYSIZE(nodes); ++i) {
        ZeroMemory(nodes[i], sizeof(*nodes[i]));
        child(nodes[i], &root);
        nodes[i]->spwndNext = i + 1 < ARRAYSIZE(nodes) ? nodes[i + 1] : NULL;
    }
    first.style |= WS_GROUP;
    nested.style |= WS_DISABLED;
    last.style |= WS_GROUP;
    CHECK(_GetNextDlgGroupItem(&root, &first, FALSE) == &container);
    CHECK(_GetNextDlgGroupItem(&root, &container, FALSE) == &first);
    CHECK(_GetNextDlgGroupItem(&root, &first, TRUE) == &container);
    CHECK(_GetNextDlgGroupItem(&root, &last, FALSE) == &last);
    container.style |= WS_DISABLED;
    CHECK(_GetNextDlgGroupItem(&root, &first, FALSE) == &first);
    container.style &= ~(WS_DISABLED | WS_VISIBLE);
    CHECK(_GetNextDlgGroupItem(&root, &first, TRUE) == &first);
    container.style |= WS_VISIBLE;
    first.style &= ~WS_GROUP;
    last.style &= ~WS_GROUP;
    CHECK(_GetNextDlgGroupItem(&root, &last, FALSE) == &first);
    CHECK(_GetNextDlgGroupItem(&root, &first, TRUE) == &last);
}
static void verify_dialog_geometry(void)
{
    wow_default_context context = {0};
    RECT rect;
    int bx = GetSystemMetrics(SM_CXBORDER), by = GetSystemMetrics(SM_CYBORDER);
    int caption = GetSystemMetrics(SM_CYCAPTION), menu = GetSystemMetrics(SM_CYMENU);
    wow_default_current = &context;
    context.server.gclBorder = 3;
    CHECK(!NeedsWindowEdge(WS_CAPTION, 0, FALSE));
    CHECK(NeedsWindowEdge(WS_CAPTION, 0, TRUE));
    CHECK(NeedsWindowEdge(WS_DLGFRAME, 0, FALSE));
    CHECK(!NeedsWindowEdge(WS_THICKFRAME, WS_EX_STATICEDGE, TRUE));
    CHECK(NeedsWindowEdge(0, WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE, FALSE));
    client.dwExpWinVer = 0x030a;
    SetRect(&rect, 10, 20, 110, 120);
    CHECK(_AdjustWindowRectEx(&rect, WS_CAPTION, TRUE, 0));
    CHECK(rect.left == 10 - bx && rect.right == 110 + bx &&
        rect.top == 20 - caption - menu - by && rect.bottom == 120 + by);
    client.dwExpWinVer = 0x0400;
    SetRect(&rect, 10, 20, 110, 120);
    CHECK(_AdjustWindowRectEx(&rect, WS_CAPTION | WS_THICKFRAME, FALSE, WS_EX_CLIENTEDGE));
    CHECK(rect.left == 10 - 8 * bx && rect.right == 110 + 8 * bx &&
        rect.top == 20 - caption - 8 * by && rect.bottom == 120 + 8 * by);
    SetRect(&rect, 10, 20, 110, 120);
    CHECK(_AdjustWindowRectEx(&rect, WS_CAPTION, FALSE, WS_EX_TOOLWINDOW));
    CHECK(rect.top == 20 - GetSystemMetrics(SM_CYSMCAPTION) - 3 * by);
    wow_default_current = NULL;
}
int __cdecl main(void)
{
    /* Actual original traversal over a retained, consistent synthetic graph.
     * No HWND validator double or host navigation API decides the result.
     * This does not prove production graph publication/lifetime. */
    desktop.fnid = FNID_DESKTOP;
    root.spwndParent = &desktop;
    root.ExStyle = WS_EX_CONTROLPARENT;
    CHECK(_GetNextDlgTabItem(&root, NULL, FALSE) == &root);
    CHECK(_GetNextDlgTabItem(&root, NULL, TRUE) == NULL);
    child(&first, &root); child(&container, &root);
    child(&nested, &container); child(&disabled, &root);
    child(&last, &root); child(&outside, &desktop);
    root.spwndChild = &first;
    first.spwndNext = &container;
    container.ExStyle = WS_EX_CONTROLPARENT;
    container.spwndChild = &nested;
    container.spwndNext = &disabled;
    disabled.style |= WS_DISABLED;
    disabled.spwndNext = &last;
    CHECK(_GetNextDlgTabItem(&root, NULL, FALSE) == &first);
    root_help = 101; first_help = 202; container_help = 303;
    desktop_help = 404;
    CHECK(GetContextHelpId(&first) == 202);
    first_help = 0;
    CHECK(GetContextHelpId(&first) == 101);
    CHECK(GetContextHelpId(&nested) == 303);
    container_help = 0;
    CHECK(GetContextHelpId(&nested) == 101);
    outside.style = WS_POPUP;
    outside.spwndOwner = &root;
    CHECK(GetContextHelpId(&outside) == 101);
    /* Mixed WS_CHILD|WS_POPUP follows TestwndChild, not a raw child-bit test. */
    outside.style |= WS_CHILD;
    outside.spwndParent = &first;
    first_help = 202;
    CHECK(GetContextHelpId(&outside) == 101);
    outside.spwndOwner = NULL;
    CHECK(GetContextHelpId(&outside) == 0);
    root_help = 0;
    root.spwndOwner = &desktop;
    CHECK(GetContextHelpId(&nested) == 0);
    CHECK(GetContextHelpId(&desktop) == 404);
    child(&outside, &desktop);
    root.spwndOwner = NULL;
    CHECK(_GetNextDlgTabItem(&root, &first, FALSE) == &nested);
    CHECK(_GetNextDlgTabItem(&root, &nested, FALSE) == &last);
    CHECK(_GetNextDlgTabItem(&root, &last, FALSE) == &first);
    CHECK(_GetNextDlgTabItem(&root, &first, TRUE) == &last);
    CHECK(_GetNextDlgTabItem(&root, &last, TRUE) == &nested);
    CHECK(_GetNextDlgTabItem(&root, &outside, FALSE) == NULL);
    container.style &= ~WS_VISIBLE;
    CHECK(_GetNextDlgTabItem(&root, &first, FALSE) == &last);
    container.style |= WS_VISIBLE | WS_DISABLED;
    CHECK(_GetNextDlgTabItem(&root, &last, TRUE) == &first);
    /* No eligible tabstop terminates at the original sentinel, rather than
     * spinning or returning a fabricated fallback HWND. */
    first.style &= ~WS_TABSTOP; last.style &= ~WS_TABSTOP;
    CHECK(_GetNextDlgTabItem(&root, NULL, FALSE) == &first);
    verify_dialog_controls();
    verify_dialog_groups();
    verify_dialog_geometry();
    if (!failures) puts("WOW_DIALOG_NAVIGATION_OK");
    return failures ? 1 : 0;
}
