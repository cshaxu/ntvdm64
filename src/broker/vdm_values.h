#ifndef BROKER_VDM_VALUES_H
#define BROKER_VDM_VALUES_H
#include <stdint.h>
/* Numeric fragments from original basemsg.h. No native pointer, resource,
 * Console identity or task policy. Envelope, buffers and attachments are
 * separate. Values are x86 little endian and operation-selected. */
typedef struct broker_vdm_check_values {
    uint32_t task, binary_type, code_page, creation_flags, drive, state;
} broker_vdm_check_values;
typedef struct broker_vdm_update_values {
    uint32_t task, binary_type, entry, creation_state;
} broker_vdm_update_values;
typedef struct broker_vdm_get_values {
    uint32_t task, code_page, creation_flags, exit_code, drive, state, from_bat;
} broker_vdm_get_values;
#endif
