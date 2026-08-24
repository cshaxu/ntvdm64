#ifndef RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_H
#define RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_H

#include <stdint.h>

#include "generic_ud_bridge.h"

#define RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_MAGIC UINT32_C(0x4258554f)
#define RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_MAX_RECORDS 128u

/* Default-off copied record of existing generic #UD bridge events.  This is
 * selector-blind: it carries no decoded BOP fields, CPU registers, guest
 * memory, provider identity or retained host object. */
struct runtime_generic_ud_sequence_observation_record_v1 {
    uint16_t cs;
    uint16_t reserved0;
    uint32_t eip;
    uint32_t execution_mode;
    uint32_t window_bytes;
    uint8_t disposition;
    uint8_t reserved1[3];
    uint8_t window[RUNTIME_GENERIC_UD_WINDOW_BYTES];
};

struct runtime_generic_ud_sequence_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t record_count;
    uint32_t overflowed;
    struct runtime_generic_ud_sequence_observation_record_v1
        records[RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_V1_MAX_RECORDS];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_generic_ud_sequence_observation_v1_enable(uint32_t enabled);
int runtime_generic_ud_sequence_observation_v1_copy(
    struct runtime_generic_ud_sequence_observation_v1 *value);
void runtime_generic_ud_sequence_observation_v1_consider(
    const struct runtime_generic_ud_event_v1 *event,
    const struct runtime_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif