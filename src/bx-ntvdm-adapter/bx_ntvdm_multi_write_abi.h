#ifndef BX_NTVDM_MULTI_WRITE_ABI_H
#define BX_NTVDM_MULTI_WRITE_ABI_H

#include <stdint.h>
#include "bx_ntvdm_guest_write_abi.h"

#define BX_NTVDM_MULTI_WRITE_ABI_MAGIC 0x42584d57u
#define BX_NTVDM_MULTI_WRITE_ABI_VERSION 1u
#define BX_NTVDM_MULTI_WRITE_MAX_WRITES 64u
#define BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD 4096u

typedef struct bx_ntvdm_multi_write_v1 {
    uint32_t magic, abi_version, struct_bytes, write_count;
    uint64_t payload_bytes;
    bx_ntvdm_guest_write_v1 writes[BX_NTVDM_MULTI_WRITE_MAX_WRITES];
} bx_ntvdm_multi_write_v1;

void bx_ntvdm_multi_write_v1_initialize(bx_ntvdm_multi_write_v1 *value);
int bx_ntvdm_multi_write_v1_add(bx_ntvdm_multi_write_v1 *value,
    uint64_t address, uint64_t bytes, uint64_t payload_offset);
int bx_ntvdm_multi_write_v1_preflight(const bx_ntvdm_multi_write_v1 *value,
    uint64_t aperture_bytes, uint64_t payload_bytes);
#endif
