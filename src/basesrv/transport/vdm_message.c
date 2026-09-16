/* Finite transport envelope replacing excluded CSR port metadata, not its
 * dispatch, task or replay policy. Original Base VDM owners remain unchanged. */
#include "vdm_message.h"
#include <string.h>
typedef char message_header_size[(sizeof(broker_vdm_message_header)==32)?1:-1];
int broker_vdm_message_read(const void *message, uint32_t bytes,
    uint32_t trusted_generation, uint32_t expected_reply,
    broker_vdm_message_header *out)
{
    broker_vdm_message_header header;
    if (!message || !out || bytes<sizeof(header) || !trusted_generation || expected_reply>1) return 0;
    memcpy(&header,message,sizeof(header));
    if (header.version!=BROKER_VDM_MESSAGE_VERSION || header.bytes!=bytes ||
        header.payload_bytes!=bytes-sizeof(header) ||
        header.operation<BROKER_VDM_CHECK || header.operation>BROKER_VDM_WOWEXEC ||
        !header.request_id || header.generation!=trusted_generation ||
        header.reply!=expected_reply || (!header.reply && header.status)) return 0;
    *out=header;
    return 1;
}
