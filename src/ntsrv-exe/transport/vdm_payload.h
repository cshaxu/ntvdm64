#ifndef BROKER_VDM_PAYLOAD_H
#define BROKER_VDM_PAYLOAD_H
#include <stdint.h>

#include "interface/vdm_protocol.h"
/* Local encode inputs only: no native pointer enters the copied record. */
typedef struct broker_vdm_payload_input {
    uint32_t present, length, data_bytes;
    const void *data;
} broker_vdm_payload_input;

/* Inputs must not overlap output. Query with output NULL; required is set on
 * valid input even when capacity is short. Invalid/short calls write no output.
 * No allocation or original capacity/consumption policy is supplied here. */
int broker_vdm_payload_encode(const broker_vdm_payload_input *, void *output,
    uint32_t capacity, uint32_t *required);
/* Validate exact canonical spans before any native pointer is materialized.
 * The caller additionally checks operation, direction, semantic string lengths
 * and authenticated owner. Not a validator for an entire RPC request. */
int broker_vdm_payload_validate(const void *, uint32_t bytes);
#endif
