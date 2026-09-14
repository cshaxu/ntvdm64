/* Compose copied original command fields at the unavailable CSR transport
 * boundary. No selection, retry, resource duplication or record policy. */
#include "basesrv.h"
#include <base_command.h>
#include <base_payload.h>
#include <base_values.h>
#include <base_startup.h>
#include <string.h>
typedef struct check_prefix {
    broker_vdm_message_header header;
    broker_vdm_check_values values;
    broker_vdm_startup startup;
} check_prefix;
typedef char check_prefix_size[sizeof(check_prefix)==96?1:-1];

BOOL OpenNtBaseEncodeCheckCommand(const BASE_API_MSG *message, uint32_t request,
    uint32_t generation, void *output, uint32_t capacity, uint32_t *required)
{
    check_prefix prefix={0};
    uint32_t payloadBytes,total;
    if (required) *required=0;
    if (!message || !required || !request || !generation ||
        !OpenNtBaseEncodeCheckPayload(&message->u.CheckVDM,NULL,0,&payloadBytes) ||
        payloadBytes>UINT32_MAX-sizeof(prefix)) return FALSE;
    total=sizeof(prefix)+payloadBytes;
    if (!OpenNtBaseEncodeValues(message,BROKER_VDM_CHECK,&prefix.values,sizeof(prefix.values))) return FALSE;
    OpenNtBaseEncodeStartup(message->u.CheckVDM.StartupInfo,&prefix.startup);
    prefix.header.version=BROKER_VDM_MESSAGE_VERSION;
    prefix.header.bytes=total; prefix.header.operation=BROKER_VDM_CHECK;
    prefix.header.request_id=request; prefix.header.generation=generation;
    prefix.header.payload_bytes=total-sizeof(prefix.header);
    *required=total;
    if (!output) return TRUE;
    if (capacity<total) return FALSE;
    if (!OpenNtBaseEncodeCheckPayload(&message->u.CheckVDM,
        (unsigned char *)output+sizeof(prefix),payloadBytes,&payloadBytes)) return FALSE;
    memcpy(output,&prefix,sizeof(prefix));
    return TRUE;
}

BOOL OpenNtBaseDecodeCheckCommand(void *input, uint32_t bytes, uint32_t generation,
    PBASE_API_MSG message, STARTUPINFOA *startup, uint32_t *request)
{
    check_prefix prefix;
    broker_vdm_message_header header;
    BASE_API_MSG decoded;
    STARTUPINFOA decodedStartup;
    if (!message || !startup || !request || bytes<sizeof(prefix) ||
        !broker_vdm_message_read(input,bytes,generation,0,&header) ||
        header.operation!=BROKER_VDM_CHECK) return FALSE;
    memcpy(&prefix,input,sizeof(prefix));
    decoded=*message;
    if (!OpenNtBaseDecodeValues(&prefix.values,sizeof(prefix.values),BROKER_VDM_CHECK,&decoded) ||
        !OpenNtBaseDecodeStartup(&prefix.startup,&decodedStartup) ||
        !OpenNtBaseDecodeCheckPayload((unsigned char *)input+sizeof(prefix),
            bytes-sizeof(prefix),&decoded.u.CheckVDM)) return FALSE;
    decoded.u.CheckVDM.StartupInfo=prefix.startup.present?startup:NULL;
    *startup=decodedStartup;
    *message=decoded;
    *request=header.request_id;
    return TRUE;
}
