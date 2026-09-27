#ifndef FRONTEND_NATIVE_LAUNCH_H
#define FRONTEND_NATIVE_LAUNCH_H
#include <windows.h>
#include <stdint.h>
/* Strings in the packet follow this header in application/command/directory/
 * environment order. Handles are borrowed helper-local duplicates retained by
 * the root until LAUNCH replies; the helper never closes a caller's duplicate.
 * console_mask replaces only actual Console streams, not files/pipes/NULL. */
typedef struct run16_native_launch_packet {
    uint32_t characters[4],console_mask;
    uint64_t standard[3],capabilities[2];
} run16_native_launch_packet;
typedef struct run16_native_start {
    PCWSTR application,command,directory,environment;
    HANDLE standard[3],capabilities[2];
    DWORD console_mask;
} run16_native_start;
DWORD run16_native_launch_pack(const run16_native_start *,BYTE **,DWORD *);
/* Validated borrowed string views; caller keeps payload alive. */
DWORD run16_native_launch_unpack(BYTE *,DWORD,run16_native_launch_packet *,WCHAR **);
DWORD run16_native_launch_start(BYTE *,DWORD,PROCESS_INFORMATION *);

#endif
