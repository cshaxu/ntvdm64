#ifndef COMMON_NATIVE_LAUNCH_CODEC_H
#define COMMON_NATIVE_LAUNCH_CODEC_H
#include <windows.h>
#include <stdint.h>
#include "common/protocol/frontend_protocol.h"
/* Product packet bound, not a Windows environment-size restriction. Allow
 * long Unicode environments and all existing 32767-character string fields. */
#define NATIVE_LAUNCH_MAX_BYTES (1024u * 1024u)
/* Strings in the packet follow this header in application/command/directory/
 * environment order. The request receiver validates sender-local values and
 * duplicates allowed resources before calling the shared launch binding.
 * console_mask replaces only actual Console streams, not files/pipes/NULL. */
typedef struct run16_native_start {
    PCWSTR application,command,directory,environment;
    HANDLE standard[3],capabilities[2];
    DWORD console_mask;
} run16_native_start;
DWORD run16_native_launch_pack(const run16_native_start *,BYTE **,DWORD *);
/* Validated borrowed string views; caller keeps payload alive. */
DWORD run16_native_launch_unpack(BYTE *,DWORD,run16_native_launch_packet *,WCHAR **);

#endif
