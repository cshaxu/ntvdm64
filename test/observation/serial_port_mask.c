/* Host oracle for the guest VCD serial-discovery integration test. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    DWORD size, length = 0, error = ERROR_INSUFFICIENT_BUFFER, mask = 0;
    char *names = NULL;
    for (size = 32768; size <= 1048576; size *= 2) {
        names = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if (!names) return ERROR_NOT_ENOUGH_MEMORY;
        length = QueryDosDeviceA(NULL, names, size);
        if (length) break;
        error = GetLastError();
        HeapFree(GetProcessHeap(), 0, names);
        names = NULL;
        if (error != ERROR_INSUFFICIENT_BUFFER) return (int)error;
    }
    if (!names) return (int)error;
    for (DWORD start = 0; start < length && names[start];) {
        DWORD end = start;
        while (end < length && names[end]) ++end;
        if (end == length) {
            HeapFree(GetProcessHeap(), 0, names);
            return ERROR_INVALID_DATA;
        }
        if (end - start == 4 && !_strnicmp(names + start, "COM", 3) &&
            names[start + 3] >= '1' && names[start + 3] <= '9')
            mask |= 1u << (names[start + 3] - '1');
        start = end + 1;
    }
    HeapFree(GetProcessHeap(), 0, names);
    printf("%lu\n", mask);
    return 0;
}
