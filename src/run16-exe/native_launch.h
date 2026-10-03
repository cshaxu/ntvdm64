#ifndef RUN16_NATIVE_LAUNCH_H
#define RUN16_NATIVE_LAUNCH_H
#include "common/codec/native_launch.h"

/* Local resource materialization and target creation, not a common codec.
 * Reused by the native worker; no frontend or scheduling ownership. */
DWORD run16_native_launch_start(BYTE *,DWORD,PROCESS_INFORMATION *);
/* Bind the suspended target to its authenticated request before execution. */
DWORD run16_native_launch_start_suspended(BYTE *,DWORD,PROCESS_INFORMATION *);
#endif
