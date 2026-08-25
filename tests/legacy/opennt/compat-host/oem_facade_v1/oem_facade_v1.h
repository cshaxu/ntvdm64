#ifndef RUNNER_OEM_FACADE_V1_H
#define RUNNER_OEM_FACADE_V1_H

#include <windows.h>

BOOL WINAPI runner_oem_facade_v1_configure_resource_root(LPCSTR path);
VOID WINAPI runner_oem_facade_v1_reset(void);

#endif
