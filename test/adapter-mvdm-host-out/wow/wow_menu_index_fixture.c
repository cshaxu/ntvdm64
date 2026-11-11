#include <windows.h>
#include <stdio.h>
#include "wow_private_user_compat.h"

/* Native boundary fixture; no guest or desktop window is created. */
int main(void)
{
    HMENU menu = CreateMenu();
    HMENU child = CreatePopupMenu();
    HMENU grandchild = CreatePopupMenu();
    HMENU unrelated = CreatePopupMenu();
    HMENU narrow;
    DWORD value, error;
    int failed = 0;
    if (!menu || !child || !grandchild || !unrelated) return 2;
    if (!AppendMenuA(menu, MF_STRING, 1, "item") ||
        !AppendMenuA(menu, MF_POPUP, (UINT_PTR)child, "child") ||
        !AppendMenuA(child, MF_POPUP, (UINT_PTR)grandchild, "grandchild"))
        return 3;

#define CHECK(label, expression) do { \
    int ok = !!(expression); \
    printf("%s=%d\n", label, ok); \
    if (!ok) ++failed; \
} while (0)
    CHECK("immediate_index", wow_private_user_get_menu_index(menu, child) == 1);
    CHECK("not_recursive", wow_private_user_get_menu_index(menu, grandchild) == (DWORD)-1);
    CHECK("absent", wow_private_user_get_menu_index(menu, unrelated) == (DWORD)-1);
    CHECK("self_absent", wow_private_user_get_menu_index(menu, menu) == (DWORD)-1);
    narrow = (HMENU)(INT_PTR)(SHORT)(WORD)(UINT_PTR)child;
    CHECK("original_narrow_submenu", wow_private_user_get_menu_index(menu, narrow) == 1);
    narrow = (HMENU)(INT_PTR)(SHORT)(WORD)(UINT_PTR)menu;
    CHECK("original_narrow_parent", wow_private_user_get_menu_index(narrow, child) == 1);
    SetLastError(0);
    value = wow_private_user_get_menu_index(NULL, child);
    error = GetLastError();
    CHECK("invalid_parent", value == 0 && error == ERROR_INVALID_MENU_HANDLE);
    SetLastError(0);
    value = wow_private_user_get_menu_index(menu, NULL);
    error = GetLastError();
    CHECK("invalid_child", value == 0 && error == ERROR_INVALID_MENU_HANDLE);
    CHECK("detach", RemoveMenu(menu, 1, MF_BYPOSITION));
    CHECK("detached_absent", wow_private_user_get_menu_index(menu, child) == (DWORD)-1);
    CHECK("destroy_child_tree", DestroyMenu(child));
    SetLastError(0);
    value = wow_private_user_get_menu_index(menu, child);
    error = GetLastError();
    CHECK("destroyed_child", value == 0 && error == ERROR_INVALID_MENU_HANDLE);
    CHECK("destroy_menu", DestroyMenu(menu));
    CHECK("destroy_unrelated", DestroyMenu(unrelated));
    if (!failed) puts("WOW_MENU_INDEX_BOUNDARY_OK");
    return failed ? 1 : 0;
}
