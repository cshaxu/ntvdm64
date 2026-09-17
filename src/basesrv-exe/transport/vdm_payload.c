/* Finite copied-buffer binding for Base VDM messages. Original CSR capture
 * pointers cannot cross standalone IPC; original copy/capacity policy remains
 * in opennt-host/base/win32/{client/vdm.c,server/srvvdm.c}. */
#include "vdm_payload.h"
#include <string.h>

typedef char span_layout_must_be_16_bytes[(sizeof(broker_vdm_payload_span)==16)?1:-1];
#define PAYLOAD_HEADER_BYTES (16u * BROKER_VDM_PAYLOAD_FIELDS)

int broker_vdm_payload_encode(const broker_vdm_payload_input *input,
    void *output, uint32_t capacity, uint32_t *required)
{
    uint32_t total=PAYLOAD_HEADER_BYTES, i, offset;
    if (!required) return 0;
    *required=0;
    if (!input) return 0;
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        if (input[i].present>1 || (!input[i].present && input[i].data_bytes) ||
            (input[i].data_bytes && !input[i].data) ||
            input[i].data_bytes>UINT32_MAX-total) return 0;
        total+=input[i].data_bytes;
    }
    *required=total;
    if (!output) return 1;
    if (capacity<total) return 0;
    offset=PAYLOAD_HEADER_BYTES;
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        broker_vdm_payload_span span;
        span.present=input[i].present;
        span.length=input[i].length;
        span.data_bytes=input[i].data_bytes;
        span.offset=span.data_bytes?offset:0;
        memcpy((unsigned char *)output+i*16u,&span,sizeof(span));
        if (span.data_bytes) {
            memcpy((unsigned char *)output+offset,input[i].data,span.data_bytes);
            offset+=span.data_bytes;
        }
    }
    return 1;
}

int broker_vdm_payload_validate(const void *payload, uint32_t bytes)
{
    uint32_t i, offset=PAYLOAD_HEADER_BYTES;
    if (!payload || bytes<PAYLOAD_HEADER_BYTES) return 0;
    for (i=0;i<BROKER_VDM_PAYLOAD_FIELDS;++i) {
        broker_vdm_payload_span span;
        memcpy(&span,(const unsigned char *)payload+i*16u,sizeof(span));
        if (span.present>1 || (!span.present && span.data_bytes)) return 0;
        if (!span.data_bytes) {
            if (span.offset) return 0;
        } else {
            if (span.offset!=offset || span.data_bytes>bytes-offset) return 0;
            offset+=span.data_bytes;
        }
    }
    return offset==bytes;
}
