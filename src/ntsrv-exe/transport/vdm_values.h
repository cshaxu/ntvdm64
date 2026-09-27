#ifndef BROKER_VDM_VALUES_H
#define BROKER_VDM_VALUES_H
#include <stdint.h>
/* Numeric fragments from original basemsg.h. No native pointer, resource,
 * Console identity or task policy. Envelope, buffers and attachments are
 * separate. Values are x86 little endian and operation-selected. */
typedef struct broker_vdm_check_values {
    uint32_t task, binary_type, code_page, creation_flags, drive, state;
    /* These are authenticated attachment receipts, never sender-local
     * HANDLE values.  They retain the original BASE_CHECKVDM_MSG fields so
     * BaseSrvDupStandardHandles remains the sole stream-copy owner. */
    uint32_t std_in, std_out, std_err;
} broker_vdm_check_values;
typedef struct broker_vdm_update_values {
    uint32_t task, binary_type, entry, creation_state;
} broker_vdm_update_values;
typedef struct broker_vdm_get_values {
    uint32_t task, code_page, creation_flags, exit_code, drive, state, from_bat;
    /* Presence only: native stream handles remain typed RPC attachments.
     * The Get reply never carries a sender- or worker-local HANDLE value. */
    uint32_t standard_mask;
} broker_vdm_get_values;
#endif
