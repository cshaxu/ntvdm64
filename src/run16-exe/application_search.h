#ifndef RUN16_APPLICATION_SEARCH_H
#define RUN16_APPLICATION_SEARCH_H
#include <windows.h>

/* User-image discovery only: CWD, then each PATH entry; no product root.
 * Explicit paths remain local to that path. Output is cleared on failure. */
DWORD run16_resolve_application(PCWSTR image,PWSTR output,DWORD capacity);
#endif
