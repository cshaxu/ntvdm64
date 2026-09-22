#include "wow_menu_resource_bindings.h"
#include "wow_private_user_compat.h"
#include <stdio.h>
#include <string.h>

/* Controlled resource callbacks; real original USER client and native menu
 * construction. This is not a guest resource/CPU acceptance test. */
static unsigned errors, step, fail_at;
static HINSTANCE module = (HINSTANCE)0x12341234;
static BOOL string_name;
static WORD resource[] = {0, 0, MF_END, 23, 'T', 'e', 's', 't', 0};
static WORD owner_resource[] = {0, 0, MF_END | MF_OWNERDRAW, 23, 0x5678, 0x1234, 0};
static BOOL owner_probe;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
HANDLE APIENTRY W32FindResource(HANDLE mod, LPCSTR type, LPCSTR name, WORD lang)
{
#ifdef WOW_RESOURCE_CALLBACKS_ONLY
    RaiseException(0xe0420036, EXCEPTION_NONCONTINUABLE, 0, NULL);
#endif
    CHECK(++step == 1 && mod == module && (ULONG_PTR)type == 4 && lang == 0);
    if (string_name) CHECK(HIWORD(name) && !strcmp(name, "MENU"));
    else CHECK((ULONG_PTR)name == 7);
    return fail_at == 1 ? NULL : (HANDLE)11;
}
HANDLE APIENTRY W32LoadResource(HANDLE mod, HANDLE info)
{
#ifdef WOW_RESOURCE_CALLBACKS_ONLY
    RaiseException(0xe0420036, EXCEPTION_NONCONTINUABLE, 0, NULL);
#endif
    CHECK(++step == 2 && mod == module && info == (HANDLE)11);
    return fail_at == 2 ? NULL : (HANDLE)12;
}
LPSTR APIENTRY W32LockResource(HANDLE data, HANDLE mod)
{
#ifdef WOW_RESOURCE_CALLBACKS_ONLY
    RaiseException(0xe0420036, EXCEPTION_NONCONTINUABLE, 0, NULL);
#endif
    CHECK(++step == 3 && mod == module && data == (HANDLE)12);
    return fail_at == 3 ? NULL : (LPSTR)(owner_probe ? owner_resource : resource);
}
BOOL APIENTRY W32UnlockResource(HANDLE data, HANDLE mod)
{
#ifdef WOW_RESOURCE_CALLBACKS_ONLY
    RaiseException(0xe0420036, EXCEPTION_NONCONTINUABLE, 0, NULL);
#endif
    CHECK(++step == 4 && mod == module && data == (HANDLE)12);
    return TRUE;
}
#ifndef WOW_RESOURCE_CALLBACKS_ONLY
static void parser_cases(void)
{
    WORD popup[] = {0,0, MF_POPUP | MF_END,'P',0, MF_END,31,'C',0};
    WORD empty_owner[] = {0,0,MF_OWNERDRAW | MF_END,41,0};
    DWORD invalid[] = {0x00040001,0,0x80000000,0,23,0x80,0};
    DWORD invalid_child[] = {0x00040001,0,0,0,23,0x00500081,0,
        0,0x80000000,0,24,0x80,0};
    WORD version[] = {2,0};
    /* MENUEX version 1: header skips the root help ID; fields aligned DWORD. */
    DWORD extended[] = {0x00040001,77,0,0,51,0x00580080,0};
    MENUITEMINFOW item = {0};
    HMENU menu, child;
    unsigned repeat;
    menu = CreateMenuFromResource((LPBYTE)popup, TRUE);
    CHECK(menu != NULL);
    if (menu) {
        child = GetSubMenu(menu, 0);
        CHECK(child && GetMenuItemID(child, 0) == 31);
        CHECK(DestroyMenu(menu));
    }
    menu = CreateMenuFromResource((LPBYTE)empty_owner, TRUE);
    CHECK(menu != NULL);
    item.cbSize = sizeof(item); item.fMask = MIIM_DATA | MIIM_FTYPE;
    if (menu) {
        CHECK(GetMenuItemInfoW(menu, 0, TRUE, &item));
        CHECK(item.dwItemData == 0 && (item.fType & MFT_OWNERDRAW));
        CHECK(DestroyMenu(menu));
    }
    menu = CreateMenuFromResource((LPBYTE)owner_resource, FALSE);
    CHECK(menu != NULL);
    if (menu) {
        CHECK(GetMenuItemInfoW(menu, 0, TRUE, &item));
        CHECK(item.dwItemData == (ULONG_PTR)&owner_resource[4]);
        CHECK(DestroyMenu(menu));
    }
    menu = CreateMenuFromResource((LPBYTE)extended, FALSE);
    CHECK(menu != NULL);
    if (menu) {
        CHECK(GetMenuItemID(menu, 0) == 51);
        CHECK(DestroyMenu(menu));
    }
    CHECK(CreateMenuFromResource((LPBYTE)version, TRUE) == NULL);
    for (repeat = 0; repeat != 64; ++repeat) {
        SetLastError(0);
        CHECK(CreateMenuFromResource((LPBYTE)invalid, TRUE) == NULL);
        CHECK(GetLastError() == ERROR_INVALID_DATA);
        CHECK(CreateMenuFromResource((LPBYTE)invalid_child, TRUE) == NULL);
    }
}

