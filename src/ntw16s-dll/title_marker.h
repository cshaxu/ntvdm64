#ifndef NTW16S_TITLE_MARKER_H
#define NTW16S_TITLE_MARKER_H
#include "api.h"
/* Title witness only: no drawing, geometry, theme or process-policy changes.
 * Borrowed caption HWND, same GUI thread. Repeated install is idempotent.
 * Remove restores the last accepted source caption; destruction cleans up.
 * Retain the DLL reference through every installed window's lifetime. */
NTW16S_API BOOL __cdecl Ntw16sTitleInstall(HWND window);
NTW16S_API BOOL __cdecl Ntw16sTitleRemove(HWND window);
#endif
