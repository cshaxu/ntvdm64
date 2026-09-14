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
    }
#define DECODE(i,p,n,t) message->p=spans[i].present? \
    (PCHAR)payload+(spans[i].data_bytes?spans[i].offset:sizeof(spans)):NULL; \
    message->n=(t)spans[i].length;
    CHECK_FIELDS(DECODE)
#undef DECODE
    return TRUE;
}
#undef CHECK_FIELDS
