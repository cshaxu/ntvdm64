/* Exercise the selected original RTL time object against Windows' public
 * FILETIME calendar conversion, including the full unsigned input boundary. */
#include <windows.h>
#include <stdio.h>

VOID RtlSecondsSince1970ToTime(ULONG seconds, PLARGE_INTEGER time);

int main(void)
{
    static const struct {
        ULONG seconds;
        WORD year, month, day, hour, minute, second;
    } cases[] = {
        {0u, 1970, 1, 1, 0, 0, 0},
        {1u, 1970, 1, 1, 0, 0, 1},
        {86400u, 1970, 1, 2, 0, 0, 0},
        {951782400u, 2000, 2, 29, 0, 0, 0},
        {0x7fffffffu, 2038, 1, 19, 3, 14, 7},
        {0x80000000u, 2038, 1, 19, 3, 14, 8},
        {0xffffffffu, 2106, 2, 7, 6, 28, 15}
    };
    unsigned i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        struct { DWORD before; LARGE_INTEGER value; DWORD after; } result;
        SYSTEMTIME calendar = {0};
        FILETIME expected;
        result.before = 0x12345678u;
        result.after = 0x87654321u;
        result.value.QuadPart = -1;
        calendar.wYear = cases[i].year;
        calendar.wMonth = cases[i].month;
        calendar.wDay = cases[i].day;
        calendar.wHour = cases[i].hour;
        calendar.wMinute = cases[i].minute;
        calendar.wSecond = cases[i].second;
        if (!SystemTimeToFileTime(&calendar, &expected)) return 1;
        RtlSecondsSince1970ToTime(cases[i].seconds, &result.value);
        if (result.value.LowPart != expected.dwLowDateTime ||
            (DWORD)result.value.HighPart != expected.dwHighDateTime ||
            result.before != 0x12345678u || result.after != 0x87654321u)
            return 2;
    }
    puts("PASS original RTL time: epoch, leap day, signed boundary, ULONG maximum and output guards");
    return 0;
}
