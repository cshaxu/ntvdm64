/*
 * Finite modern bindings for the selected original USER chartran.c slice.
 * Allocation remains compatible with the reached WOW32 LocalFree ownership.
 */

#include <windows.h>

/* OPENNT-HOST-119: native input layout owns this value. Original
 * client/client.c ToAsciiEx uses this public locale query; the original
 * kernel layout producer falls back to CP_ACP if charset discovery fails.
 * Do not cache it across layout changes or read a private CLIENTINFO. */
USHORT WINAPI OpenNtUserThreadCodePage(VOID)
{
    DWORD error = GetLastError();
    DWORD page = CP_ACP;
    HKL layout = GetKeyboardLayout(0);
    if (!layout || !GetLocaleInfoW(LOWORD((ULONG_PTR)layout),
            LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,
            (LPWSTR)&page, sizeof(page) / sizeof(WCHAR)) || page > 0xffffu)
        page = CP_ACP;
    SetLastError(error);
    return (USHORT)page;
}

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
