/* Native-field binding for original basemsg.h BASE_CHECKVDM_MSG. Original
 * capture/copy policy stays in vdm.c/srvvdm.c; only unavailable CSR address
 * translation is replaced by explicitly copied, lifetime-bounded bytes. */
#include "basesrv.h"
#include <base_payload.h>
#include <string.h>
#define CHECK_FIELDS(X) \
    X(0,CmdLine,CmdLen,USHORT) X(1,AppName,AppLen,USHORT) \
    X(2,PifFile,PifLen,USHORT) X(3,CurDirectory,CurDirectoryLen,USHORT) \
    X(4,Env,EnvLen,ULONG) X(5,Desktop,DesktopLen,ULONG) \
    X(6,Title,TitleLen,ULONG) X(7,Reserved,ReservedLen,ULONG)

BOOL OpenNtBaseEncodeCheckPayload(const BASE_CHECKVDM_MSG *message,
    void *output, uint32_t capacity, uint32_t *required)
{
    broker_vdm_payload_input input[BROKER_VDM_PAYLOAD_FIELDS];
    if (!message) { if (required) *required=0; return FALSE; }
#define ENCODE(i,p,n,t) input[i].present=message->p!=NULL; input[i].length=message->n; \
    input[i].data_bytes=message->p?message->n:0; input[i].data=message->p;
    CHECK_FIELDS(ENCODE)
#undef ENCODE
    return broker_vdm_payload_encode(input,output,capacity,required);
}

BOOL OpenNtBaseDecodeCheckPayload(void *payload, uint32_t bytes,
    PBASE_CHECKVDM_MSG message)
{
    broker_vdm_payload_span spans[BROKER_VDM_PAYLOAD_FIELDS];
    uint32_t i;
    if (!message || !broker_vdm_payload_validate(payload,bytes)) return FALSE;
    memcpy(spans,payload,sizeof(spans));
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        if (spans[i].length!=spans[i].data_bytes ||
            (!spans[i].present && spans[i].length) ||
            (i<4 && spans[i].length>0xffffu)) return FALSE;
        if (spans[i].present) {
            const unsigned char *data=(const unsigned char *)payload+spans[i].offset;
            if (!spans[i].length || data[spans[i].length-1]!=0) return FALSE;
            /* Original BaseCreateVDMEnvironment emits one NUL for an empty
             * environment; nonempty entries end in a double NUL. */
            if (i==BROKER_VDM_ENVIRONMENT && spans[i].length>1 &&
                data[spans[i].length-2]!=0) return FALSE;
        }
    }
    if (!spans[BROKER_VDM_COMMAND].present) return FALSE;
#define DECODE(i,p,n,t) message->p=spans[i].present? \
    (PCHAR)payload+(spans[i].data_bytes?spans[i].offset:sizeof(spans)):NULL; \
    message->n=(t)spans[i].length;
    CHECK_FIELDS(DECODE)
#undef DECODE
    return TRUE;
}
BOOL OpenNtBaseApplyGetPayload(const void *payload, uint32_t bytes,
    PBASE_GET_NEXT_VDM_COMMAND_MSG message)
{
    broker_vdm_payload_span spans[BROKER_VDM_PAYLOAD_FIELDS];
    if (!message || !broker_vdm_payload_validate(payload,bytes)) return FALSE;
    memcpy(spans,payload,sizeof(spans));
#define CHECK_REPLY(i,p,n,t) if (spans[i].present!=(uint32_t)(message->p!=NULL) || \
    spans[i].data_bytes>message->n || (i<4 && spans[i].length>0xffffu)) return FALSE;
    CHECK_FIELDS(CHECK_REPLY)
#undef CHECK_REPLY
#define APPLY_REPLY(i,p,n,t) if (spans[i].data_bytes) \
    memcpy(message->p,(const unsigned char *)payload+spans[i].offset,spans[i].data_bytes); \
    message->n=(t)spans[i].length;
    CHECK_FIELDS(APPLY_REPLY)
#undef APPLY_REPLY
    return TRUE;
}
BOOL OpenNtBaseEncodeGetRequest(const BASE_GET_NEXT_VDM_COMMAND_MSG *message,
    void *output, uint32_t capacity, uint32_t *required)
{
    broker_vdm_payload_input input[BROKER_VDM_PAYLOAD_FIELDS];
    if (!message) { if (required) *required=0; return FALSE; }
#define GET_REQUEST(i,p,n,t) input[i].present=message->p!=NULL; input[i].length=message->n; \
    input[i].data_bytes=0; input[i].data=NULL;
    CHECK_FIELDS(GET_REQUEST)
#undef GET_REQUEST
    return broker_vdm_payload_encode(input,output,capacity,required);
}

DWORD OpenNtBasePrepareGetPayload(const void *payload, uint32_t bytes,
    PBASE_GET_NEXT_VDM_COMMAND_MSG message, OPENNT_BASE_GET_PAYLOAD *state)
{
    broker_vdm_payload_span spans[BROKER_VDM_PAYLOAD_FIELDS];
    uint32_t i,total=sizeof(spans),offset=sizeof(spans);
    void *storage;
    if (!message || !state || state->bytes || !broker_vdm_payload_validate(payload,bytes))
        return ERROR_INVALID_PARAMETER;
    memcpy(spans,payload,sizeof(spans));
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        if (spans[i].data_bytes || (i<4 && spans[i].length>0xffffu)) return ERROR_INVALID_PARAMETER;
        if (spans[i].present) {
            if (spans[i].length>UINT32_MAX-total) return ERROR_ARITHMETIC_OVERFLOW;
            total+=spans[i].length;
        }
        spans[i].data_bytes=spans[i].present?spans[i].length:0;
        spans[i].offset=spans[i].data_bytes?offset:0;
        offset+=spans[i].data_bytes;
    }
    storage=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total);
    if (!storage) return ERROR_NOT_ENOUGH_MEMORY;
    state->bytes=storage; state->size=total;
    memcpy(state->spans,spans,sizeof(spans));
#define GET_BUFFER(i,p,n,t) message->p=spans[i].present? \
    (PCHAR)storage+(spans[i].data_bytes?spans[i].offset:sizeof(spans)):NULL; message->n=(t)spans[i].length;
    CHECK_FIELDS(GET_BUFFER)
#undef GET_BUFFER
    return ERROR_SUCCESS;
}

BOOL OpenNtBaseFinishGetPayload(const BASE_GET_NEXT_VDM_COMMAND_MSG *message,
    OPENNT_BASE_GET_PAYLOAD *state)
{
    broker_vdm_payload_span spans[BROKER_VDM_PAYLOAD_FIELDS];
    if (!message || !state || !state->bytes) return FALSE;
    memcpy(spans,state->spans,sizeof(spans));
#define GET_RESULT(i,p,n,t) spans[i].length=message->n;
    CHECK_FIELDS(GET_RESULT)
#undef GET_RESULT
    /* Data is already in this reply allocation. No post-consumption allocation
     * or fallible resource acquisition is introduced by reply construction. */
    memcpy(state->bytes,spans,sizeof(spans));
    return TRUE;
}

void OpenNtBaseReleaseGetPayload(OPENNT_BASE_GET_PAYLOAD *state)
{
    if (!state) return;
    if (state->bytes) HeapFree(GetProcessHeap(),0,state->bytes);
    memset(state,0,sizeof(*state));
}
#undef CHECK_FIELDS
