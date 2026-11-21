/* Private binding for original BaseGetVdmConfigInfo: immutable app-owned
 * package paths replace the NT4 WOW registry. No executable classification. */
#ifndef OPENNT_BASE_CONFIG_H
#define OPENNT_BASE_CONFIG_H
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <string.h>
#include "compat/include/base_vdm_config.h"
BOOL BaseGetVdmConfigInfo(LPCWSTR, ULONG, ULONG, PUNICODE_STRING, PULONG);
#endif
