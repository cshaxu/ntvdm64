#include "edit-view/precomp.h"
#include <stdio.h>
#include <stdlib.h>
/* Test-owned native window setup/cleanup, outside the product call scope. */
#undef EnableWindow
#undef DestroyWindow

/* Compile the three actual OPENNT-HOST-078 units with the isolated
 * edit-view precomp and /Gz. Synthetic WND relationships are supplemental;
 * this fixture does not prove production parent/view lifetime. */
int view_fixture_CXMINIMIZED = 90, view_fixture_CYMINIMIZED = 30;
static WND desktop, parent, leaf;
static unsigned failures;
__declspec(thread) wow_edit_context *wow_edit_current;
PWND WINAPI wow_window_client_desktop(void) { return &desktop; }
/* These restored sibling helpers are outside this relationship fixture. */
/* Monitor geometry is exercised by the navigation fixture, not this one. */
MONITORINFO *WINAPI wow_control_monitor(HWND window, DWORD flags)
{ (void)window; (void)flags; abort(); }
BOOL WINAPI wow_edit_native_ensure_gui_thread(void) { abort(); }
const struct wow_task_client_info *WINAPI wow_edit_client_info(void) { abort(); }
int WINAPI GetWindowBorders(LONG style, DWORD ex, BOOL window, BOOL client)
{ (void)style; (void)ex; (void)window; (void)client; abort(); }
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line %d\n", __LINE__); } } while (0)
PWND WINAPI view_fixture_validate(HWND hwnd)
{
    return hwnd == (HWND)(ULONG_PTR)1 ? &leaf : NULL;
}

