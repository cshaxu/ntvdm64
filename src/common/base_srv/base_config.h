/* Private binding for original BaseGetVdmConfigInfo: immutable app-owned
 * package paths replace the NT4 WOW registry. No executable classification. */
#ifndef OPENNT_BASE_CONFIG_H
#define OPENNT_BASE_CONFIG_H
#include <nt.h>
#include <stdio.h>
#include <string.h>
#include "compat/include/base_vdm_config.h"
#define FULL_INFO_BUFFER_SIZE (sizeof(KEY_VALUE_FULL_INFORMATION) + MAX_VDM_CFG_LINE)
BOOL BaseGetVdmConfigInfo(LPCWSTR, ULONG, ULONG, PUNICODE_STRING, PULONG);
#endif
