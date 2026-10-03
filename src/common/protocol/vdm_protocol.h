#ifndef INTERFACE_VDM_PROTOCOL_H
#define INTERFACE_VDM_PROTOCOL_H
#include <stdint.h>

/* Shared copied protocol only. Encoding, validation and local pointers stay
 * with the service transport implementation. No layout/version change. */
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

/* Only numeric fields captured by original BaseCheckVDM. The command envelope
 * owns version/length; strings and resources use separate admitted bindings. */
typedef struct broker_vdm_startup {
    uint32_t present;
    uint32_t x, y, x_size, y_size, x_chars, y_chars, fill, flags, show;
} broker_vdm_startup;

#define BROKER_VDM_MESSAGE_VERSION 1u
enum broker_vdm_operation {
    BROKER_VDM_CHECK=1, BROKER_VDM_UPDATE, BROKER_VDM_GET_NEXT,
    BROKER_VDM_EXIT, BROKER_VDM_IS_FIRST, BROKER_VDM_EXIT_CODE,
    BROKER_VDM_REENTER, BROKER_VDM_SET_DIRECTORIES, BROKER_VDM_GET_DIRECTORIES,
    BROKER_VDM_BAT, BROKER_VDM_WOWEXEC
};
typedef struct broker_vdm_message_header {
    uint32_t version, bytes, operation, request_id, generation;
    uint32_t reply, status, payload_bytes;
} broker_vdm_message_header;

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
#endif
