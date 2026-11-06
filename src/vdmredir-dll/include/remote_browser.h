#ifndef VDMREDIR_REMOTE_BROWSER_H
#define VDMREDIR_REMOTE_BROWSER_H
#include <windows.h>
/* Original lmbrowsr.h WORD/cdecl import; never a replacement XACTSRV handler. */
WORD I_BrowserServerEnumForXactsrv(LPCWSTR transport,LPCWSTR client,ULONG native_level,
    USHORT client_level,PVOID buffer,WORD bytes,DWORD preferred,LPDWORD read,
    LPDWORD total,DWORD type,LPCWSTR domain,LPCWSTR first,PWORD converter);
#endif
