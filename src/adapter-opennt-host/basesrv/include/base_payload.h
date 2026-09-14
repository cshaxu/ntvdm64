#ifndef OPENNT_BASE_PAYLOAD_H
#define OPENNT_BASE_PAYLOAD_H
#include "broker/vdm_payload.h"
/* Base message types must already be declared. This binds only CheckVDM's
 * eight copied buffers, not scalar fields, authentication or resources. */
BOOL OpenNtBaseEncodeCheckPayload(const BASE_CHECKVDM_MSG *, void *, uint32_t, uint32_t *);
/* Output pointers borrow this mutable payload until the synchronous source
 * call finishes. Validate full operation/string semantics before dispatch.
 * On failure no message field changes. No allocation or policy is supplied. */
BOOL OpenNtBaseDecodeCheckPayload(void *, uint32_t, PBASE_CHECKVDM_MSG);
/* Message still holds the original request pointers/capacities. Validate all
 * spans before writes, then copy response bytes and update returned lengths.
 * Payload must not overlap destinations; no other message fields change. */
BOOL OpenNtBaseApplyGetPayload(const void *, uint32_t, PBASE_GET_NEXT_VDM_COMMAND_MSG);
#endif
