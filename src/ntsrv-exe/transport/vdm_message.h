#ifndef BROKER_VDM_MESSAGE_H
#define BROKER_VDM_MESSAGE_H
#include <stdint.h>
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
/* x86 little-endian envelope only. No sender identity, native value or generic
 * CSR message is represented. Operation-specific body validation is mandatory.
 * trusted_generation comes from an authenticated registration; it is not read
 * from the message being validated. expected_reply is 0=request or 1=reply.
 * A matching request ID correlates a reply; it is not authority to replay. */
int broker_vdm_message_read(const void *, uint32_t bytes,
    uint32_t trusted_generation, uint32_t expected_reply,
    broker_vdm_message_header *);
#endif
