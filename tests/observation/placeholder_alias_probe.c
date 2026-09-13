/* T406 mechanism experiment, not a production provider or guest fixture.
 * Contracts: Microsoft VirtualAlloc2 / MapViewOfFile3 / UnmapViewOfFile2.
 * Proves only native section alias mechanics; no OpenNT API is replaced. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); \
    goto cleanup; } } while (0)

int main(void)
{
    SYSTEM_INFO info;
    MEMORY_BASIC_INFORMATION region;
    HANDLE section = NULL;
    unsigned char *backing = NULL, *window = NULL;
    unsigned char result[4];
    SIZE_T page;
    int split = 0, first_mapped = 0, second_mapped = 0;
    int status = 1;

    GetSystemInfo(&info);
    page = info.dwPageSize;
    CHECK(sizeof(void *) == 4 && page == 4096);
    section = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL,
        PAGE_READWRITE, 0, (DWORD)(page * 2), NULL);
    CHECK(section != NULL);
    backing = MapViewOfFile(section, FILE_MAP_ALL_ACCESS, 0, 0, page * 2);
    CHECK(backing != NULL);
    backing[page * 2 - 2] = 0x6c;
    backing[page * 2 - 1] = 0x5b;
    backing[0] = 0x8e;
    backing[1] = 0x7d;
    window = VirtualAlloc2(NULL, NULL, page * 2,
        MEM_RESERVE | MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, NULL, 0);
    CHECK(window != NULL);
    CHECK(VirtualFree(window, page, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER));
    split = 1;
    CHECK(MapViewOfFile3(section, GetCurrentProcess(), window, page, page,
        MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0) == window);
    first_mapped = 1;
    CHECK(MapViewOfFile3(section, GetCurrentProcess(), window + page, 0, page,
        MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0) == window + page);
    second_mapped = 1;
    memmove(result, window + page - 2, sizeof(result));
    CHECK(result[0] == 0x6c && result[1] == 0x5b &&
        result[2] == 0x8e && result[3] == 0x7d);
    /* Reject replacement of a live view; preserve its existing bytes. */
    CHECK(MapViewOfFile3(section, GetCurrentProcess(), window, 0, page,
        MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0) == NULL);
    CHECK(window[page - 2] == 0x6c && window[page] == 0x8e);
    CHECK(UnmapViewOfFile2(GetCurrentProcess(), window + page,
        MEM_PRESERVE_PLACEHOLDER));
    second_mapped = 0;
    CHECK(VirtualQuery(window + page, &region, sizeof(region)) != 0);
    CHECK(region.State == MEM_RESERVE);
    CHECK(MapViewOfFile3(section, GetCurrentProcess(), window + page, page,
        page, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0) == window + page);
    second_mapped = 1;
    window[17] = 0xa5;
    CHECK(window[page + 17] == 0xa5 && backing[page + 17] == 0xa5);
    window[page + 18] = 0x5a;
    CHECK(window[18] == 0x5a && backing[page + 18] == 0x5a);
    CHECK(UnmapViewOfFile2(GetCurrentProcess(), window + page,
        MEM_PRESERVE_PLACEHOLDER));
    second_mapped = 0;
    CHECK(MapViewOfFile3(section, GetCurrentProcess(), window + page, 0,
        page, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, NULL, 0) == window + page);
    second_mapped = 1;
    CHECK(window[page] == 0x8e && window[page + 1] == 0x7d);
    status = 0;
cleanup:
    if (second_mapped && !UnmapViewOfFile2(GetCurrentProcess(),
            window + page, MEM_PRESERVE_PLACEHOLDER)) status = 1;
    if (first_mapped && !UnmapViewOfFile2(GetCurrentProcess(),
            window, MEM_PRESERVE_PLACEHOLDER)) status = 1;
    if (window != NULL) {
        if (split && !VirtualFree(window + page, 0, MEM_RELEASE)) status = 1;
        if (!VirtualFree(window, 0, MEM_RELEASE)) status = 1;
    }
    if (backing != NULL && !UnmapViewOfFile(backing)) status = 1;
    if (section != NULL && !CloseHandle(section)) status = 1;
    if (status == 0) puts("PASS x86 reverse-span duplicate-alias refusal restore cleanup");
    return status;
}
