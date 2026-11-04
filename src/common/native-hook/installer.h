#ifndef COMMON_NATIVE_HOOK_INSTALLER_H
#define COMMON_NATIVE_HOOK_INSTALLER_H
#include "common/native-hook/context.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Borrows a newly created suspended child, never resumes or closes its
 * handles, and may wait for a finite cross-width installer. On failure the
 * caller must abort the uncommitted child instead of resuming it. */
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode);
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher);
#ifdef __cplusplus
}
#endif
#endif
