#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

static UINT drive_type;
static unsigned drive_calls, remove_calls;
static int add_result = 1;
static BOOL remove_result = TRUE, fail_allocation;
static unsigned search_calls, fonts_calls;
static BOOL fonts_hit = TRUE, search_failure;
static WCHAR last_removed[MAX_PATH];

static UINT WINAPI fixture_drive(LPCWSTR path)
{
    (void)path;
    ++drive_calls;
    return drive_type;
}
static int WINAPI fixture_add(LPCWSTR path)
{
    (void)path;
    return add_result;
}
static BOOL WINAPI fixture_remove(LPCWSTR path)
{
    lstrcpynW(last_removed, path, MAX_PATH);
    ++remove_calls;
    return remove_result;
}
#ifndef WOW_FONT_NATIVE_SEARCH
static DWORD WINAPI fixture_search(LPCWSTR directory, LPCWSTR name,
    LPCWSTR extension, DWORD capacity, LPWSTR output, LPWSTR *part)
{
    (void)extension;
    (void)part;
    ++search_calls;
    if (directory) ++fonts_calls;
    if (search_failure || (directory && !fonts_hit)) return 0;
    if (name[0] == L'\\' || name[1] == L':')
        return GetFullPathNameW(name, capacity, output, NULL);
    lstrcpynW(output, directory ? L"\\\\fixture-host\\fonts\\\x4e2d.fon" :
        L"\\\\fixture-host\\fallback\\\x4e2d.fon", (int)capacity);
    return (DWORD)lstrlenW(output);
}
#endif
static HLOCAL WINAPI fixture_alloc(UINT flags, SIZE_T size)
{
    return fail_allocation ? NULL : LocalAlloc(flags, size);
}

/* Real adapter/list/locking and ACP conversion; deterministic search results
 * avoid requiring installed network fonts. No host font is changed. */
#ifndef WOW_FONT_NATIVE_SEARCH
#define SearchPathW fixture_search
#endif
#define GetDriveTypeW fixture_drive
#define AddFontResourceW fixture_add
#define RemoveFontResourceW fixture_remove
#define LocalAlloc fixture_alloc
#include "../../../src/wow32-dll/source/wow_public_user_facade.c"
#undef LocalAlloc
#undef RemoveFontResourceW
#undef AddFontResourceW
#undef GetDriveTypeW
#undef SearchPathW

#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)

