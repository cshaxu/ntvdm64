#ifndef BX_NTVDM_INITIAL_STATE_ABI_H
#define BX_NTVDM_INITIAL_STATE_ABI_H

#include <stdint.h>

#include "bx_ntvdm_guest_write_abi.h"

#define BX_NTVDM_INITIAL_STATE_ABI_MAGIC 0x42584953u
#define BX_NTVDM_INITIAL_STATE_ABI_VERSION 1u
#define BX_NTVDM_INITIAL_STATE_V1_MAX_RANGES 4u
#define BX_NTVDM_INITIAL_STATE_V1_MAX_BYTES 64u
#define BX_NTVDM_INITIAL_STATE_V1_EVIDENCE_SHA256_BYTES 32u

enum bx_ntvdm_initial_state_v1_disposition {
    BX_NTVDM_INITIAL_STATE_V1_ABSENT = 0u,
    BX_NTVDM_INITIAL_STATE_V1_PRESENT = 1u
};

/* Profile-owned, copied ordinary-RAM initial state. Range identities and the
 * evidence digest are opaque at this boundary: this ABI names neither a
 * firmware feature nor a guest service. An absent declaration is valid data,
 * but can never be admitted to a future mechanical publisher. */
typedef struct bx_ntvdm_initial_state_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t disposition;
    uint32_t range_count;
    uint32_t payload_bytes;
    uint8_t evidence_sha256[BX_NTVDM_INITIAL_STATE_V1_EVIDENCE_SHA256_BYTES];
    bx_ntvdm_guest_write_v1 ranges[BX_NTVDM_INITIAL_STATE_V1_MAX_RANGES];
    uint8_t payload[BX_NTVDM_INITIAL_STATE_V1_MAX_BYTES];
} bx_ntvdm_initial_state_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_initial_state_v1_clear(bx_ntvdm_initial_state_v1 *value);
int bx_ntvdm_initial_state_v1_valid(const bx_ntvdm_initial_state_v1 *value);
int bx_ntvdm_initial_state_v1_admitted(const bx_ntvdm_initial_state_v1 *value,
    uint64_t aperture_bytes);

#ifdef __cplusplus
}
#endif

#endif
