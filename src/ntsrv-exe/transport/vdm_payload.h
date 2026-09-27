#ifndef BROKER_VDM_PAYLOAD_H
#define BROKER_VDM_PAYLOAD_H
#include <stdint.h>

/* CheckVDM/GetNextVDMCommand variable fields, in this fixed order. This is
 * only the copied-buffer portion, not a complete command or authentication. */
enum broker_vdm_payload_field {
    BROKER_VDM_COMMAND, BROKER_VDM_APPLICATION, BROKER_VDM_PIF,
    BROKER_VDM_DIRECTORY, BROKER_VDM_ENVIRONMENT, BROKER_VDM_DESKTOP,
    BROKER_VDM_TITLE, BROKER_VDM_RESERVED, BROKER_VDM_PAYLOAD_FIELDS
};
/* Wire scalars are little-endian, as in the selected x86 RPC composition.
 * length is the original input capacity/output required length. It is not
 * inferred from data_bytes; NULL buffers and capacity replies remain distinct.
 * data_bytes may exceed length: original PIF output writes a NUL even when
 * its returned required length is zero. Operation bindings must validate bytes
 * against the independently retained actual buffer capacity. */
typedef struct broker_vdm_payload_span {
    uint32_t present, length, data_bytes, offset;
} broker_vdm_payload_span;
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
