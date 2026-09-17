#ifndef OPENNT_BASE_COMMAND_H
#define OPENNT_BASE_COMMAND_H
#include "basesrv-exe/transport/vdm_message.h"
/* Decoded CheckVDM ConsoleHandle carries only one of these service-local
 * semantic sentinels.  A native Console handle is never serialized. */
#define OPENNT_BASE_CONSOLE_NEW ((HANDLE)0)
#define OPENNT_BASE_CONSOLE_EXISTING ((HANDLE)1)
#define OPENNT_BASE_CONSOLE_WOW ((HANDLE)(LONG_PTR)-1)
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
/* Check reply carries only source output scalars/status; wait resources are
 * attached separately. Failure replies never read undefined output scalars. */
BOOL OpenNtBaseEncodeCheckReply(const BASE_API_MSG *,uint32_t request,uint32_t generation,
    void *,uint32_t capacity,uint32_t *required);
BOOL OpenNtBaseApplyCheckReply(const void *,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG);
/* Update request carries only scalar inputs; worker/wait handles are separate
 * authenticated resources. The caller must initialize the original task ID. */
BOOL OpenNtBaseEncodeUpdateCommand(const BASE_API_MSG *,uint32_t request,uint32_t generation,
    void *,uint32_t capacity,uint32_t *required);
BOOL OpenNtBaseDecodeUpdateCommand(const void *,uint32_t bytes,uint32_t generation,
    PBASE_API_MSG,uint32_t *request);
/* Update has no scalar output beyond NTSTATUS. Its parent wait handle is
 * separately bound before the original caller consumes a successful reply. */
BOOL OpenNtBaseEncodeUpdateReply(const BASE_API_MSG *,uint32_t request,uint32_t generation,
    void *,uint32_t capacity,uint32_t *required);
BOOL OpenNtBaseApplyUpdateReply(const void *,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG);
#include <base_payload.h>
/* Per-call native ownership. Initialize to zero; prepare reserves all reply
 * storage before source consumption. Never serialize this structure. */
typedef struct OPENNT_BASE_GET_COMMAND {
    OPENNT_BASE_GET_PAYLOAD payload;
    STARTUPINFOA startup;
    broker_vdm_message_header request;
    void *reply;
    uint32_t reply_bytes;
} OPENNT_BASE_GET_COMMAND;
BOOL OpenNtBaseEncodeGetCommand(const BASE_API_MSG *,uint32_t request,uint32_t generation,
    void *,uint32_t capacity,uint32_t *required);
DWORD OpenNtBasePrepareGetCommand(const void *,uint32_t bytes,uint32_t generation,
    PBASE_API_MSG,OPENNT_BASE_GET_COMMAND *);
BOOL OpenNtBaseFinishGetCommand(const BASE_API_MSG *,OPENNT_BASE_GET_COMMAND *);
BOOL OpenNtBaseApplyGetCommand(const void *,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG);
/* Restore/discard borrowed native pointers before release; repeat-safe. */
void OpenNtBaseReleaseGetCommand(OPENNT_BASE_GET_COMMAND *);
#endif
