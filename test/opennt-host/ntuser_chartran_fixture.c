#include <windows.h>

int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);

static int expect(int condition, int code)
{
    return condition ? 0 : code;
}

int main(void)
{
    WCHAR wide_storage[8] = { 0 };
    LPWSTR wide = wide_storage;
    WCHAR *allocated_wide = NULL;
    CHAR ansi_storage[8] = { 0 };
    LPSTR ansi = ansi_storage;
    CHAR *allocated_ansi = NULL;
    int result;

    result = expect(MBToWCSEx(CP_ACP, "abc", -1, &allocated_wide, -1,
        TRUE) == 4 && allocated_wide[0] == L'a' &&
        allocated_wide[3] == L'\0', 1);
    if (result != 0) return result;
    LocalFree(allocated_wide);

    result = expect(MBToWCSEx(65001, "abc", -1, &wide, 4, FALSE) == 4 &&
        wide[2] == L'c' && wide[3] == L'\0', 2);
    if (result != 0) return result;
    /* USER's ACP RTL conversion returns the produced prefix when its fixed
     * destination is short; this differs from a generic all-or-nothing
     * facade and is why this source owner is retained. */
    result = expect(MBToWCSEx(CP_ACP, "abc", -1, &wide, 2, FALSE) == 2 &&
        wide[0] == L'a' && wide[1] == L'b', 3);
    if (result != 0) return result;
    result = expect(MBToWCSEx(CP_ACP, "abc", -2, &wide, 4, FALSE) == 0,
        4);
    if (result != 0) return result;

    result = expect(WCSToMBEx(CP_ACP, L"abc", -1, &allocated_ansi, -1,
        TRUE) == 4 && allocated_ansi[0] == 'a' &&
        allocated_ansi[3] == '\0', 5);
    if (result != 0) return result;
    LocalFree(allocated_ansi);

    result = expect(WCSToMBEx(65001, L"abc", -1, &ansi, 4, FALSE) == 4 &&
        ansi[2] == 'c' && ansi[3] == '\0', 6);
    if (result != 0) return result;
    result = expect(WCSToMBEx(CP_ACP, L"abc", -1, &ansi, 2, FALSE) == 2 &&
        ansi[0] == 'a' && ansi[1] == 'b', 7);
    if (result != 0) return result;

    return 0;
}
