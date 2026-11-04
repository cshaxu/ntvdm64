#ifndef COMMON_NATIVE_HOOK_CONTEXT_H
#define COMMON_NATIVE_HOOK_CONTEXT_H
#include <windows.h>
#include "common/protocol/native_hook.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct nthook_context {
    DWORD mode;
    DWORD creation_flags; /* One redirected launch, never descendant policy. */
    HANDLE frontend,execution;
    WCHAR launcher[MAX_PATH],hook[MAX_PATH],hook64[MAX_PATH];
} nthook_context;
DWORD nthook_context_parse(nthook_context *context,const void *packet,DWORD bytes,DWORD machine);
DWORD nthook_context_read(nthook_context *context,BOOL *found);
DWORD nthook_context_paths(nthook_context *context);
#ifdef __cplusplus
}
#endif
#endif
