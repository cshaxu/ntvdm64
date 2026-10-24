#ifndef COMMON_NATIVE_PROCESS_H
#define COMMON_NATIVE_PROCESS_H
#include "common/codec/native_launch.h"

/* Restricted local handle materialization and process creation, selected
 * separately from the packet codec. No admission, frontend or lifetime policy. */
DWORD run16_native_launch_start(BYTE *,DWORD,PROCESS_INFORMATION *);
/* Bind the suspended target to its authenticated request before execution. */
DWORD run16_native_launch_start_suspended(BYTE *,DWORD,PROCESS_INFORMATION *);
#endif
