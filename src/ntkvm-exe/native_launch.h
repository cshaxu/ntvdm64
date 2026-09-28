#ifndef FRONTEND_NATIVE_LAUNCH_H
#define FRONTEND_NATIVE_LAUNCH_H
#include <windows.h>
#include <stdint.h>
/* Strings in the packet follow this header in application/command/directory/
 * environment order. The request receiver validates sender-local values and
 * duplicates allowed resources before calling the frontend-local launch binding.
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
/* Frontend-local creation: no helper process or Console attachment.
 * The returned process is the actual target; caller owns both result handles. */
DWORD run16_native_launch_conpty(const run16_native_start *,HPCON,PROCESS_INFORMATION *);

#endif
