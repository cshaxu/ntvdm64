#ifndef NTHOOK32_H
#define NTHOOK32_H
#include <windows.h>
#include "common/protocol/native_hook.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct nthook_context {
    DWORD mode;
    HANDLE frontend,execution;
    /* Both pinned siblings propagate unchanged; hook is always Hook32. */
    WCHAR launcher[MAX_PATH],hook[MAX_PATH],hook64[MAX_PATH];
} nthook_context;
/* Installer borrows an actual, newly created suspended child. Never resumes,
 * closes its handles or waits for it. On failure the creator MUST abort this
 * uncommitted child; it must not resume a partially modified image. */
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode);
DWORD nthook_context_read(nthook_context *context,BOOL *found);
DWORD nthook_context_paths(nthook_context *context);
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher);
DWORD nthook_legacy_type(PCWSTR application,DWORD *type);
DWORD nthook_native_subsystem(HANDLE child,DWORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
