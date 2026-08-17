#ifndef BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_H
#define BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAGIC UINT32_C(0x4258534f)
#define BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_VERSION UINT32_C(1)
#define BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS 128u

/* Default-off copied sequence of reached C4 C4 instructions. It records
 * accepted and declined invocations alike, but carries no CPU registers,
 * guest-memory data, provider identity, or semantic interpretation. */
struct bx_ntvdm_bop_sequence_observation_record_v1 {
    uint16_t cs;
    uint16_t reserved0;
    uint32_t eip;
    uint8_t selector;
    uint8_t service;
    uint8_t has_service;
    uint8_t disposition;
};

struct bx_ntvdm_bop_sequence_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t record_count;
    uint32_t overflowed;
    struct bx_ntvdm_bop_sequence_observation_record_v1
        records[BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS];
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_bop_sequence_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_bop_sequence_observation_v1_copy(
    struct bx_ntvdm_bop_sequence_observation_v1 *value);
void bx_ntvdm_bop_sequence_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
