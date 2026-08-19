#ifndef BX_NTVDM_MUTATION_OVERLAY_V1_H
#define BX_NTVDM_MUTATION_OVERLAY_V1_H

#include <stdint.h>

#include "bx_ntvdm_mutation_profile_v1.h"

#define BX_NTVDM_MUTATION_OVERLAY_V1_MAGIC 0x42584d4fu
#define BX_NTVDM_MUTATION_OVERLAY_V1_VERSION 1u
#define BX_NTVDM_MUTATION_OVERLAY_V1_MAX_RECORDS 16u
/* Covers the largest currently admitted copied session value: the DEM CWD
 * context's 128-wide-character relative path including its terminator. */
#define BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES 256u

typedef struct bx_ntvdm_mutation_overlay_record_v1 {
    uint32_t owner_id;
    uint32_t mutation_class;
    uint32_t key;
    uint32_t value_bytes;
    uint8_t value[BX_NTVDM_MUTATION_OVERLAY_V1_MAX_BYTES];
} bx_ntvdm_mutation_overlay_record_v1;

/* This is session-owned volatile state. key and value are opaque copied
 * payload; neither represents a host path, handle, pointer, BOP or guest
 * address. */
typedef struct bx_ntvdm_mutation_overlay_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t record_count;
    uint32_t reserved0;
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_overlay_record_v1
        records[BX_NTVDM_MUTATION_OVERLAY_V1_MAX_RECORDS];
} bx_ntvdm_mutation_overlay_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_mutation_overlay_v1_initialize(
    bx_ntvdm_mutation_overlay_v1 *overlay,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_mutation_overlay_v1_valid(
    const bx_ntvdm_mutation_overlay_v1 *overlay);
int bx_ntvdm_mutation_overlay_v1_record(
    bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, const uint8_t *value,
    uint32_t value_bytes);
/* Replaces an existing owned key or records a new one.  It preserves the
 * same session/profile authorization as record(), while making stateful
 * session capabilities possible without a side store. */
int bx_ntvdm_mutation_overlay_v1_replace(
    bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, const uint8_t *value,
    uint32_t value_bytes);
int bx_ntvdm_mutation_overlay_v1_lookup(
    const bx_ntvdm_mutation_overlay_v1 *overlay, uint32_t owner_id,
    uint32_t mutation_class, uint32_t key, uint8_t *value,
    uint32_t value_capacity, uint32_t *value_bytes);
void bx_ntvdm_mutation_overlay_v1_teardown(
    bx_ntvdm_mutation_overlay_v1 *overlay);

#ifdef __cplusplus
}
#endif

#endif
