#ifndef NTCON_NATIVE_LAUNCH_H
#define NTCON_NATIVE_LAUNCH_H
#include <windows.h>
#include <stdint.h>
#include "interface/frontend_protocol.h"
/* Strings in the packet follow this header in application/command/directory/
 * environment order. The request receiver validates sender-local values and
 * duplicates allowed resources before calling the NTCON-local launch binding.
 * console_mask replaces only actual Console streams, not files/pipes/NULL. */
typedef struct run16_native_start {
    PCWSTR application,command,directory,environment;
    HANDLE standard[3],capabilities[2];
    DWORD console_mask;
} run16_native_start;
DWORD run16_native_launch_pack(const run16_native_start *,BYTE **,DWORD *);
/* Validated borrowed string views; caller keeps payload alive. */
DWORD run16_native_launch_unpack(BYTE *,DWORD,run16_native_launch_packet *,WCHAR **);
/* NTCON-owned local resource materialization, also used by run16 GUI launch.
 * No worker scheduling, Console creation or frontend ownership is involved. */
DWORD run16_native_launch_start(BYTE *,DWORD,PROCESS_INFORMATION *);
/* NTCON uses this exact create path to attach a direct target to its
 * event-only Job before any target instruction can execute. */
DWORD run16_native_launch_start_suspended(BYTE *,DWORD,PROCESS_INFORMATION *);

#endif
