#ifndef COMMON_APPLICATION_SEARCH_H
#define COMMON_APPLICATION_SEARCH_H
#include <windows.h>

/* User-image discovery only: CWD, then each PATH entry; no product root.
 * Explicit paths remain local to that path. Output is cleared on failure. */
#ifdef __cplusplus
extern "C" {
#endif
DWORD common_resolve_application(PCWSTR image,PWSTR output,DWORD capacity);
#ifdef __cplusplus
}
#endif
#endif
