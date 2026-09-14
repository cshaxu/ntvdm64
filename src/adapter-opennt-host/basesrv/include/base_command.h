#ifndef OPENNT_BASE_COMMAND_H
#define OPENNT_BASE_COMMAND_H
#include "broker/vdm_message.h"
/* Complete copied CheckVDM request: envelope, scalar/startup fragments and
 * eight buffers. Authenticated attachments remain a separate mandatory path.
 * Source/output must not overlap; NULL output queries required bytes. */
BOOL OpenNtBaseEncodeCheckCommand(const BASE_API_MSG *, uint32_t request,
    uint32_t generation, void *, uint32_t capacity, uint32_t *required);
/* Authenticated registration supplies generation. Message resource fields
 * must already be locally bound, never copied from the sender. They remain
 * unchanged. Payload pointers borrow input; StartupInfo borrows startup.
 * Both outputs stay unchanged on invalid input. Storage must not overlap. */
BOOL OpenNtBaseDecodeCheckCommand(void *, uint32_t bytes, uint32_t generation,
    PBASE_API_MSG, STARTUPINFOA *, uint32_t *request);
#endif
