/*
 * Source-shaped declaration bridge for the historical OpenNT conapi.h
 * include.  This is deliberately not a replacement console provider.
 */
#ifndef MVDM_ADAPTER_CONAPI_H
#define MVDM_ADAPTER_CONAPI_H

#include <windows.h>

/* DIVERGENCE(ADAPTER-WIN32-012): The selected OpenNT declaration carrier
 * pulls private NT headers which are not a valid modern user-mode closure.
 * Retain only the declarations reached by the selected original SoftPC
 * sources.  The functions remain external contracts; this header authors no
 * fallback behavior. */
#ifndef NOGDI
typedef struct _CONSOLE_GRAPHICS_BUFFER_INFO {
    DWORD dwBitMapInfoLength;
    LPBITMAPINFO lpBitMapInfo;
    DWORD dwUsage;
    HANDLE hMutex;
    PVOID lpBitMap;
} CONSOLE_GRAPHICS_BUFFER_INFO, *PCONSOLE_GRAPHICS_BUFFER_INFO;
#endif

typedef struct _APPKEY {
    WORD Modifier;
    WORD ScanCode;
} APPKEY, *LPAPPKEY;

BOOL WINAPI SetConsoleKeyShortcuts(
    BOOL bSet,
    BYTE bReserveKeys,
    LPAPPKEY lpAppKeys,
    DWORD dwNumAppKeys
    );

#endif /* MVDM_ADAPTER_CONAPI_H */