int __cdecl main(int argc, char **argv)
{
    unsigned wide, named, failure;
    if (argc == 2 && !strcmp(argv[1], "--owner-draw")) {
        MENUITEMINFOW item = {0};
        HMENU menu;
        /* Original ConvertMenuItems16 writes a packed guest pointer, not a
         * Unicode string. Trailing zero bounds a native-parser mismatch. */
        owner_probe = TRUE;
        menu = wow_user_load_menuW(module, MAKEINTRESOURCEW(7));
        CHECK(menu != NULL && step == 4);
        item.cbSize = sizeof(item);
        item.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID;
        if (menu) {
            CHECK(GetMenuItemInfoW(menu, 0, TRUE, &item));
            printf("OWNER_DRAW expected=12345678 actual=%08lX type=%08X id=%u\n",
                (DWORD)item.dwItemData, item.fType, item.wID);
            CHECK(item.dwItemData == 0x12345678 &&
                (item.fType & MFT_OWNERDRAW) && item.wID == 23);
            CHECK(DestroyMenu(menu));
        }
        menu = wow_user_load_menu_indirect((const MENUTEMPLATEW *)owner_resource);
        CHECK(menu != NULL);
        if (menu) {
            CHECK(GetMenuItemInfoW(menu, 0, TRUE, &item));
            CHECK(item.dwItemData == 0x12345678);
            CHECK(DestroyMenu(menu));
        }
        menu = wow_private_user_load_create_menu(module, MAKEINTRESOURCE(7),
            (LPMENUTEMPLATE)owner_resource, sizeof(owner_resource), FALSE);
        CHECK(menu != NULL);
        if (menu) {
            CHECK(GetMenuItemInfoW(menu, 0, TRUE, &item));
            CHECK(item.dwItemData == 0x12345678);
            CHECK(DestroyMenu(menu));
        }
        printf("WOW_OWNER_DRAW_RESOURCE errors=%u\n", errors);
        return errors != 0;
    }
    for (wide = 0; wide != 2; ++wide) {
        for (named = 0; named != 2; ++named) {
            for (failure = 0; failure != 4; ++failure) {
                HMENU menu;
                WCHAR text[16];
                step = 0; fail_at = failure; string_name = named != 0;
                menu = wide ? wow_user_load_menuW(module, named ? L"MENU" : MAKEINTRESOURCEW(7)) :
                    wow_user_load_menuA(module, named ? "MENU" : MAKEINTRESOURCEA(7));
                CHECK(step == (failure ? failure : 4));
                if (failure) CHECK(menu == NULL);
                else {
                    CHECK(menu != NULL);
                    if (menu) {
                        CHECK(GetMenuItemCount(menu) == 1 && GetMenuItemID(menu, 0) == 23);
                        CHECK(GetMenuStringW(menu, 0, text, 16, MF_BYPOSITION) == 4);
                        CHECK(!lstrcmpW(text, L"Test"));
                        CHECK(DestroyMenu(menu));
                    }
                }
            }
        }
    }
    parser_cases();
    printf("WOW_ORIGINAL_MENU_RESOURCE errors=%u resource_cases=16 parser_cases=7 invalid_repeats=64\n", errors);
    return errors != 0;
}
#endif
