#ifndef COMMON_WOW_PATHS_H
#define COMMON_WOW_PATHS_H
#include <stdint.h>
/* Copy package paths or stage the worker-owned SYSTEM.INI shadow. */
int __cdecl mvdm_softpc_profile_shadow_system_ini(char *, uint32_t);
uint32_t __cdecl NtvdmGetWindowsDirectoryA(char *, uint32_t);
uint32_t __cdecl NtvdmGetSystemDirectoryA(char *, uint32_t);
#endif
