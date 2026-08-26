#ifndef RUNTIME_BOP_SEQUENCE_OBSERVATION_H
#define RUNTIME_BOP_SEQUENCE_OBSERVATION_H

#include <stdint.h>

#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_BOP_SEQUENCE_OBSERVATION_MAGIC UINT32_C(0x4258534f)
#define RUNTIME_BOP_SEQUENCE_OBSERVATION_VERSION UINT32_C(2)
#define RUNTIME_BOP_SEQUENCE_OBSERVATION_MAX_RECORDS 128u

/* Default-off copied sequence of reached C4 C4 instructions. It records
 * accepted and declined invocations alike, with a fixed-width pre-dispatch
 * CPU snapshot only; it carries no guest-memory data, provider identity, or semantic interpretation. */
struct runtime_bop_sequence_observation_record {
    uint16_t cs;
    uint16_t reserved0;
    uint32_t eip;
    uint8_t selector;
    uint8_t service;
    uint8_t has_service;
    uint8_t disposition;
    uint32_t eax, ebx, ecx, edx, esi, edi, eflags;
    uint16_t ds, es, ss;
    uint16_t reserved1;
};

struct runtime_bop_sequence_observation {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t record_count;
    uint32_t overflowed;
    struct runtime_bop_sequence_observation_record
        records[RUNTIME_BOP_SEQUENCE_OBSERVATION_MAX_RECORDS];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_bop_sequence_observation_enable(uint32_t enabled);
int runtime_bop_sequence_observation_copy(
    struct runtime_bop_sequence_observation *value);
void runtime_bop_sequence_observation_consider(
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome);

#ifdef __cplusplus
}
#endif

#endif