PWND _GetWindow(PWND, UINT);
UINT _GetMenuState(PMENU, UINT, UINT);
static HWND popup_handle;
static WND popup_state;
static PWND popup_owner;
PWND WINAPI wow_window_client_validate_window_no_rip(HWND window)
{
    if (window && window == popup_handle) return &popup_state;
    return window && popup_owner && window == popup_owner->head.h ? popup_owner : NULL;
}
static void verify_xp_menu_and_popup(void)
{
    MENU menu = {0};
    ITEM item = {0};
    WND owner = {0};
    HWND host_owner;
    menu.head.pSelf = (PBYTE)&menu;
    menu.cItems = 1;
    menu.rgItems = &item;
    item.wID = 42;
    item.hbmp = (HBITMAP)(ULONG_PTR)1;
    CHECK((_GetMenuState(&menu, 42, MF_BYCOMMAND) & MF_BITMAP) != 0);
    item.lpstr = L"named";
    CHECK((_GetMenuState(&menu, 42, MF_BYCOMMAND) & MF_BITMAP) == 0);
    CHECK(_GetMenuState(&menu, 99, MF_BYCOMMAND) == (UINT)-1);
    host_owner = CreateWindowExW(0, L"STATIC", L"XP popup owner", WS_OVERLAPPED,
        0, 0, 20, 20, NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(host_owner != NULL);
    if (!host_owner) return;
    popup_handle = CreateWindowExW(0, L"STATIC", L"XP owned popup", WS_POPUP,
        0, 0, 10, 10, host_owner, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(popup_handle != NULL);
    owner.head.pSelf = (PBYTE)&owner;
    owner.head.h = host_owner;
    popup_owner = &owner;
    popup_state.head.pSelf = (PBYTE)&popup_state;
    popup_state.head.h = popup_handle;
    if (popup_handle) {
        HWND selected = GetWindow(host_owner, GW_ENABLEDPOPUP);
        CHECK(_GetWindow(&owner, GW_ENABLEDPOPUP) ==
            wow_window_client_validate_window_no_rip(selected));
        EnableWindow(popup_handle, FALSE);
        selected = GetWindow(host_owner, GW_ENABLEDPOPUP);
        CHECK(_GetWindow(&owner, GW_ENABLEDPOPUP) ==
            wow_window_client_validate_window_no_rip(selected));
        DestroyWindow(popup_handle);
        popup_handle = NULL;
    }
    DestroyWindow(host_owner);
    popup_owner = NULL;
}

int __cdecl main(void)
{
    RECT result;
    POINT point = {-23, 45};
    desktop.fnid = FNID_DESKTOP;
    parent.spwndParent = &desktop;
    leaf.spwndParent = &parent;
    SetWF(&desktop, WFVISIBLE);
    SetWF(&parent, WFVISIBLE);
    SetWF(&leaf, WFVISIBLE | WFCHILD);
    CHECK(_IsChild(&parent, &leaf));
    CHECK(!_IsChild(&leaf, &leaf));
    CHECK(!_IsChild(&parent, NULL));
    CHECK(GetTopLevelWindow(&leaf) == &parent);
    CHECK(GetTopLevelWindow(NULL) == NULL);
    CHECK(IsVisible(&leaf));
    SetWF(&parent, WFMINIMIZED);
    CHECK(!IsVisible(&leaf));
    CHECK(IsVisible(&parent)); /* Own minimized state is not ancestor hiding. */
    ClrWF(&parent, WFMINIMIZED);
    leaf.rcClient.left = -100; leaf.rcClient.top = 200;
    _ClientToScreen(&leaf, &point);
    CHECK(point.x == -123 && point.y == 245);
    _ScreenToClient(&leaf, &point);
    CHECK(point.x == -23 && point.y == 45);
    leaf.rcClient.right = 300;
    SetWF(&leaf, WEFLAYOUTRTL);
    _ClientToScreen(&leaf, &point);
    CHECK(point.x == 323 && point.y == 245);
    _ScreenToClient(&leaf, &point);
    CHECK(point.x == -23 && point.y == 45);
    ClrWF(&leaf, WEFLAYOUTRTL);
    _ClientToScreen(&desktop, &point);
    CHECK(point.x == -23 && point.y == 45);
    _ScreenToClient(&desktop, &point);
    CHECK(point.x == -23 && point.y == 45);
    _GetClientRect(&desktop, &result);
    CHECK(result.left == 0 && result.top == 0 &&
        result.right == GetSystemMetrics(SM_CXSCREEN) &&
        result.bottom == GetSystemMetrics(SM_CYSCREEN));
    _GetWindowRect(&desktop, &result);
    CHECK(result.left == 0 && result.top == 0 &&
        result.right == GetSystemMetrics(SM_CXSCREEN) &&
        result.bottom == GetSystemMetrics(SM_CYSCREEN));
    leaf.rcWindow = (RECT){-120, 180, 320, 450};
    _GetWindowRect(&leaf, &result);
    CHECK(EqualRect(&result, &leaf.rcWindow));
    CHECK(_IsWindowVisible(NULL));
    CHECK(_IsWindowVisible(&leaf));
    CHECK(_FChildVisible(&leaf));
    ClrWF(&leaf, WFVISIBLE);
    CHECK(!_IsWindowVisible(&leaf));
    CHECK(_FChildVisible(&leaf)); /* Deliberately ignores own visibility. */
    ClrWF(&parent, WFVISIBLE);
    CHECK(!_FChildVisible(&leaf));
    SetWF(&leaf, WFPOPUP);
    CHECK(_FChildVisible(&leaf)); /* CHILD|POPUP is not TestwndChild. */
    ClrWF(&leaf, WFPOPUP);
    CHECK(!FChildVisible((HWND)(ULONG_PTR)1));
    CHECK(!FChildVisible(NULL));
    leaf.spwndParent = NULL;
    CHECK(_FChildVisible(&leaf)); /* Original tolerates absent parent. */
    leaf.rcClient.left = 13; leaf.rcClient.top = 17;
    leaf.rcClient.right = 70013; leaf.rcClient.bottom = 90017;
    _GetClientRect(&leaf, &result);
    CHECK(result.left == 0 && result.top == 0 && result.right == 70000 && result.bottom == 90000);
    SetWF(&leaf, WFMINIMIZED);
    _GetClientRect(&leaf, &result);
    CHECK(result.right == 90 && result.bottom == 30);
    SetWF(&leaf, WFWIN40COMPAT);
    _GetClientRect(&leaf, &result);
    CHECK(result.right == 70000 && result.bottom == 90000);
    verify_xp_menu_and_popup();
    printf("original_edit_view_failures=%u\n", failures);
    return failures != 0;
}
