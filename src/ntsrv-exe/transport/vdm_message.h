#ifndef BROKER_VDM_MESSAGE_H
#define BROKER_VDM_MESSAGE_H
#include <stdint.h>
#include "common/protocol/vdm_protocol.h"
/* x86 little-endian envelope only. No sender identity, native value or generic
 * CSR message is represented. Operation-specific body validation is mandatory.
 * trusted_generation comes from an authenticated registration; it is not read
 * from the message being validated. expected_reply is 0=request or 1=reply.
 * A matching request ID correlates a reply; it is not authority to replay. */
int broker_vdm_message_read(const void *, uint32_t bytes,
    uint32_t trusted_generation, uint32_t expected_reply,
    broker_vdm_message_header *);
#endif
