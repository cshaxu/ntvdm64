/*
 * DIVERGENCE(ADAPTER-WIN32-030): Same-shaped public-Win32 replacement for
 * reached NT4 Console Server client calls.  Modern public Console/User32 APIs
 * provide record and coordinate operations but no private server export or
 * cursor reference count.  Original SoftPC callers retain their names,
 * parameters and ordering; unsupported operations fail explicitly.
 */
#include <windows.h>
#include "conapi.h"

HANDLE GetConsoleInputWaitHandle(VOID)
{
    /* DIVERGENCE(ADAPTER-WIN32-030): NT4 supplied a Console Server wait
     * object through this source-facing call.  A modern console input handle
     * is itself waitable, so preserve the call shape and return the process
     * console input endpoint without manufacturing an event or MVDM token. */
    return GetStdHandle(STD_INPUT_HANDLE);
}

BOOL WINAPI ReadConsoleInputExW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                LPDWORD read, USHORT flags)
{
    DWORD available;
    if ((flags & ~CONSOLE_READ_VALID) != 0u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if ((flags & CONSOLE_READ_NOWAIT) != 0u) {
        if (!GetNumberOfConsoleInputEvents(input, &available)) return FALSE;
        if (available == 0u) { if (read != NULL) *read = 0u; return TRUE; }
    }
    if ((flags & CONSOLE_READ_NOREMOVE) != 0u) return PeekConsoleInputW(input, records, count, read);
    return ReadConsoleInputW(input, records, count, read);
}

BOOL WINAPI WriteConsoleInputVDMW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                  LPDWORD written)
{
    return WriteConsoleInputW(input, records, count, written);
}

int WINAPI ShowConsoleCursor(HANDLE output, BOOL show)
{
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(output, &info)) return -1;
    info.bVisible = show;
    if (!SetConsoleCursorInfo(output, &info)) return -1;
    /* Original callers loop while hiding until a negative count and while
       showing until a nonnegative count.  Modern Console API has no count. */
    return show ? 0 : -1;
}

BOOL WINAPI VDMConsoleOperation(DWORD operation, LPVOID data)
{
    HWND window = GetConsoleWindow();
    if (window == NULL || data == NULL) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
    switch (operation) {
    case VDM_IS_ICONIC: *(BOOL *)data = IsIconic(window); return TRUE;
    case VDM_CLIENT_RECT: return GetClientRect(window, (RECT *)data);
    case VDM_CLIENT_TO_SCREEN: return ClientToScreen(window, (POINT *)data);
    default: SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE;
    }
}
