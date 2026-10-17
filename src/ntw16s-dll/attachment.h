#ifndef NTW16S_ATTACHMENT_H
#define NTW16S_ATTACHMENT_H
#include "api.h"

/* Same-thread, strictly nested creation scopes. FALSE masks an outer enabled
 * scope (notably a different WOW task). No process/task policy lives here.
 * Begin precedes USER creation; End follows it, including exceptional exits.
 * HWND attachments survive End, and release themselves on WM_NCDESTROY.
 * Keep this DLL loaded while scopes, windows or callbacks remain alive. */
typedef struct ntw16s_creation_scope ntw16s_creation_scope;
typedef struct ntw16s_creation_result {
    DWORD installed,unsupported,failure;
} ntw16s_creation_result;
NTW16S_API ntw16s_creation_scope *__cdecl Ntw16sCreationBegin(BOOL enabled);
NTW16S_API BOOL __cdecl Ntw16sCreationEnd(ntw16s_creation_scope *,ntw16s_creation_result *);
/* Reports actual automatic installation, not title text or inherited policy. */
NTW16S_API BOOL __cdecl Ntw16sWindowAttached(HWND);
NTW16S_API BOOL __cdecl Ntw16sWindowRemove(HWND);
/* Same-thread terminal resource release, including after a caller replaced
 * the native window procedure. No application notification or appearance
 * restoration; the caller owns the native terminal edge. Idempotent. */
NTW16S_API BOOL __cdecl Ntw16sWindowTerminal(HWND);
/* Repaint after a native range/visibility/enable mutation. Native USER still
 * owns those values; this performs no application-content scrolling. */
NTW16S_API void __cdecl Ntw16sScrollChanged(HWND);
/* Native Hook supplies this public USER-shaped binding. Uncaptured windows
 * are delegated unchanged; captured controls retain application theme strings
 * for detach while the active presentation remains locally unthemed. */
NTW16S_API HRESULT __cdecl Ntw16sSetWindowTheme(HWND,LPCWSTR,LPCWSTR);
#endif
