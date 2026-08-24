#ifndef NTDOS64_OEM_FACADE_V1_H
#define NTDOS64_OEM_FACADE_V1_H

#include <windows.h>

BOOL WINAPI ntdos64_oem_facade_v1_configure_resource_root(LPCSTR path);
VOID WINAPI ntdos64_oem_facade_v1_reset(void);

#endif
