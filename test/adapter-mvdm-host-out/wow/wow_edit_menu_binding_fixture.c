#define WOW_ORIGINAL_EDIT_CLIENT
#include "wow_edit_menu_bindings.h"
#include <stdio.h>

/* Compile-boundary checks only: these doubles do not prove original ECMenu
 * policy, native popup interaction or guest menu acceptance. */
HANDLE hmodWOW32 = (HANDLE)(ULONG_PTR)0x12340000;
static unsigned failures, loads, tracks, destroys, sends;
static BOOL fail_load;
static int command = WM_CUT;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)
#define MENU_HANDLE ((HMENU)(ULONG_PTR)0x101)
#define OWNER_WINDOW ((HWND)(ULONG_PTR)0x202)

HMENU WINAPI wow_user_load_menuW(HINSTANCE module, LPCWSTR name)
{
    ++loads;
    CHECK(module == (HINSTANCE)hmodWOW32 && (ULONG_PTR)name == 1);
    SetLastError(701);
    return fail_load ? NULL : MENU_HANDLE;
}

BOOL WINAPI wow_menu_native_TrackPopupMenuEx(HMENU menu, UINT flags,
    int x, int y, HWND window, LPTPMPARAMS parameters)
{
    ++tracks;
    CHECK(menu == MENU_HANDLE && window == OWNER_WINDOW);
    CHECK(flags == (TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON));
    CHECK(x == -17 && y == 29 && parameters == NULL);
    SetLastError(702);
    return command;
}

BOOL WINAPI wow_menu_native_DestroyMenu(HMENU menu)
{
    ++destroys;
    CHECK(menu == MENU_HANDLE);
    return TRUE;
}

LRESULT WINAPI wow_native_SendMessageW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    ++sends;
    CHECK(window == OWNER_WINDOW && message == EM_SETSEL && wp == 0);
    CHECK((DWORD)lp == 0xffffffff);
    return 73;
}

int __cdecl main(void)
{
    CHECK(LoadMenu(hmodUser, MAKEINTRESOURCEW(1)) == MENU_HANDLE);
    CHECK(GetLastError() == 701 && loads == 1);
    fail_load = TRUE;
    CHECK(LoadMenu(hmodUser, MAKEINTRESOURCEW(1)) == NULL);
    CHECK(loads == 2 && tracks == 0);
    CHECK(NtUserTrackPopupMenuEx(MENU_HANDLE,
        TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON,
        -17, 29, OWNER_WINDOW, NULL) == WM_CUT);
    CHECK(GetLastError() == 702);
    command = -1;
    CHECK(NtUserTrackPopupMenuEx(MENU_HANDLE,
        TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON,
        -17, 29, OWNER_WINDOW, NULL) == -1);
    CHECK(NtUserDestroyMenu(MENU_HANDLE) && destroys == 1);
    CHECK(SendMessage(OWNER_WINDOW, EM_SETSEL, 0, (LPARAM)0xffffffff) == 73);
    CHECK(sends == 1 && tracks == 2);
    printf("edit_menu_binding_failures=%u\n", failures);
    return failures != 0;
}
