#ifndef WOW_USER_GLOBALS_BINDING_H
#define WOW_USER_GLOBALS_BINDING_H
#include <windows.h>

/* Original globals declarations need only tags for these opaque objects;
 * their runtime layouts and ownership stay with the existing USER providers. */
typedef struct tagSERVERINFO *PSERVERINFO;
typedef struct tagUSERAPIHOOK USERAPIHOOK;
/* XP client/usercli.h: original message-dispatch ABI, also declared there. */
typedef LRESULT (APIENTRY *CFNSCSENDMESSAGE)(HWND, UINT, WPARAM, LPARAM,
    ULONG_PTR, DWORD, BOOL);

/* XP windows/core/w32inc/w32err.h:848-860; original zero-object ABI. */
typedef union tagALWAYSZERO
{
    BYTE b;
    WORD w;
    DWORD dw;
    int i;
    POINT pt;
    POINTL ptl;
    RECT rc;
    RECTL rcl;
    LARGE_INTEGER li;
} ALWAYSZERO;

/* XP windows/core/ntuser/client/usercli.h:2209-2215. */
typedef struct tagACCELCACHE ACCELCACHE, *PACCELCACHE;

/* XP public/internal/windows/inc/winuserp.h:2625. */
typedef BOOL (CALLBACK* INITUSERAPIHOOK)(DWORD dwCmd, void* pvParam);
/* XP windows/core/ntuser/client/globals.h:194. */
#define UNKNOWN_CONIME ~0

/* Retained original clres.c providers, original usercli.h declarations. */
BOOL APIENTRY _FreeResource(HANDLE hResData, HINSTANCE hModule);
LPSTR APIENTRY _LockResource(HANDLE hResData, HINSTANCE hModule);
BOOL APIENTRY _UnlockResource(HANDLE hResData, HINSTANCE hModule);
/* XP usercli.h:985-990, original LPK default providers. */
LONG WINAPI UserLpkTabbedTextOut(HDC, int, int, LPCWSTR, int, int,
    CONST INT *, int, BOOL, int, int, int);
VOID WINAPI UserLpkPSMTextOut(HDC, int, int, LPWSTR, int, DWORD);
#endif
