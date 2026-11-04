#ifndef NTHOOK32_H
#define NTHOOK32_H
#include "common/native-hook/context.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Installer borrows an actual, newly created suspended child. Never resumes
 * or closes its handles; it may wait for a finite cross-width installer only.
 * On failure the creator MUST abort this
 * uncommitted child; it must not resume a partially modified image. */
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode);
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher);
DWORD nthook_legacy_type(PCWSTR application,DWORD *type);
DWORD nthook_native_subsystem(HANDLE child,DWORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
