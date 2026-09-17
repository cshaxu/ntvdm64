#ifndef OPENNT_BASE_PAYLOAD_H
#define OPENNT_BASE_PAYLOAD_H
#include "basesrv-exe/transport/vdm_payload.h"
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
/* Per-call native storage, zero-initialized before prepare; never wire data.
 * The complete reply storage is reserved before original command consumption. */
typedef struct OPENNT_BASE_GET_PAYLOAD {
    void *bytes;
    uint32_t size;
    broker_vdm_payload_span spans[BROKER_VDM_PAYLOAD_FIELDS];
} OPENNT_BASE_GET_PAYLOAD;
BOOL OpenNtBaseEncodeGetRequest(const BASE_GET_NEXT_VDM_COMMAND_MSG *, void *, uint32_t, uint32_t *);
DWORD OpenNtBasePrepareGetPayload(const void *, uint32_t, PBASE_GET_NEXT_VDM_COMMAND_MSG, OPENNT_BASE_GET_PAYLOAD *);
BOOL OpenNtBaseFinishGetPayload(const BASE_GET_NEXT_VDM_COMMAND_MSG *, OPENNT_BASE_GET_PAYLOAD *);
/* Discard/restore the borrowed message pointers before releasing storage. */
void OpenNtBaseReleaseGetPayload(OPENNT_BASE_GET_PAYLOAD *);
#endif
