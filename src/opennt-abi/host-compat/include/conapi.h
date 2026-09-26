/*
 * Source-shaped declaration bridge for the historical OpenNT conapi.h
 * include.  This is deliberately not a replacement console provider.
 */
#ifndef MVDM_ADAPTER_CONAPI_H
#define MVDM_ADAPTER_CONAPI_H

#include <windows.h>
#include "console_title.h"

/* Standalone Console graphics backing remains worker-local; presentation
 * is copied to the authenticated frontend. Non-owned handles remain native. */
HANDLE WINAPI MvdmCreateConsoleScreenBuffer(DWORD,DWORD,const SECURITY_ATTRIBUTES *,DWORD,void *);
BOOL WINAPI MvdmSetConsoleActiveScreenBuffer(HANDLE);
BOOL WINAPI MvdmCloseConsoleHandle(HANDLE);
#define CreateConsoleScreenBuffer MvdmCreateConsoleScreenBuffer
#define SetConsoleActiveScreenBuffer MvdmSetConsoleActiveScreenBuffer
#define CloseHandle MvdmCloseConsoleHandle

/* Native desktop pointer operations belong to the admitted DOS frontend.
 * Original mouse scaling/warp policy remains in nt_mouse.c. */
BOOL WINAPI MvdmGetCursorPos(LPPOINT);
BOOL WINAPI MvdmSetCursorPos(int,int);
BOOL WINAPI MvdmGetClipCursor(LPRECT);
BOOL WINAPI MvdmClipCursor(const RECT *);
#define GetCursorPos MvdmGetCursorPos
#define SetCursorPos MvdmSetCursorPos
#define GetClipCursor MvdmGetClipCursor
#define ClipCursor MvdmClipCursor

UINT WINAPI MvdmGetConsoleCP(void);
UINT WINAPI MvdmGetConsoleOutputCP(void);
#define GetConsoleCP MvdmGetConsoleCP
#define GetConsoleOutputCP MvdmGetConsoleOutputCP

BOOL WINAPI MvdmGetCurrentConsoleFont(HANDLE,BOOL,PCONSOLE_FONT_INFO);
COORD WINAPI MvdmGetConsoleFontSize(HANDLE,DWORD);
#define GetCurrentConsoleFont MvdmGetCurrentConsoleFont
#define GetConsoleFontSize MvdmGetConsoleFontSize

BOOL WINAPI MvdmGetConsoleDisplayMode(LPDWORD);
BOOL WINAPI MvdmSetConsoleDisplayMode(HANDLE,DWORD,PCOORD);
#define GetConsoleDisplayMode MvdmGetConsoleDisplayMode
#define SetConsoleDisplayMode MvdmSetConsoleDisplayMode

/* T423-S2: original stream/screen callers retain their call shapes; the admitted
 * DOS frontend channel owns user presentation in run16, not the worker. */
BOOL WINAPI MvdmWriteConsoleA(HANDLE,const VOID *,DWORD,LPDWORD,LPVOID);
#define WriteConsoleA MvdmWriteConsoleA
BOOL WINAPI MvdmGetConsoleScreenBufferInfo(HANDLE,PCONSOLE_SCREEN_BUFFER_INFO);
BOOL WINAPI MvdmSetConsoleCursorPosition(HANDLE,COORD);
BOOL WINAPI MvdmSetConsoleCursorInfo(HANDLE,const CONSOLE_CURSOR_INFO *);
BOOL WINAPI MvdmFillConsoleOutputCharacterA(HANDLE,CHAR,DWORD,COORD,LPDWORD);
BOOL WINAPI MvdmFillConsoleOutputAttribute(HANDLE,WORD,DWORD,COORD,LPDWORD);
BOOL WINAPI MvdmScrollConsoleScreenBufferA(HANDLE,const SMALL_RECT *,const SMALL_RECT *,COORD,const CHAR_INFO *);
BOOL WINAPI MvdmSetConsoleTextAttribute(HANDLE,WORD);
#define GetConsoleScreenBufferInfo MvdmGetConsoleScreenBufferInfo
#define SetConsoleCursorPosition MvdmSetConsoleCursorPosition
#define SetConsoleCursorInfo MvdmSetConsoleCursorInfo
BOOL WINAPI MvdmGetConsoleCursorInfo(HANDLE,PCONSOLE_CURSOR_INFO);
BOOL WINAPI MvdmGetConsoleMode(HANDLE,LPDWORD);
BOOL WINAPI MvdmSetConsoleMode(HANDLE,DWORD);
#define GetConsoleCursorInfo MvdmGetConsoleCursorInfo
#define GetConsoleMode MvdmGetConsoleMode
#define SetConsoleMode MvdmSetConsoleMode
#define FillConsoleOutputCharacterA MvdmFillConsoleOutputCharacterA
#define FillConsoleOutputAttribute MvdmFillConsoleOutputAttribute
#define ScrollConsoleScreenBufferA MvdmScrollConsoleScreenBufferA
#define SetConsoleTextAttribute MvdmSetConsoleTextAttribute
BOOL WINAPI MvdmWriteConsoleOutputA(HANDLE,const CHAR_INFO *,COORD,COORD,PSMALL_RECT);
BOOL WINAPI MvdmWriteConsoleOutputW(HANDLE,const CHAR_INFO *,COORD,COORD,PSMALL_RECT);
BOOL WINAPI MvdmReadConsoleOutputA(HANDLE,PCHAR_INFO,COORD,COORD,PSMALL_RECT);
BOOL WINAPI MvdmReadConsoleOutputW(HANDLE,PCHAR_INFO,COORD,COORD,PSMALL_RECT);
#define WriteConsoleOutputA MvdmWriteConsoleOutputA
#define WriteConsoleOutputW MvdmWriteConsoleOutputW
#define ReadConsoleOutputA MvdmReadConsoleOutputA
#define ReadConsoleOutputW MvdmReadConsoleOutputW
BOOL WINAPI MvdmReadConsoleInputW(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
BOOL WINAPI MvdmPeekConsoleInputW(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
#define ReadConsoleInputW MvdmReadConsoleInputW
#define PeekConsoleInputW MvdmPeekConsoleInputW

/* ADAPTER-WIN32-052: retain NT4 cell-grid resize semantics at the modern
 * Console boundary. Original callers and argument shapes are unchanged. */
BOOL WINAPI MvdmSetConsoleScreenBufferSize(HANDLE output, COORD size);
#define SetConsoleScreenBufferSize MvdmSetConsoleScreenBufferSize
BOOL WINAPI MvdmSetConsoleWindowInfo(HANDLE output, BOOL absolute, const SMALL_RECT *window);
#define SetConsoleWindowInfo MvdmSetConsoleWindowInfo

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
