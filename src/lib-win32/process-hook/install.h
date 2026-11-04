#ifndef LIB_WIN32_PROCESS_HOOK_INSTALL_H
#define LIB_WIN32_PROCESS_HOOK_INSTALL_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Borrow a newly created suspended process and the caller's payload. A NULL
 * dll installs payload only. Failure may leave the child partially modified;
 * its creator must abort the uncommitted child. This function never resumes,
 * closes or terminates the borrowed process. */
DWORD win32_process_hook_apply(HANDLE child,DWORD machine,const GUID *payload_id,
    const void *payload,DWORD bytes,const char *dll);
#ifdef __cplusplus
}
#endif
#endif
