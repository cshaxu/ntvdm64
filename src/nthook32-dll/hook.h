#ifndef NTHOOK32_H
#define NTHOOK32_H
#include "common/native-hook/installer.h"
#ifdef __cplusplus
extern "C" {
#endif
DWORD nthook_legacy_type_flags(PCWSTR application,DWORD flags,DWORD *type);
DWORD nthook_legacy_type(PCWSTR application,DWORD *type);
DWORD nthook_native_subsystem(HANDLE child,DWORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
