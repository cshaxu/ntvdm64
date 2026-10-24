#include "precomp.h"

PVOID FASTCALL malloc_w(ULONG size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}

VOID FASTCALL free_w(PVOID value)
{
    if (value) HeapFree(GetProcessHeap(), 0, value);
}

int __cdecl main(void)
{
    HTASK16 alias;
    CHAR full[32768];
    CHAR short_name[4] = { 'x', 'x', 'x', 'x' };
    CHAR one[1] = { 'x' };
    UINT full_length;
    UINT short_length;
    UINT one_length;
    DWORD written;
    static const CHAR marker[] = "WOW_MODULE_ALIAS_BOUNDS_OK\r\n";

    alias = AddHtaskAlias(GetCurrentThreadId());
    if (!alias || FindHtaskAlias(GetCurrentThreadId()) != alias) return 1;

    full_length = GetHtaskAliasProcessName(alias, full, sizeof(full));
    if (!full_length || full[full_length] != '\0') return 2;

    short_length = GetHtaskAliasProcessName(alias, short_name, sizeof(short_name));
    if (short_length != sizeof(short_name) - 1 || short_name[3] != '\0' ||
        memcmp(short_name, full, short_length) != 0) return 3;

    one_length = GetHtaskAliasProcessName(alias, one, sizeof(one));
    if (one_length || one[0] != '\0') return 4;
    RemoveHtaskAlias(alias);
    if (GetHtaskAliasProcessName(0, full, sizeof(full)) != 0 ||
        full[0] != '\0') return 5;

    UNREFERENCED_PARAMETER(full_length);
    UNREFERENCED_PARAMETER(short_length);
    UNREFERENCED_PARAMETER(one_length);
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), marker,
                   sizeof(marker) - 1, &written, NULL) ||
        written != sizeof(marker) - 1) return 6;
    return 0;
}
