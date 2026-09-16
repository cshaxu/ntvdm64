/*
 * Finite modern bindings for the selected original USER chartran.c slice.
 * Allocation remains compatible with the reached WOW32 LocalFree ownership.
 */

#include <windows.h>

USHORT WINAPI OpenNtUserNlsAnsiCodePage(VOID)
{
    UINT code_page = GetACP();

    return code_page <= 0xffffu ? (USHORT)code_page : (USHORT)CP_ACP;
}

PVOID WINAPI OpenNtUserRtlAllocMem(SIZE_T bytes)
{
    return LocalAlloc(LMEM_FIXED, bytes);
}

VOID WINAPI OpenNtUserRtlFreeMem(PVOID value)
{
    if (value != NULL) (void)LocalFree((HLOCAL)value);
}
