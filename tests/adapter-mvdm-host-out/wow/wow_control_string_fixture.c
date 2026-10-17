#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "wow_edit_native_bindings.h"
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
VOID RtlInitLargeUnicodeString(PLARGE_UNICODE_STRING, LPCWSTR, UINT);
VOID RtlInitLargeAnsiString(PLARGE_ANSI_STRING, LPCSTR, UINT);
DWORD FindCharPosition(LPWSTR, WCHAR);
UINT TextCopy(PLARGE_UNICODE_STRING, LPWSTR, UINT);
DWORD wcsncpycch(LPWSTR, LPCWSTR, DWORD);
LONG GetPrefixCount(LPCWSTR, int, LPWSTR, int);
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
BOOL RtlMBMessageWParamCharToWCS(DWORD, PDWORD);
USHORT WINAPI OpenNtUserThreadCodePage(VOID);
static int failures;
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); ++failures; } } while (0)
static void message_character_contract(void)
{
    static const UINT messages[] = { WM_CHAR, WM_CHARTOITEM,
        EM_SETPASSWORDCHAR, WM_DEADCHAR, WM_SYSCHAR, WM_SYSDEADCHAR, WM_MENUCHAR };
    unsigned i;
    DWORD value;
    SetLastError(0x447119);
    (void)OpenNtUserThreadCodePage();
    CHECK(GetLastError() == 0x447119);
    for (i = 0; i < ARRAYSIZE(messages); ++i) {
        value = 0x4241;
        CHECK(RtlMBMessageWParamCharToWCS(messages[i], &value));
        CHECK(value == 0x00420041); /* Two characters, not only the low byte. */
        value = 'A';
        CHECK(RtlMBMessageWParamCharToWCS(messages[i], &value));
        CHECK(value == 'A');
    }
    value = 0xdeadbeef;
    CHECK(RtlMBMessageWParamCharToWCS(WM_USER, &value));
    CHECK(value == 0xdeadbeef);
}
static void password_transform_boundary(void)
{
    /* Original sertl.c fixed-seed byte vector, including odd byte lengths.
     * This tests the RTL leaf, not EDIT's ECLock/ECUnlock or guest lifecycle. */
    const BYTE plain[] = {0x41, 0, 0x42, 0, 0xa5};
    const BYTE encoded[] = {0x16, 0x43, 0x54, 0x01};
    BYTE bytes[sizeof(plain)], seed;
    UNICODE_STRING string;
    USHORT length, i;
    for (length = 0; length <= 4; ++length) {
        CopyMemory(bytes, plain, sizeof(bytes));
        string.Buffer = (PWSTR)bytes;
        string.Length = length;
        string.MaximumLength = sizeof(bytes);
        seed = 0x55;
        RtlRunEncodeUnicodeString(&seed, &string);
        CHECK(seed == 0x55 && string.Length == length &&
            string.MaximumLength == sizeof(bytes) && string.Buffer == (PWSTR)bytes);
        for (i = 0; i < length; ++i) CHECK(bytes[i] == encoded[i]);
        for (i = length; i < sizeof(bytes); ++i) CHECK(bytes[i] == plain[i]);
        RtlRunDecodeUnicodeString(seed, &string);
        CHECK(!memcmp(bytes, plain, sizeof(bytes)));
    }
    seed = 0;
    RtlRunEncodeUnicodeString(&seed, &string);
    CHECK(seed != 0);
    RtlRunDecodeUnicodeString(seed, &string);
    CHECK(!memcmp(bytes, plain, sizeof(bytes)));
}
static void mnemonic_prefix_contract(void)
{
    WCHAR copy[32];
    LONG result;
    result = GetPrefixCount(L"&Open && Close", 14, copy, 31);
    CHECK(LOWORD(result) == 0 && HIWORD(result) == 2);
    CHECK(!wcscmp(copy, L"Open & Close"));
    CHECK(GetPrefixCount(L"&Open && Close", 14, NULL, 0) == result);
    result = GetPrefixCount(L"A&&B", 4, copy, 31);
    CHECK(LOWORD(result) == 0xffff && HIWORD(result) == 1);
    CHECK(!wcscmp(copy, L"A&B"));
    result = GetPrefixCount(L"plain", 5, copy, 31);
    CHECK(LOWORD(result) == 0xffff && HIWORD(result) == 0);
    CHECK(!wcscmp(copy, L"plain"));
    copy[0] = copy[1] = copy[2] = L'!';
    result = GetPrefixCount(L"abc", 3, copy, 1);
    CHECK(HIWORD(result) == 0 && copy[0] == L'a');
    CHECK(copy[1] == 0 && copy[2] == L'!'); /* capacity excludes terminator */
    copy[0] = copy[1] = L'!';
    GetPrefixCount(L"abc", 3, copy, 0);
    CHECK(copy[0] == 0 && copy[1] == L'!');
    result = GetPrefixCount(L"&", 1, copy, 31);
    CHECK(LOWORD(result) == 0 && HIWORD(result) == 1 && copy[0] == 0);
}
static void mnemonic_extent_contract(void)
{
    static const int lengths[] = {0, 1, 254, 255, 256, 1024};
    WCHAR input[1026], expected[1026];
    SIZE actual, native;
    HDC dc = CreateCompatibleDC(NULL);
    unsigned i, mode;
    int j, count;
    CHECK(dc != NULL);
    if (!dc) return;
    for (i = 0; i < ARRAYSIZE(lengths); ++i) {
        for (mode = 0; mode < 3; ++mode) {
            count = lengths[i];
            for (j = 0; j < count; ++j) input[j] = expected[j] = L'W';
            input[count] = expected[count] = 0;
            if (mode && count) {
                input[0] = L'&';
                if (mode == 2 && count > 1) {
                    input[1] = L'&'; expected[0] = L'&';
                }
                expected[count - 1] = 0;
            }
            CHECK(GetTextExtentPointW(dc, expected,
                count - (mode && count ? 1 : 0), &native));
            CHECK(PSMGetTextExtent(dc, input, count, &actual));
            CHECK(actual.cx == native.cx && actual.cy == native.cy);
        }
    }
    actual.cx = actual.cy = 99;
    CHECK(!PSMGetTextExtent(dc, L"x", -1, &actual));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && !actual.cx && !actual.cy);
    CHECK(DeleteDC(dc));
}
int main(void)
{
    WCHAR source[] = L"ab*cd";
    WCHAR out[8];
    LARGE_UNICODE_STRING text;
    LARGE_ANSI_STRING ansi;
    const char ansi_source[] = "ab\x81\x40";
    RtlInitLargeAnsiString(&ansi, ansi_source, (UINT)-1);
    CHECK(ansi.Buffer == ansi_source && ansi.Length == 4 &&
        ansi.MaximumLength == 5 && ansi.bAnsi);
    /* Count bytes, do not decode DBCS or allocate another text buffer. */
    RtlInitLargeAnsiString(&ansi, ansi_source, 3);
    CHECK(ansi.Length == 3 && ansi.MaximumLength == 3 && ansi.bAnsi);
    RtlInitLargeAnsiString(&ansi, ansi_source, 0);
    CHECK(ansi.Buffer == ansi_source && !ansi.Length && !ansi.MaximumLength);
    RtlInitLargeAnsiString(&ansi, "", (UINT)-1);
    CHECK(ansi.Buffer && !ansi.Length && ansi.MaximumLength == 1 && ansi.bAnsi);
    RtlInitLargeAnsiString(&ansi, NULL, (UINT)-1);
    CHECK(!ansi.Buffer && !ansi.Length && !ansi.MaximumLength && ansi.bAnsi);
    password_transform_boundary();
    message_character_contract();
    mnemonic_prefix_contract();
    mnemonic_extent_contract();
    RtlInitLargeUnicodeString(&text, source, (UINT)-1);
    CHECK(text.Buffer == source && text.Length == 10 && text.MaximumLength == 12 && !text.bAnsi);
    CHECK(FindCharPosition(source, L'*') == 2);
    CHECK(FindCharPosition(source, L'?') == 5);
    CHECK(FindCharPosition(source, 0) == 5);
    CHECK(TextCopy(&text, out, 8) == 5 && !wcscmp(out, source));
    out[3] = L'!';
    CHECK(TextCopy(&text, out, 3) == 2 && !wcscmp(out, L"ab") && out[3] == L'!');
    out[0] = L'!';
    CHECK(TextCopy(&text, out, 0) == 0 && out[0] == L'!');
    CHECK(TextCopy(&text, out, 1) == 0 && out[0] == 0);
    /* The original initializer limit is bytes, including odd limits;
     * do not substitute a character-count or USHORT-limited helper. */
    RtlInitLargeUnicodeString(&text, source, 3);
    CHECK(text.Length == 3 && text.MaximumLength == 3);
    CHECK(TextCopy(&text, out, 8) == 1 && !wcscmp(out, L"a"));
    RtlInitLargeUnicodeString(&text, NULL, (UINT)-1);
    CHECK(!text.Buffer && !text.Length && !text.MaximumLength && !text.bAnsi);
    out[0] = L'!';
    CHECK(wcsncpycch(out, source, 0) == 0 && out[0] == L'!');
    out[2] = L'!';
    CHECK(wcsncpycch(out, source, 2) == 2 && out[0] == L'a' && out[1] == L'b' && out[2] == L'!');
    out[6] = L'!';
    CHECK(wcsncpycch(out, source, 8) == 6 && !wcscmp(out, source) && out[6] == L'!');
    if (!failures) puts("WOW_CONTROL_STRING_OK");
    return failures ? 1 : 0;
}
