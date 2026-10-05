/* Copied, versioned per-child bootstrap. Handle values are recipient-local
 * attachments; NTSRV, not this declaration, authenticates their objects. */
#ifndef COMMON_NATIVE_HOOK_H
#define COMMON_NATIVE_HOOK_H
#include <stdint.h>
#define NATIVE_HOOK_VERSION 2u
#define NATIVE_HOOK_MAX_BYTES 65536u
#define NATIVE_HOOK_INTERCEPT 1u
#define NATIVE_HOOK_LAUNCHER 2u
typedef struct native_hook_packet {
    uint32_t bytes,version,mode,machine,flags,reserved;
    uint64_t frontend,execution;
    uint32_t launcher_offset,launcher_bytes,hook_offset,hook_bytes;
    uint32_t hook64_offset,hook64_bytes;
} native_hook_packet;
typedef char native_hook_header_is_64[(sizeof(native_hook_packet)==64)?1:-1];
#endif
