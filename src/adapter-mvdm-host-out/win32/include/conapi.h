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

/* DIVERGENCE(ADAPTER-WIN32-012): These are the reached original VDM console
 * operation selectors from OpenNT's conapi.h.  The complete historical
 * header redeclares APIs and structures now owned by the modern SDK, so it
 * cannot be included beside <windows.h>.  Retain only the exact selectors
 * required by the unchanged original SoftPC host callers. */
#define CONSOLE_GRAPHICS_BUFFER 2
#define SYSTEM_ROOT_CONSOLE_EVENT 3
#define VDM_HIDE_WINDOW         1
#define VDM_IS_ICONIC           2
#define VDM_CLIENT_RECT         3
#define VDM_CLIENT_TO_SCREEN    4
#define VDM_SCREEN_TO_CLIENT    5
#define VDM_IS_HIDDEN           6
#define VDM_FULLSCREEN_NOPAINT  7
#define CONSOLE_UNREGISTER_VDM  0
#define CONSOLE_REGISTER_VDM    1
#define CONSOLE_REGISTER_WOW    2

BOOL WINAPI InvalidateConsoleDIBits(HANDLE hConsoleOutput, PSMALL_RECT lpRect);
BOOL WINAPI SetConsolePalette(HANDLE hConsoleOutput, HPALETTE hPalette,
                              DWORD dwUsage);
VOID WINAPI SetLastConsoleEventActive(VOID);
BOOL WINAPI VDMConsoleOperation(DWORD iFunction, LPVOID lpData);
int WINAPI ShowConsoleCursor(HANDLE hConsoleOutput, BOOL bShow);
/*
 * The original NT4 client sends this operation to Console Server, which
 * associates the command range with the selected output buffer and returns
 * that buffer's menu.  This declaration restores the exact source-facing
 * ABI for the original SoftPC mouse/trapper callers.  It does not claim that
 * modern conhost exposes the historical event-routing provider: that
 * source-shaped provider remains a separately tracked console boundary.
 */
HMENU WINAPI ConsoleMenuControl(
    HANDLE hConsoleOutput,
    UINT dwCommandIdLow,
    UINT dwCommandIdHigh
    );

/*
 * This is the original client-side registration ABI.  NT4 Console Server
 * owned the paired hardware events and mapped state/text buffers.  The
 * adapter retains the full ABI; its selected windowed CPU40 provider returns
 * only the source caller's host-local text buffer and zero hardware state.
 * It does not fabricate the private fullscreen/controller provider.
 * Keep the pointer-bearing signature exact so selected original callers do
 * not fall back to an implicit int declaration on either host architecture.
 */
BOOL WINAPI RegisterConsoleVDM(
    DWORD dwRegisterFlags,
    HANDLE hStartHardwareEvent,
    HANDLE hEndHardwareEvent,
    LPWSTR lpStateSectionName,
    DWORD dwStateSectionNameLength,
    LPDWORD lpStateLength,
    PVOID *lpState,
    LPWSTR lpVDMBufferSectionName,
    DWORD dwVDMBufferSectionNameLength,
    COORD VDMBufferSize,
    PVOID *lpVDMBuffer
    );

/* This is not an NT4 Console Server export.  It is the deliberately narrow
 * modern binding for the one private graphics-buffer allocation site in
 * original nt_graph.c.  The original controller code retains ownership of
 * the DIB description and writes the returned bytes unchanged. */
BOOL WINAPI MvdmPresentationGraphicsBuffer(
    HANDLE hConsoleOutput,
    PCONSOLE_GRAPHICS_BUFFER_INFO lpGraphicsInfo,
    HANDLE *lpScreenBuffer
    );
VOID WINAPI MvdmPresentationGraphicsClear(VOID);

#define CONSOLE_READ_NOREMOVE 0x0001
#define CONSOLE_READ_NOWAIT   0x0002
#define CONSOLE_READ_VALID    (CONSOLE_READ_NOREMOVE | CONSOLE_READ_NOWAIT)

BOOL WINAPI ReadConsoleInputExW(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsRead,
    USHORT wFlags);

BOOL WINAPI WriteConsoleInputVDMW(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsWritten);

BOOL WINAPI SetConsoleKeyShortcuts(
    BOOL bSet,
    BYTE bReserveKeys,
    LPAPPKEY lpAppKeys,
    DWORD dwNumAppKeys
    );

#endif /* MVDM_ADAPTER_CONAPI_H */
