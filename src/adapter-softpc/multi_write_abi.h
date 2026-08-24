#ifndef RUNTIME_MULTI_WRITE_ABI_H
#define RUNTIME_MULTI_WRITE_ABI_H

#include <stdint.h>
#include "guest_write_abi.h"

#define RUNTIME_MULTI_WRITE_ABI_MAGIC 0x42584d57u
#define RUNTIME_MULTI_WRITE_ABI_VERSION 1u
#define RUNTIME_MULTI_WRITE_MAX_WRITES 64u
/* A COMMAND environment is a 16-bit byte-counted guest object.  Match the
 * mantle's already-admitted 65535-byte mechanical action ceiling so a valid
 * OpenNT-style environment need not be rejected at adapter admission. */
#define RUNTIME_MULTI_WRITE_MAX_PAYLOAD 65535u

typedef struct runtime_multi_write_v1 {
    uint32_t magic, abi_version, struct_bytes, write_count;
    uint64_t payload_bytes;
    runtime_guest_write_v1 writes[RUNTIME_MULTI_WRITE_MAX_WRITES];
} runtime_multi_write_v1;

void runtime_multi_write_v1_initialize(runtime_multi_write_v1 *value);
int runtime_multi_write_v1_add(runtime_multi_write_v1 *value,
    uint64_t address, uint64_t bytes, uint64_t payload_offset);
int runtime_multi_write_v1_preflight(const runtime_multi_write_v1 *value,
    uint64_t aperture_bytes, uint64_t payload_bytes);
#endif