int main(void)
{
#ifdef WOW_FONT_NATIVE_SEARCH
    WCHAR directory[MAX_PATH], pattern[MAX_PATH], expected[MAX_PATH], actual[MAX_PATH];
    WIN32_FIND_DATAW data;
    HANDLE find;
    UINT length = GetWindowsDirectoryW(directory, MAX_PATH);
    CHECK(length && length + 12 < MAX_PATH);
    if (directory[length - 1] == L'\\') directory[--length] = 0;
    lstrcatW(directory, L"\\fonts");
    lstrcpyW(pattern, directory);
    lstrcatW(pattern, L"\\*.ttf");
    find = FindFirstFileW(pattern, &data);
    CHECK(find != INVALID_HANDLE_VALUE);
    FindClose(find);
    CHECK(SearchPathW(directory, data.cFileName, NULL, MAX_PATH, expected, NULL));
    drive_type = DRIVE_REMOTE;
    CHECK(wow_network_font_is_remote(data.cFileName, actual));
    CHECK(_wcsicmp(actual, expected) == 0);
    CHECK(wow_network_font_is_remote(expected, actual));
    CHECK(_wcsicmp(actual, expected) == 0);
    CHECK(!wow_network_font_is_remote(L"ntvdm-s40-font-does-not-exist-7219.fon", actual));
    puts("WOW_FONT_NATIVE_READ_ONLY_SEARCH_OK");
    return 0;
#else
    WCHAR full[MAX_PATH];
    UINT kind;
    const char *unc = "\\\\fixture-host\\fonts\\fixture.fon";
    for (kind = DRIVE_UNKNOWN; kind <= DRIVE_RAMDISK; ++kind) {
        BOOL expected = kind != DRIVE_REMOVABLE && kind != DRIVE_FIXED &&
            kind != DRIVE_CDROM && kind != DRIVE_RAMDISK;
        drive_type = kind;
        drive_calls = 0;
        CHECK(wow_network_font_is_remote(L"X:\\fixture.fon", full) == expected);
        CHECK(drive_calls == 1);
        drive_calls = 0;
        CHECK(wow_network_font_is_remote(L"\\\\fixture-host\\fonts\\fixture.fon", full));
        CHECK(drive_calls == 0);
    }
    puts("WOW_FONT_ORIGINAL_DRIVE_CLASSIFICATION_OK");
    CHECK(AddFontResourceTracking(unc, 1) == 1);
    CHECK(AddFontResourceTracking("\\\\FIXTURE-HOST\\FONTS\\FIXTURE.FON", 1) == 1);
    CHECK(AddFontResourceTracking(unc, 2) == 1);
    CHECK(wow_network_fonts && wow_network_fonts->owner_id == 2);
    CHECK(wow_network_fonts->next && wow_network_fonts->next->load_count == 2);
    remove_result = FALSE;
    CHECK(!RemoveFontResourceTracking(unc, 1));
    CHECK(wow_network_fonts->next->load_count == 2);
    remove_result = TRUE;
    CHECK(RemoveFontResourceTracking(unc, 1));
    CHECK(wow_network_fonts->next->load_count == 1);
    remove_calls = 0;
    UnloadNetworkFonts(1);
    CHECK(remove_calls == 1 && wow_network_fonts->owner_id == 2);
    UnloadNetworkFonts(1);
    CHECK(remove_calls == 1);
    UnloadNetworkFonts(2);
    CHECK(remove_calls == 2 && wow_network_fonts == NULL);
    CHECK(AddFontResourceTracking(unc, 3) == 1);
    CHECK(AddFontResourceTracking(unc, 3) == 1);
    remove_calls = 0;
    UnloadNetworkFonts(3);
    CHECK(remove_calls == 2 && wow_network_fonts == NULL);
    add_result = 0;
    CHECK(AddFontResourceTracking(unc, 4) == 0 && wow_network_fonts == NULL);
    add_result = 1;
    fail_allocation = TRUE;
    CHECK(AddFontResourceTracking(unc, 4) == 1 && wow_network_fonts == NULL);
    fail_allocation = FALSE;
    remove_calls = 0;
    UnloadNetworkFonts(4);
    CHECK(remove_calls == 0);
    drive_type = DRIVE_FIXED;
    CHECK(AddFontResourceTracking("X:\\fixture.fon", 5) == 1);
    CHECK(wow_network_fonts == NULL);
    puts("WOW_FONT_TASK_REFERENCE_AND_FAILURE_CONTRACT_OK");
    drive_type = DRIVE_REMOTE;
    search_calls = fonts_calls = 0;
    CHECK(AddFontResourceTracking("relative.fon", 6) == 1);
    CHECK(search_calls == 1 && fonts_calls == 1);
    CHECK(wow_network_fonts && wcsstr(wow_network_fonts->path, L"\\fonts\\\x4e2d.fon"));
    UnloadNetworkFonts(6);
    CHECK(wcscmp(last_removed, L"\\\\fixture-host\\fonts\\\x4e2d.fon") == 0);
    fonts_hit = FALSE;
    search_calls = fonts_calls = 0;
    CHECK(AddFontResourceTracking("relative.fon", 7) == 1);
    CHECK(search_calls == 2 && fonts_calls == 1);
    UnloadNetworkFonts(7);
    CHECK(wcscmp(last_removed, L"\\\\fixture-host\\fallback\\\x4e2d.fon") == 0);
    search_failure = TRUE;
    CHECK(AddFontResourceTracking("missing.fon", 8) == 1);
    CHECK(wow_network_fonts == NULL);
    puts("WOW_FONT_SEARCH_ORDER_AND_UNICODE_UNLOAD_OK");
    return 0;
#endif
}
